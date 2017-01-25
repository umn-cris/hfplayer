/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/**
 **    File:  HFParsed.cc
 **    Authors:  Sai Susarla, Weiping He, Jerry Fredin, Ibra Fall
 **
 ******************************************************************************
 **
 **    Copyright 2012 NetApp, Inc.
 **
 **    Licensed under the Apache License, Version 2.0 (the "License");
 **    you may not use this file except in compliance with the License.
 **    You may obtain a copy of the License at
 **
 **    http://www.apache.org/licenses/LICENSE-2.0
 **
 **    Unless required by applicable law or agreed to in writing, software
 **    distributed under the License is distributed on an "AS IS" BASIS,
 **    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 **    See the License for the specific language governing permissions and
 **    limitations under the License.
 **
 ******************************************************************************
 **/

#include "HFParsed.h"

// WARNING these op strings must be < 7 chars as they're stored in a ULONG64
// field in DRecord::FieldType.
string* IOLogTrace::opnames[] = { new string("RD_IN"),
            new string("WR_IN"),
            new string("RD_OUT"),
            new string("WR_OUT")
};
const unsigned IOLogTrace::max_optypes = sizeof(IOLogTrace::opnames) / (2 * sizeof(IOLogTrace::opnames[0]));


bool IOLogTrace::start()
{
    if(! dump || ! dump->start())
        return false;

    if(dump->schema_defs->count("IOLogTraceCmdHdr") == 0 ||
            dump->schema_defs->count("IOLogTraceCmdIn") == 0 ||
            dump->schema_defs->count("IOLogTraceCmdOut") == 0)
    {
        fprintf(stderr, "IOLogTrace schema missing.\n");
        return false;
    }

    dcmdhdr_meta = (*dump->schema_defs)["IOLogTraceCmdHdr"];
    dcmdin_meta = (*dump->schema_defs)["IOLogTraceCmdIn"];
    dcmdout_meta = (*dump->schema_defs)["IOLogTraceCmdOut"];

    if(dump->header["rec_size"].i < dcmdin_meta->dumpsize)
    {
        fprintf(stderr, "Trace record size mismatch: " FMT_ULONG64 " in file, %d in schema\n",
                dump->header["rec_size"].i, dcmdin_meta->dumpsize);
        return false;
    }

    /* Create a unified and augmented trace record schema */
    cmdall_schema.add("dump_offset");
    cmdall_schema.add("elapsed_usecs", DRecordSchema::MyDouble);
    cmdall_schema.add("elapsed_ticks");
    // human-readable op name
    cmdall_schema.add("cmd", DRecordSchema::MyStr);
    cmdall_schema.add("inflight_ios");
    // Add all cmdhdr fields (common to both cmdin and cmdout records)
    cmdall_schema.append(dcmdhdr_meta->schema);
    // Add all fields in cmdin after skipping cmdhdr fields
    int nhdr_fields = dcmdhdr_meta->schema->fields.size();
    cmdall_schema.append(dcmdin_meta->schema, nhdr_fields);
    // Add all fields in cmdout after skipping cmdhdr fields
    cmdall_schema.append(dcmdout_meta->schema, nhdr_fields);
    cmdall_schema.add("latency_usecs", DRecordSchema::MyDouble);
    // Make opcode human-readable
    // cmdout_schema.set("op", DRecordSchema::MyStr, cmdout_schema.indices["op"]);
    cur_cmd.set_schema(&cmdall_schema);
    cur_cmdhdr.set_schema(dcmdhdr_meta->schema);
    cum_ticks = 0;
    start_ticks = 0;
    max_seqid = 0;
    seqid = 0;
    nrecs = 0;
    nrecs_invalid = 0;
    numios_inflight = 0;
    has_ts = dcmdhdr_meta->schema->has("ts");
    has_phase = dcmdhdr_meta->schema->has("phase");
    inflight_ios.clear();
    return true;
}

IOLogTraceRecordParsed_t* IOLogTrace::next()
{
    ULONG64 dump_offset = dump->curoffset;
    void* buf = dump->next();

    if(! buf)
        return NULL;

    ++ nrecs;
    dcmdhdr_meta->to_native_fn(dump, buf, cur_cmdhdr);
    seqid = cur_cmdhdr["seqid"].i;

    if(! verify_trace_record(&cur_cmdhdr))
    {
//      fprintf(stderr, "skipping record %u, Seqid " FMT_ULONG64 ": Invalid contents\n", dump->recnum, seqid);
        curRec.isValid = false;
        ++ nrecs_invalid;
        return &curRec;
    }

    // ignore reply packets
    cum_ticks = has_ts ? cur_cmdhdr["ts"].i : cum_ticks + cur_cmdhdr["ts_incr"].i;

    if(nrecs == 1)
    {
        start_ticks = cum_ticks;
    }

    curRec.isValid = true;
    curRec.cmdhdr = &cur_cmdhdr;

    if((dump->end_ts > 0) && ((cum_ticks < dump->start_ts) ||
                              (cum_ticks >= dump->end_ts)))
    {
        curRec.isValid = false;
        ++ nrecs_invalid;
    }

    int nhdr_fields = cur_cmdhdr.schema->fields.size();
    /* Is it a CMD-IN or a CMD-OUT record? */
    DRecordData dcmd; // record in the raw dump

    if(has_phase)
    {
        /* Dump Version 1.0 or later */
        curRec.isCmdIn = (cur_cmdhdr["phase"].i == 0);

        if(curRec.isCmdIn)
            dcmdin_meta->to_native_fn(dump, buf, dcmd);
    }
    else
        if(seqid <= max_seqid)
        {
            curRec.isCmdIn = false;
        }
        else
        {
            /* In trace dump version < 1.0, there's no clear indicator of a
             * CMD out record. Have to do some guess work.
             * First, let's assume that it is a CMD_IN record (most frequent
             * case) and parse it as such. */
            dcmdin_meta->to_native_fn(dump, buf, dcmd);
            curRec.isCmdIn = (dcmd["nblks"].i <= (MAX_IO_SIZE / DISK_BLOCK_SIZE));
        }

    if(max_seqid < seqid)
        max_seqid = seqid;

    if(curRec.isCmdIn)
    {
        /* New Request */
        curRec.cmdout = 0;
        DRecordData* new_cmd = new DRecordData(&cmdall_schema);
        curRec.cmdin = new_cmd;
        new_cmd->zero();
        (*new_cmd)["dump_offset"].i = dump_offset;
        (*new_cmd)["elapsed_usecs"].d = dump->ticks2us(cum_ticks);
        (*new_cmd)["elapsed_ticks"].i = cum_ticks;
        int opcode = dcmd["op"].i;
        (*new_cmd)["cmd"].s = opnames[opcode];
        (*new_cmd)["inflight_ios"].i = numios_inflight;
        new_cmd->copy(&dcmd);
        inflight_ios[seqid] = new_cmd;
        ++ numios_inflight;
    }
    else
    {
        /* Reply */
        curRec.cmdin = 0;
        curRec.cmdout = &cur_cmd;
        cur_cmd.zero();
        cur_cmd["dump_offset"].i = dump_offset;
        cur_cmd["elapsed_usecs"].d = dump->ticks2us(cum_ticks);
        cur_cmd["elapsed_ticks"].i = cum_ticks;
        cur_cmd["inflight_ios"].i = numios_inflight;
        dcmd.set_schema(0);
        dcmdout_meta->to_native_fn(dump, buf, dcmd);
        cur_cmd.copy(&dcmd);
        int opcode = dcmd["op"].i + max_optypes;
        cur_cmd["op"].i = opcode;
        cur_cmd["cmd"].s = opnames[opcode];
        cur_cmd["latency_usecs"].d = dump->ticks2us(dcmd["latency_ticks"].i);
        map<ULONG64, DRecordData*>::iterator it = inflight_ios.find(seqid);

        if(it != inflight_ios.end())
        {
            // Found the cmdin record. Save the cmd completion info there.
            DRecordData* ref_cmdin = (*it).second;
            curRec.cmdin = ref_cmdin;
            ref_cmdin->copy(&dcmd, nhdr_fields);
            (*ref_cmdin)["latency_usecs"].d = cur_cmd["latency_usecs"].d;
            cur_cmd.copy(ref_cmdin, nhdr_fields,
                         dcmdin_meta->schema->fields.size() - nhdr_fields);
//          fprintf(stderr, "req = %s\n", ref_cmdin->str(",").c_str());
//          inflight_ios.erase(it);
//          delete ref_cmdin;
            -- numios_inflight;
        }
        else
        {
//          fprintf(stderr, "missed seqid " FMT_ULONG64 "\n", seqid);
        }
    }

    return &curRec;
}

bool IOLogStats::start()
{
    if(! dump || ! dump->start())
        return false;

    if(dump->schema_defs->count("IOLogStats") == 0)
    {
        fprintf(stderr, "IOLogStats schema missing.\n");
        return false;
    }

    dstats_meta = (*dump->schema_defs)["IOLogStats"];

    if(dump->header["rec_size"].i < dstats_meta->dumpsize)
    {
        fprintf(stderr, "Stats record size mismatch: " FMT_ULONG64 " in file, %d in schema\n",
                dump->header["rec_size"].i, dstats_meta->dumpsize);
        return false;
    }

    stats_schema.add("elapsed_secs", DRecordSchema::MyDouble);
    stats_schema.add("rdlat_usecs", DRecordSchema::MyDouble);
    stats_schema.add("cache_rdlat_usecs", DRecordSchema::MyDouble);
    stats_schema.add("rd_iops", DRecordSchema::MyDouble);
    stats_schema.add("rd_qdepth", DRecordSchema::MyDouble);
    stats_schema.add("rdhit_pcnt", DRecordSchema::MyDouble);
    stats_schema.add("rd_bw", DRecordSchema::MyDouble);
    stats_schema.add("wrlat_usecs", DRecordSchema::MyDouble);
    stats_schema.add("wr_iops", DRecordSchema::MyDouble);
    stats_schema.add("wr_qdepth", DRecordSchema::MyDouble);
    stats_schema.add("wr_bw", DRecordSchema::MyDouble);

    for(size_t i = 0; i < dstats_meta->schema->fields.size(); ++ i)
        stats_schema.add(dstats_meta->schema->fields[i], dstats_meta->schema->types[i]);

    cur_stats.set_schema(&stats_schema);
    prev_ts = 0;
    start_ts = 0;
    nrecs = 0;
    dump_intvl_in_ticks = stats_schema.has("intvl_ticks");
    return true;
}

IOLogStatsParsed_t* IOLogStats::next()
{
    void* buf = dump->next();

    if(! buf)
        return NULL;

    dstats_meta->to_native_fn(dump, buf, cur_stats);
    ULONG64 ts = cur_stats["ts"].i;

    if(dump->recnum == 1)
    {
        start_ts = prev_ts = ts;
    }

    if(ts < prev_ts)
        return NULL;

    ++ nrecs;
    curRec.isValid = true;

    if(! verify_record(&cur_stats))
    {
//      fprintf(stderr, "skipping record %u, Seqid " FMT_ULONG64 ": Invalid contents\n", dump->recnum, seqid);
        curRec.isValid = false;
        curRec.stats = &cur_stats;
        ++ nrecs_invalid;
        return &curRec;
    }

    double intvl = dump_intvl_in_ticks
                   ? dump->ticks2sec(cur_stats["intvl_ticks"].i)
                   : cur_stats["intvl_msecs"].i / 1000.0;
    double cum_secs = dump->ticks2us(ts - start_ts) / 1000000.0;
    ULONG64 nreads = cur_stats["nreads"].i;
    ULONG64 nwrites = cur_stats["nwrites"].i;
    ULONG64 cache_hits = cur_stats["cache_hits"].i;
    prev_ts = ts;
    cur_stats["elapsed_secs"].d = cum_secs;
    cur_stats["rdlat_usecs"].d = (nreads == 0) ? 0 : dump->ticks2us(cur_stats["cum_read_ticks"].i / nreads);
    cur_stats["cache_rdlat_usecs"].d = (cache_hits == 0) ? 0 : dump->ticks2us(cur_stats["cum_cache_read_ticks"].i / cache_hits);
    cur_stats["rd_iops"].d = (intvl == 0) ? 0 : (nreads / intvl);
    cur_stats["rd_qdepth"].d = (intvl == 0) ? 0 : dump->ticks2us(cur_stats["cum_read_ticks"].i / intvl) / 1000000.0;
    cur_stats["rdhit_pcnt"].d = (nreads == 0) ? 0 : (cache_hits * 100.0 / nreads);
    cur_stats["rd_bw"].d = (intvl == 0) ? 0 : (cur_stats["nread_blks"].i * 512 / intvl);
    cur_stats["wrlat_usecs"].d = (nwrites == 0) ? 0 : dump->ticks2us(cur_stats["cum_write_ticks"].i / nwrites);
    cur_stats["wr_iops"].d = (intvl == 0) ? 0 : (nwrites / intvl);
    cur_stats["wr_qdepth"].d = (intvl == 0) ? 0 : dump->ticks2us(cur_stats["cum_write_ticks"].i / intvl) / 1000000.0;
    cur_stats["wr_bw"].d = (intvl == 0) ? 0 : (cur_stats["nwrite_blks"].i * 512 / intvl);
    curRec.isValid = true;
    curRec.stats = &cur_stats;
    return &curRec;
}
