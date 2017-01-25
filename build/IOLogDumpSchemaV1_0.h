/**
 **     File:  IOLogDumpSchemaV1_0.h
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
#include "IOLogTypes.h"

#define FMT_LBA "%lu"
#define FMT_ULONG64 "%lu"
#define FMT_ULONG "%u"
typedef struct IOLogDumpHeader {
    ULONG sig;
    USHORT version_minor;
    USHORT version_major;
    ULONG64 start_byteoffset;
    ULONG64 end_byteoffset;
    ULONG64 start_time;
    ULONG64 end_time;
    ULONG64 start_ticks;
    ULONG64 end_ticks;
    ULONG64 ts_rate;
    ULONG64 dumpvol_size;
    ULONG dumpvol_ssid;
    ULONG rec_size;
    ULONG hdr_size;
    ULONG hdr_savethres;
    ULONG ctlr_id;
    BYTE serialno[16];
    ULONG nvolumes;
} IOLogDumpHeader_t;
#define HAVE_IOLogDumpHeader_SIG 1
#define HAVE_IOLogDumpHeader_VERSION_MINOR 1
#define HAVE_IOLogDumpHeader_VERSION_MAJOR 1
#define HAVE_IOLogDumpHeader_START_BYTEOFFSET 1
#define HAVE_IOLogDumpHeader_END_BYTEOFFSET 1
#define HAVE_IOLogDumpHeader_START_TIME 1
#define HAVE_IOLogDumpHeader_END_TIME 1
#define HAVE_IOLogDumpHeader_START_TICKS 1
#define HAVE_IOLogDumpHeader_END_TICKS 1
#define HAVE_IOLogDumpHeader_TS_RATE 1
#define HAVE_IOLogDumpHeader_DUMPVOL_SIZE 1
#define HAVE_IOLogDumpHeader_DUMPVOL_SSID 1
#define HAVE_IOLogDumpHeader_REC_SIZE 1
#define HAVE_IOLogDumpHeader_HDR_SIZE 1
#define HAVE_IOLogDumpHeader_HDR_SAVETHRES 1
#define HAVE_IOLogDumpHeader_CTLR_ID 1
#define HAVE_IOLogDumpHeader_SERIALNO 1
#define HAVE_IOLogDumpHeader_NVOLUMES 1

typedef struct IOLogTraceCmdInRecord {
    ULONG64 ts;
    ULONG seqid;
    USHORT lun;
    BYTE op;
    BYTE phase;
    ULONG64 lba;
    ULONG nblks;
} IOLogTraceCmdInRecord_t;
#define HAVE_IOLogTraceCmdInRecord_TS 1
#define HAVE_IOLogTraceCmdInRecord_SEQID 1
#define HAVE_IOLogTraceCmdInRecord_LUN 1
#define HAVE_IOLogTraceCmdInRecord_OP 1
#define HAVE_IOLogTraceCmdInRecord_PHASE 1
#define HAVE_IOLogTraceCmdInRecord_LBA 1
#define HAVE_IOLogTraceCmdInRecord_NBLKS 1

typedef struct IOLogTraceCmdOutRecord {
    ULONG64 ts;
    ULONG seqid;
    USHORT lun;
    BYTE op;
    BYTE phase;
    ULONG latency_ticks;
    USHORT host_id;
    USHORT host_lun;
} IOLogTraceCmdOutRecord_t;
#define HAVE_IOLogTraceCmdOutRecord_TS 1
#define HAVE_IOLogTraceCmdOutRecord_SEQID 1
#define HAVE_IOLogTraceCmdOutRecord_LUN 1
#define HAVE_IOLogTraceCmdOutRecord_OP 1
#define HAVE_IOLogTraceCmdOutRecord_PHASE 1
#define HAVE_IOLogTraceCmdOutRecord_LATENCY_TICKS 1
#define HAVE_IOLogTraceCmdOutRecord_HOST_ID 1
#define HAVE_IOLogTraceCmdOutRecord_HOST_LUN 1

typedef struct IOLogTraceRecord {
	union {
		struct IOLogTraceCmdInRecord cmdin;
		struct IOLogTraceCmdOutRecord cmdout;
	} u;
} IOLogTraceRecord_t;

typedef struct IOLogStatsRecord {
    ULONG64 ts;
    ULONG dump_intvl;
    USHORT obj_type;
    USHORT obj_id;
    ULONG64 nreqs;
    ULONG64 nreads;
    ULONG64 nread_blks;
    ULONG64 cum_read_ticks;
    ULONG64 cache_hits;
    ULONG64 cum_cache_read_ticks;
    ULONG64 nwrites;
    ULONG64 nwrite_blks;
    ULONG64 cum_write_ticks;
    ULONG64 read_io_ticks;
    ULONG64 io_wait_ticks;
    ULONG64 prefetch_requests;
    ULONG64 prefetch_hits;
    ULONG64 prefetch_miss_exp_hit;
    ULONG64 prefetch_hit_exp_miss;
    ULONG64 prefetch_performed;
    ULONG64 prefetch_blks;
    ULONG64 idle_time;
    ULONG64 evict_prefetchhit;
    ULONG64 evict_prefetch;
    ULONG64 evict_hit2;
    ULONG64 evict_fetch;
    ULONG64 evict_write;
    ULONG64 evict_writehit;
    ULONG64 evict_prefetch_update;
    ULONG64 evict_prefetch_updatehit;
    ULONG64 evict_total;
    ULONG64 cacheblks_inuse;
} IOLogStatsRecord_t;
#define HAVE_IOLogStatsRecord_TS 1
#define HAVE_IOLogStatsRecord_DUMP_INTVL 1
#define HAVE_IOLogStatsRecord_OBJ_TYPE 1
#define HAVE_IOLogStatsRecord_OBJ_ID 1
#define HAVE_IOLogStatsRecord_NREQS 1
#define HAVE_IOLogStatsRecord_NREADS 1
#define HAVE_IOLogStatsRecord_NREAD_BLKS 1
#define HAVE_IOLogStatsRecord_CUM_READ_TICKS 1
#define HAVE_IOLogStatsRecord_CACHE_HITS 1
#define HAVE_IOLogStatsRecord_CUM_CACHE_READ_TICKS 1
#define HAVE_IOLogStatsRecord_NWRITES 1
#define HAVE_IOLogStatsRecord_NWRITE_BLKS 1
#define HAVE_IOLogStatsRecord_CUM_WRITE_TICKS 1
#define HAVE_IOLogStatsRecord_READ_IO_TICKS 1
#define HAVE_IOLogStatsRecord_IO_WAIT_TICKS 1
#define HAVE_IOLogStatsRecord_PREFETCH_REQUESTS 1
#define HAVE_IOLogStatsRecord_PREFETCH_HITS 1
#define HAVE_IOLogStatsRecord_PREFETCH_MISS_EXP_HIT 1
#define HAVE_IOLogStatsRecord_PREFETCH_HIT_EXP_MISS 1
#define HAVE_IOLogStatsRecord_PREFETCH_PERFORMED 1
#define HAVE_IOLogStatsRecord_PREFETCH_BLKS 1
#define HAVE_IOLogStatsRecord_IDLE_TIME 1
#define HAVE_IOLogStatsRecord_EVICT_PREFETCHHIT 1
#define HAVE_IOLogStatsRecord_EVICT_PREFETCH 1
#define HAVE_IOLogStatsRecord_EVICT_HIT2 1
#define HAVE_IOLogStatsRecord_EVICT_FETCH 1
#define HAVE_IOLogStatsRecord_EVICT_WRITE 1
#define HAVE_IOLogStatsRecord_EVICT_WRITEHIT 1
#define HAVE_IOLogStatsRecord_EVICT_PREFETCH_UPDATE 1
#define HAVE_IOLogStatsRecord_EVICT_PREFETCH_UPDATEHIT 1
#define HAVE_IOLogStatsRecord_EVICT_TOTAL 1
#define HAVE_IOLogStatsRecord_CACHEBLKS_INUSE 1

#define IOLOG_TRACE_SIGNATURE 0x50434c57
#define IOLOG_STATS_SIGNATURE 0x50435356
#define IOLOG_VERSION 0x00010000
#define DISK_BLOCK_SIZE		512
#define DATA_START_OFFSET	4096
#define IOLOG_TRACE_NOPS   2  // must match opnames_array_size / 2.

struct IOLogDump;
extern int parseTrace(char *dumpfile);
extern int parseStats(char *dumpfile);

extern const char *opnames[];

extern void to_native_IOLogDumpHeader(struct IOLogDump *dump, struct IOLogDumpHeader *rec);
extern void to_native_IOLogTraceCmdInRecord(struct IOLogDump *dump, struct IOLogTraceCmdInRecord *rec);
extern void to_native_IOLogTraceCmdOutRecord(struct IOLogDump *dump, struct IOLogTraceCmdOutRecord *rec);
extern void to_native_IOLogStatsRecord(struct IOLogDump *dump, struct IOLogStatsRecord *rec);

#define FMTS_IOLogDumpHeader \
    "%u,%hu,%hu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%u,%u,%u,%u,%u,%16c,%u"
#define VALS_IOLogDumpHeader(_rec) \
    (_rec)->sig, (_rec)->version_minor, (_rec)->version_major, (_rec)->start_byteoffset, (_rec)->end_byteoffset, (_rec)->start_time, (_rec)->end_time, (_rec)->start_ticks, (_rec)->end_ticks, (_rec)->ts_rate, (_rec)->dumpvol_size, (_rec)->dumpvol_ssid, (_rec)->rec_size, (_rec)->hdr_size, (_rec)->hdr_savethres, (_rec)->ctlr_id, (_rec)->serialno, (_rec)->nvolumes

#define FMTS_IOLogTraceCmdInRecord \
    "%lu,%u,%hu,%s,%c,%lu,%u"
#define VALS_IOLogTraceCmdInRecord(_rec) \
    (_rec)->ts, (_rec)->seqid, (_rec)->lun, opnames[(_rec)->op], (_rec)->phase, (_rec)->lba, (_rec)->nblks

#define FMTS_IOLogTraceCmdOutRecord \
    "%lu,%u,%hu,%s,%c,%u,%hu,%hu"
#define VALS_IOLogTraceCmdOutRecord(_rec) \
    (_rec)->ts, (_rec)->seqid, (_rec)->lun, opnames[(_rec)->op], (_rec)->phase, (_rec)->latency_ticks, (_rec)->host_id, (_rec)->host_lun

#define FMTS_IOLogStatsRecord \
    "%lu,%u,%hu,%hu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu"
#define VALS_IOLogStatsRecord(_rec) \
    (_rec)->ts, (_rec)->dump_intvl, (_rec)->obj_type, (_rec)->obj_id, (_rec)->nreqs, (_rec)->nreads, (_rec)->nread_blks, (_rec)->cum_read_ticks, (_rec)->cache_hits, (_rec)->cum_cache_read_ticks, (_rec)->nwrites, (_rec)->nwrite_blks, (_rec)->cum_write_ticks, (_rec)->read_io_ticks, (_rec)->io_wait_ticks, (_rec)->prefetch_requests, (_rec)->prefetch_hits, (_rec)->prefetch_miss_exp_hit, (_rec)->prefetch_hit_exp_miss, (_rec)->prefetch_performed, (_rec)->prefetch_blks, (_rec)->idle_time, (_rec)->evict_prefetchhit, (_rec)->evict_prefetch, (_rec)->evict_hit2, (_rec)->evict_fetch, (_rec)->evict_write, (_rec)->evict_writehit, (_rec)->evict_prefetch_update, (_rec)->evict_prefetch_updatehit, (_rec)->evict_total, (_rec)->cacheblks_inuse


#define HDRSTRING_IOTraceCmdInRecord \
    "TS,SEQID,LUN,OP,PHASE,LBA,NBLKS"
#define HDRSTRING_IOTraceCmdOutRecord \
    "TS,SEQID,LUN,OP,PHASE,LATENCY_TICKS,HOST_ID,HOST_LUN"
#define HDRSTRING_IOStatsRecord \
    "TS,DUMP_INTVL,OBJ_TYPE,OBJ_ID,NREQS,NREADS,NREAD_BLKS,CUM_READ_TICKS,CACHE_HITS,CUM_CACHE_READ_TICKS,NWRITES,NWRITE_BLKS,CUM_WRITE_TICKS,READ_IO_TICKS,IO_WAIT_TICKS,PREFETCH_REQUESTS,PREFETCH_HITS,PREFETCH_MISS_EXP_HIT,PREFETCH_HIT_EXP_MISS,PREFETCH_PERFORMED,PREFETCH_BLKS,IDLE_TIME,EVICT_PREFETCHHIT,EVICT_PREFETCH,EVICT_HIT2,EVICT_FETCH,EVICT_WRITE,EVICT_WRITEHIT,EVICT_PREFETCH_UPDATE,EVICT_PREFETCH_UPDATEHIT,EVICT_TOTAL,CACHEBLKS_INUSE"
