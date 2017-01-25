/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/**
 **    File:  HFParsed.h
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

#include <string.h>
#include <string>
#include <vector>

#include "HFPlayer.h"

using namespace std;

typedef struct IOLogTraceRecordParsed
{
    bool isValid; // Are the contents valid (verified to be sane)?
    bool isCmdIn;
    DRecordData* cmdhdr;
    DRecordData* cmdin; // schema is cmdall_schema (below)
    DRecordData* cmdout; // schema is cmdall_schema (below)
} IOLogTraceRecordParsed_t;

typedef struct IOLogStatsParsed
{
    bool isValid; // Are the contents valid (verified to be sane)?
    DRecordData* stats;
} IOLogStatsParsed_t;

struct IOLogTrace
{
private:
    IOLogDump* dump;

public:
    static string* opnames[];
    static const unsigned max_optypes;
    bool verify;
    ULONG64 cum_ticks;
    ULONG64 start_ticks;
    ULONG64 max_seqid;
    ULONG64 seqid;
    ULONG nrecs;
    ULONG nrecs_invalid;
    ULONG numios_inflight;
    IOLogDumpRecMeta_t* dcmdhdr_meta;
    IOLogDumpRecMeta_t* dcmdin_meta;
    IOLogDumpRecMeta_t* dcmdout_meta;

    /* Canonical schema of a trace record (union of cmdin and cmdout
     * schemas plus additional computed fields) */
    DRecordSchema cmdall_schema;

    IOLogTraceRecordParsed_t curRec;
    DRecordData cur_cmd, cur_cmdhdr;
    map<ULONG64, DRecordData*> inflight_ios;
    bool has_ts;
    bool has_phase;

    IOLogTrace(IOLogDump* mydump)
    {
        dump = mydump;
        verify = true;
    }

    bool start();
    IOLogTraceRecordParsed_t* next();
    void end()
    {
        inflight_ios.clear();
    }
private:
    int verify_trace_record(DRecordData* drec)
    {
        if(! verify)
            return true;

        if((*drec)["op"].i >= max_optypes)
            return false;

        if(has_phase && (*drec)["phase"].i > 1)
            return false;

        if(has_ts)
        {
            ULONG64 myts = cur_cmdhdr["ts"].i;

            if(myts < cum_ticks)
                return false;

            if((dump->end_ts > 0) &&
                    ((myts < dump->start_ts) || (myts >= dump->end_ts)))
                return false;
        }

        return true;
    }
};

struct IOLogStats
{
private:
    IOLogDump* dump;
public:
    ULONG64 prev_ts;
    ULONG64 start_ts;
    ULONG nrecs;
    ULONG nrecs_invalid;
    IOLogDumpRecMeta_t* dstats_meta;
    IOLogStatsParsed_t curRec;
    bool dump_intvl_in_ticks;
    bool verify;

    /* Schema of in-memory stats record (with new computed fields added) */
    DRecordSchema stats_schema;

    DRecordData cur_stats;

    IOLogStats(IOLogDump* mydump)
    {
        dump = mydump;
        verify = true;
    }

    ~IOLogStats()
    {
    }

    bool start();
    IOLogStatsParsed_t* next();
    inline void end() { }

    int verify_record(DRecordData* drec)
    {
        if(! verify)
            return true;

        if(((LONG64)(*drec)["nreqs"].i) < 0)
            return false;

        if(((LONG64)(*drec)["nreads"].i) < 0)
            return false;

        if(((LONG64)(*drec)["nwrites"].i) < 0)
            return false;

        return true;
    }
};
