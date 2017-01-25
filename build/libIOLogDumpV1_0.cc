/**
 **     File:  libIOLOgDumpV1_0.cc
 **    Authors:  Sai Susarla, Jerry Fredin
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
#include "IOLogDump.h"

const char *opnames[] = {
	"READ_IN",
	"WRITE_IN",
	"READ_OUT",
	"WRITE_OUT"
};

void to_native_IOLogDumpHeader(struct IOLogDump *dump, struct IOLogDumpHeader *rec)
{
    rec->sig = dump->to_native(rec->sig);
    rec->version_minor = dump->to_native(rec->version_minor);
    rec->version_major = dump->to_native(rec->version_major);
    rec->start_byteoffset = dump->to_native(rec->start_byteoffset);
    rec->end_byteoffset = dump->to_native(rec->end_byteoffset);
    rec->start_time = dump->to_native(rec->start_time);
    rec->end_time = dump->to_native(rec->end_time);
    rec->start_ticks = dump->to_native(rec->start_ticks);
    rec->end_ticks = dump->to_native(rec->end_ticks);
    rec->ts_rate = dump->to_native(rec->ts_rate);
    rec->dumpvol_size = dump->to_native(rec->dumpvol_size);
    rec->dumpvol_ssid = dump->to_native(rec->dumpvol_ssid);
    rec->rec_size = dump->to_native(rec->rec_size);
    rec->hdr_size = dump->to_native(rec->hdr_size);
    rec->hdr_savethres = dump->to_native(rec->hdr_savethres);
    rec->ctlr_id = dump->to_native(rec->ctlr_id);
    rec->nvolumes = dump->to_native(rec->nvolumes);
}

void to_native_IOLogTraceCmdInRecord(struct IOLogDump *dump, struct IOLogTraceCmdInRecord *rec)
{
    rec->ts = dump->to_native(rec->ts);
    rec->seqid = dump->to_native(rec->seqid);
    rec->lun = dump->to_native(rec->lun);
    rec->lba = dump->to_native(rec->lba);
    rec->nblks = dump->to_native(rec->nblks);
}

void to_native_IOLogTraceCmdOutRecord(struct IOLogDump *dump, struct IOLogTraceCmdOutRecord *rec)
{
    rec->ts = dump->to_native(rec->ts);
    rec->seqid = dump->to_native(rec->seqid);
    rec->lun = dump->to_native(rec->lun);
    rec->latency_ticks = dump->to_native(rec->latency_ticks);
    rec->host_id = dump->to_native(rec->host_id);
    rec->host_lun = dump->to_native(rec->host_lun);
}

void to_native_IOLogStatsRecord(struct IOLogDump *dump, struct IOLogStatsRecord *rec)
{
    rec->ts = dump->to_native(rec->ts);
    rec->dump_intvl = dump->to_native(rec->dump_intvl);
    rec->obj_type = dump->to_native(rec->obj_type);
    rec->obj_id = dump->to_native(rec->obj_id);
    rec->nreqs = dump->to_native(rec->nreqs);
    rec->nreads = dump->to_native(rec->nreads);
    rec->nread_blks = dump->to_native(rec->nread_blks);
    rec->cum_read_ticks = dump->to_native(rec->cum_read_ticks);
    rec->cache_hits = dump->to_native(rec->cache_hits);
    rec->cum_cache_read_ticks = dump->to_native(rec->cum_cache_read_ticks);
    rec->nwrites = dump->to_native(rec->nwrites);
    rec->nwrite_blks = dump->to_native(rec->nwrite_blks);
    rec->cum_write_ticks = dump->to_native(rec->cum_write_ticks);
    rec->read_io_ticks = dump->to_native(rec->read_io_ticks);
    rec->io_wait_ticks = dump->to_native(rec->io_wait_ticks);
    rec->prefetch_requests = dump->to_native(rec->prefetch_requests);
    rec->prefetch_hits = dump->to_native(rec->prefetch_hits);
    rec->prefetch_miss_exp_hit = dump->to_native(rec->prefetch_miss_exp_hit);
    rec->prefetch_hit_exp_miss = dump->to_native(rec->prefetch_hit_exp_miss);
    rec->prefetch_performed = dump->to_native(rec->prefetch_performed);
    rec->prefetch_blks = dump->to_native(rec->prefetch_blks);
    rec->idle_time = dump->to_native(rec->idle_time);
    rec->evict_prefetchhit = dump->to_native(rec->evict_prefetchhit);
    rec->evict_prefetch = dump->to_native(rec->evict_prefetch);
    rec->evict_hit2 = dump->to_native(rec->evict_hit2);
    rec->evict_fetch = dump->to_native(rec->evict_fetch);
    rec->evict_write = dump->to_native(rec->evict_write);
    rec->evict_writehit = dump->to_native(rec->evict_writehit);
    rec->evict_prefetch_update = dump->to_native(rec->evict_prefetch_update);
    rec->evict_prefetch_updatehit = dump->to_native(rec->evict_prefetch_updatehit);
    rec->evict_total = dump->to_native(rec->evict_total);
    rec->cacheblks_inuse = dump->to_native(rec->cacheblks_inuse);
}

