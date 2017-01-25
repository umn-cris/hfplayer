/**
 **     File:  libIOLogDumpSchema.cc
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
#include "HFPlayer.h"

map<SchemaVersion_t, IOLogDumpSchemas_t*> All_IOLogDumpSchemas;

static IOLogDumpRecMeta_t *meta_IOLogTraceCmdOut_v0_2 = 0;
void to_native_IOLogTraceCmdOut_v0_2(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdOut_v0_2->schema);
	int i = (drec.schema == meta_IOLogTraceCmdOut_v0_2->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdOut_v0_2->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* latency_ticks */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* host_id */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* host_lun */

}


static IOLogDumpRecMeta_t *meta_IOLogTraceCmdIn_v0_2 = 0;
void to_native_IOLogTraceCmdIn_v0_2(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdIn_v0_2->schema);
	int i = (drec.schema == meta_IOLogTraceCmdIn_v0_2->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdIn_v0_2->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* nblks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* lba */

}


static IOLogDumpRecMeta_t *meta_IOLogDumpHdr_v0_2 = 0;
void to_native_IOLogDumpHdr_v0_2(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogDumpHdr_v0_2->schema);
	int i = (drec.schema == meta_IOLogDumpHdr_v0_2->schema) ? 0 :
				drec.schema->get(meta_IOLogDumpHdr_v0_2->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* sig */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* version_minor */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* version_major */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* start_lba */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* end_lba */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* base_ts */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_rate */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* dumpvol_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* dumpvol_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* hdr_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* hdr_savethres */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* ctlr_id */

}


static IOLogDumpRecMeta_t *meta_IOLogTraceCmdHdr_v0_2 = 0;
void to_native_IOLogTraceCmdHdr_v0_2(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdHdr_v0_2->schema);
	int i = (drec.schema == meta_IOLogTraceCmdHdr_v0_2->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdHdr_v0_2->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */

}


static IOLogDumpRecMeta_t *meta_IOLogStats_v0_2 = 0;
void to_native_IOLogStats_v0_2(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogStats_v0_2->schema);
	int i = (drec.schema == meta_IOLogStats_v0_2->schema) ? 0 :
				drec.schema->get(meta_IOLogStats_v0_2->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_type */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* intvl_msecs */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nreqs */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nreads */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nread_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_read_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cache_hits */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_cache_read_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nwrites */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nwrite_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_write_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* read_io_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* io_wait_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* pad1 */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* pad2 */

}

static IOLogDumpSchemas_t* 
load_IOLogDumpSchema_v0_2() 
{
	IOLogDumpSchemas_t *schemas_v0_2 = new IOLogDumpSchemas_t();
    meta_IOLogTraceCmdOut_v0_2 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdOut_v0_2->schema = new DRecordSchema("IOLogTraceCmdOut");
    meta_IOLogTraceCmdOut_v0_2->to_native_fn = to_native_IOLogTraceCmdOut_v0_2;
	meta_IOLogTraceCmdOut_v0_2->dumpsize = 24;
    meta_IOLogTraceCmdOut_v0_2->schema->add("ts_incr");
    meta_IOLogTraceCmdOut_v0_2->schema->add("seqid");
    meta_IOLogTraceCmdOut_v0_2->schema->add("op");
    meta_IOLogTraceCmdOut_v0_2->schema->add("lun_ssid");
    meta_IOLogTraceCmdOut_v0_2->schema->add("latency_ticks");
    meta_IOLogTraceCmdOut_v0_2->schema->add("host_id");
    meta_IOLogTraceCmdOut_v0_2->schema->add("host_lun");
    (*schemas_v0_2)["IOLogTraceCmdOut"] = meta_IOLogTraceCmdOut_v0_2;

    meta_IOLogTraceCmdIn_v0_2 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdIn_v0_2->schema = new DRecordSchema("IOLogTraceCmdIn");
    meta_IOLogTraceCmdIn_v0_2->to_native_fn = to_native_IOLogTraceCmdIn_v0_2;
	meta_IOLogTraceCmdIn_v0_2->dumpsize = 28;
    meta_IOLogTraceCmdIn_v0_2->schema->add("ts_incr");
    meta_IOLogTraceCmdIn_v0_2->schema->add("seqid");
    meta_IOLogTraceCmdIn_v0_2->schema->add("op");
    meta_IOLogTraceCmdIn_v0_2->schema->add("lun_ssid");
    meta_IOLogTraceCmdIn_v0_2->schema->add("nblks");
    meta_IOLogTraceCmdIn_v0_2->schema->add("lba");
    (*schemas_v0_2)["IOLogTraceCmdIn"] = meta_IOLogTraceCmdIn_v0_2;

    meta_IOLogDumpHdr_v0_2 = new IOLogDumpRecMeta();
	meta_IOLogDumpHdr_v0_2->schema = new DRecordSchema("IOLogDumpHdr");
    meta_IOLogDumpHdr_v0_2->to_native_fn = to_native_IOLogDumpHdr_v0_2;
	meta_IOLogDumpHdr_v0_2->dumpsize = 68;
    meta_IOLogDumpHdr_v0_2->schema->add("sig");
    meta_IOLogDumpHdr_v0_2->schema->add("version_minor");
    meta_IOLogDumpHdr_v0_2->schema->add("version_major");
    meta_IOLogDumpHdr_v0_2->schema->add("start_lba");
    meta_IOLogDumpHdr_v0_2->schema->add("end_lba");
    meta_IOLogDumpHdr_v0_2->schema->add("base_ts");
    meta_IOLogDumpHdr_v0_2->schema->add("ts_rate");
    meta_IOLogDumpHdr_v0_2->schema->add("dumpvol_size");
    meta_IOLogDumpHdr_v0_2->schema->add("dumpvol_ssid");
    meta_IOLogDumpHdr_v0_2->schema->add("rec_size");
    meta_IOLogDumpHdr_v0_2->schema->add("hdr_size");
    meta_IOLogDumpHdr_v0_2->schema->add("hdr_savethres");
    meta_IOLogDumpHdr_v0_2->schema->add("ctlr_id");
    (*schemas_v0_2)["IOLogDumpHdr"] = meta_IOLogDumpHdr_v0_2;

    meta_IOLogTraceCmdHdr_v0_2 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdHdr_v0_2->schema = new DRecordSchema("IOLogTraceCmdHdr");
    meta_IOLogTraceCmdHdr_v0_2->to_native_fn = to_native_IOLogTraceCmdHdr_v0_2;
	meta_IOLogTraceCmdHdr_v0_2->dumpsize = 16;
    meta_IOLogTraceCmdHdr_v0_2->schema->add("ts_incr");
    meta_IOLogTraceCmdHdr_v0_2->schema->add("seqid");
    meta_IOLogTraceCmdHdr_v0_2->schema->add("op");
    meta_IOLogTraceCmdHdr_v0_2->schema->add("lun_ssid");
    (*schemas_v0_2)["IOLogTraceCmdHdr"] = meta_IOLogTraceCmdHdr_v0_2;

    meta_IOLogStats_v0_2 = new IOLogDumpRecMeta();
	meta_IOLogStats_v0_2->schema = new DRecordSchema("IOLogStats");
    meta_IOLogStats_v0_2->to_native_fn = to_native_IOLogStats_v0_2;
	meta_IOLogStats_v0_2->dumpsize = 128;
    meta_IOLogStats_v0_2->schema->add("lun_ssid");
    meta_IOLogStats_v0_2->schema->add("rec_size");
    meta_IOLogStats_v0_2->schema->add("rec_type");
    meta_IOLogStats_v0_2->schema->add("intvl_msecs");
    meta_IOLogStats_v0_2->schema->add("ts");
    meta_IOLogStats_v0_2->schema->add("nreqs");
    meta_IOLogStats_v0_2->schema->add("nreads");
    meta_IOLogStats_v0_2->schema->add("nread_blks");
    meta_IOLogStats_v0_2->schema->add("cum_read_ticks");
    meta_IOLogStats_v0_2->schema->add("cache_hits");
    meta_IOLogStats_v0_2->schema->add("cum_cache_read_ticks");
    meta_IOLogStats_v0_2->schema->add("nwrites");
    meta_IOLogStats_v0_2->schema->add("nwrite_blks");
    meta_IOLogStats_v0_2->schema->add("cum_write_ticks");
    meta_IOLogStats_v0_2->schema->add("read_io_ticks");
    meta_IOLogStats_v0_2->schema->add("io_wait_ticks");
    meta_IOLogStats_v0_2->schema->add("pad1");
    meta_IOLogStats_v0_2->schema->add("pad2");
    (*schemas_v0_2)["IOLogStats"] = meta_IOLogStats_v0_2;

	return schemas_v0_2;
}

static IOLogDumpRecMeta_t *meta_IOLogTraceCmdOut_v0_3 = 0;
void to_native_IOLogTraceCmdOut_v0_3(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdOut_v0_3->schema);
	int i = (drec.schema == meta_IOLogTraceCmdOut_v0_3->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdOut_v0_3->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* latency_ticks */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* host_id */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* host_lun */

}


static IOLogDumpRecMeta_t *meta_IOLogTraceCmdIn_v0_3 = 0;
void to_native_IOLogTraceCmdIn_v0_3(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdIn_v0_3->schema);
	int i = (drec.schema == meta_IOLogTraceCmdIn_v0_3->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdIn_v0_3->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* nblks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* lba */

}


static IOLogDumpRecMeta_t *meta_IOLogDumpHdr_v0_3 = 0;
void to_native_IOLogDumpHdr_v0_3(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogDumpHdr_v0_3->schema);
	int i = (drec.schema == meta_IOLogDumpHdr_v0_3->schema) ? 0 :
				drec.schema->get(meta_IOLogDumpHdr_v0_3->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* sig */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* version_minor */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* version_major */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* start_lba */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* end_lba */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* base_ts */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_rate */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* dumpvol_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* dumpvol_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* hdr_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* hdr_savethres */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* ctlr_id */

}


static IOLogDumpRecMeta_t *meta_IOLogTraceCmdHdr_v0_3 = 0;
void to_native_IOLogTraceCmdHdr_v0_3(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdHdr_v0_3->schema);
	int i = (drec.schema == meta_IOLogTraceCmdHdr_v0_3->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdHdr_v0_3->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */

}


static IOLogDumpRecMeta_t *meta_IOLogStats_v0_3 = 0;
void to_native_IOLogStats_v0_3(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogStats_v0_3->schema);
	int i = (drec.schema == meta_IOLogStats_v0_3->schema) ? 0 :
				drec.schema->get(meta_IOLogStats_v0_3->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_type */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* intvl_msecs */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nreqs */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nreads */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nread_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_read_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cache_hits */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_cache_read_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nwrites */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nwrite_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_write_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* read_io_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* io_wait_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_requests */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_hits */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_miss_exp_hit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_hit_exp_miss */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_performed */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* idle_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* pad1 */

}

static IOLogDumpSchemas_t* 
load_IOLogDumpSchema_v0_3() 
{
	IOLogDumpSchemas_t *schemas_v0_3 = new IOLogDumpSchemas_t();
    meta_IOLogTraceCmdOut_v0_3 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdOut_v0_3->schema = new DRecordSchema("IOLogTraceCmdOut");
    meta_IOLogTraceCmdOut_v0_3->to_native_fn = to_native_IOLogTraceCmdOut_v0_3;
	meta_IOLogTraceCmdOut_v0_3->dumpsize = 24;
    meta_IOLogTraceCmdOut_v0_3->schema->add("ts_incr");
    meta_IOLogTraceCmdOut_v0_3->schema->add("seqid");
    meta_IOLogTraceCmdOut_v0_3->schema->add("op");
    meta_IOLogTraceCmdOut_v0_3->schema->add("lun_ssid");
    meta_IOLogTraceCmdOut_v0_3->schema->add("latency_ticks");
    meta_IOLogTraceCmdOut_v0_3->schema->add("host_id");
    meta_IOLogTraceCmdOut_v0_3->schema->add("host_lun");
    (*schemas_v0_3)["IOLogTraceCmdOut"] = meta_IOLogTraceCmdOut_v0_3;

    meta_IOLogTraceCmdIn_v0_3 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdIn_v0_3->schema = new DRecordSchema("IOLogTraceCmdIn");
    meta_IOLogTraceCmdIn_v0_3->to_native_fn = to_native_IOLogTraceCmdIn_v0_3;
	meta_IOLogTraceCmdIn_v0_3->dumpsize = 28;
    meta_IOLogTraceCmdIn_v0_3->schema->add("ts_incr");
    meta_IOLogTraceCmdIn_v0_3->schema->add("seqid");
    meta_IOLogTraceCmdIn_v0_3->schema->add("op");
    meta_IOLogTraceCmdIn_v0_3->schema->add("lun_ssid");
    meta_IOLogTraceCmdIn_v0_3->schema->add("nblks");
    meta_IOLogTraceCmdIn_v0_3->schema->add("lba");
    (*schemas_v0_3)["IOLogTraceCmdIn"] = meta_IOLogTraceCmdIn_v0_3;

    meta_IOLogDumpHdr_v0_3 = new IOLogDumpRecMeta();
	meta_IOLogDumpHdr_v0_3->schema = new DRecordSchema("IOLogDumpHdr");
    meta_IOLogDumpHdr_v0_3->to_native_fn = to_native_IOLogDumpHdr_v0_3;
	meta_IOLogDumpHdr_v0_3->dumpsize = 68;
    meta_IOLogDumpHdr_v0_3->schema->add("sig");
    meta_IOLogDumpHdr_v0_3->schema->add("version_minor");
    meta_IOLogDumpHdr_v0_3->schema->add("version_major");
    meta_IOLogDumpHdr_v0_3->schema->add("start_lba");
    meta_IOLogDumpHdr_v0_3->schema->add("end_lba");
    meta_IOLogDumpHdr_v0_3->schema->add("base_ts");
    meta_IOLogDumpHdr_v0_3->schema->add("ts_rate");
    meta_IOLogDumpHdr_v0_3->schema->add("dumpvol_size");
    meta_IOLogDumpHdr_v0_3->schema->add("dumpvol_ssid");
    meta_IOLogDumpHdr_v0_3->schema->add("rec_size");
    meta_IOLogDumpHdr_v0_3->schema->add("hdr_size");
    meta_IOLogDumpHdr_v0_3->schema->add("hdr_savethres");
    meta_IOLogDumpHdr_v0_3->schema->add("ctlr_id");
    (*schemas_v0_3)["IOLogDumpHdr"] = meta_IOLogDumpHdr_v0_3;

    meta_IOLogTraceCmdHdr_v0_3 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdHdr_v0_3->schema = new DRecordSchema("IOLogTraceCmdHdr");
    meta_IOLogTraceCmdHdr_v0_3->to_native_fn = to_native_IOLogTraceCmdHdr_v0_3;
	meta_IOLogTraceCmdHdr_v0_3->dumpsize = 16;
    meta_IOLogTraceCmdHdr_v0_3->schema->add("ts_incr");
    meta_IOLogTraceCmdHdr_v0_3->schema->add("seqid");
    meta_IOLogTraceCmdHdr_v0_3->schema->add("op");
    meta_IOLogTraceCmdHdr_v0_3->schema->add("lun_ssid");
    (*schemas_v0_3)["IOLogTraceCmdHdr"] = meta_IOLogTraceCmdHdr_v0_3;

    meta_IOLogStats_v0_3 = new IOLogDumpRecMeta();
	meta_IOLogStats_v0_3->schema = new DRecordSchema("IOLogStats");
    meta_IOLogStats_v0_3->to_native_fn = to_native_IOLogStats_v0_3;
	meta_IOLogStats_v0_3->dumpsize = 176;
    meta_IOLogStats_v0_3->schema->add("lun_ssid");
    meta_IOLogStats_v0_3->schema->add("rec_size");
    meta_IOLogStats_v0_3->schema->add("rec_type");
    meta_IOLogStats_v0_3->schema->add("intvl_msecs");
    meta_IOLogStats_v0_3->schema->add("ts");
    meta_IOLogStats_v0_3->schema->add("nreqs");
    meta_IOLogStats_v0_3->schema->add("nreads");
    meta_IOLogStats_v0_3->schema->add("nread_blks");
    meta_IOLogStats_v0_3->schema->add("cum_read_ticks");
    meta_IOLogStats_v0_3->schema->add("cache_hits");
    meta_IOLogStats_v0_3->schema->add("cum_cache_read_ticks");
    meta_IOLogStats_v0_3->schema->add("nwrites");
    meta_IOLogStats_v0_3->schema->add("nwrite_blks");
    meta_IOLogStats_v0_3->schema->add("cum_write_ticks");
    meta_IOLogStats_v0_3->schema->add("read_io_ticks");
    meta_IOLogStats_v0_3->schema->add("io_wait_ticks");
    meta_IOLogStats_v0_3->schema->add("prefetch_requests");
    meta_IOLogStats_v0_3->schema->add("prefetch_hits");
    meta_IOLogStats_v0_3->schema->add("prefetch_miss_exp_hit");
    meta_IOLogStats_v0_3->schema->add("prefetch_hit_exp_miss");
    meta_IOLogStats_v0_3->schema->add("prefetch_performed");
    meta_IOLogStats_v0_3->schema->add("prefetch_blks");
    meta_IOLogStats_v0_3->schema->add("idle_ticks");
    meta_IOLogStats_v0_3->schema->add("pad1");
    (*schemas_v0_3)["IOLogStats"] = meta_IOLogStats_v0_3;

	return schemas_v0_3;
}

static IOLogDumpRecMeta_t *meta_IOLogTraceCmdOut_v0_4 = 0;
void to_native_IOLogTraceCmdOut_v0_4(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdOut_v0_4->schema);
	int i = (drec.schema == meta_IOLogTraceCmdOut_v0_4->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdOut_v0_4->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* latency_ticks */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* host_id */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* host_lun */

}


static IOLogDumpRecMeta_t *meta_IOLogTraceCmdIn_v0_4 = 0;
void to_native_IOLogTraceCmdIn_v0_4(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdIn_v0_4->schema);
	int i = (drec.schema == meta_IOLogTraceCmdIn_v0_4->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdIn_v0_4->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* nblks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* lba */

}


static IOLogDumpRecMeta_t *meta_IOLogDumpHdr_v0_4 = 0;
void to_native_IOLogDumpHdr_v0_4(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogDumpHdr_v0_4->schema);
	int i = (drec.schema == meta_IOLogDumpHdr_v0_4->schema) ? 0 :
				drec.schema->get(meta_IOLogDumpHdr_v0_4->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* sig */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* version_minor */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* version_major */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* start_lba */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* end_lba */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* base_ts */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_rate */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* dumpvol_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* dumpvol_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* hdr_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* hdr_savethres */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* ctlr_id */

}


static IOLogDumpRecMeta_t *meta_IOLogTraceCmdHdr_v0_4 = 0;
void to_native_IOLogTraceCmdHdr_v0_4(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdHdr_v0_4->schema);
	int i = (drec.schema == meta_IOLogTraceCmdHdr_v0_4->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdHdr_v0_4->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_incr */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* op */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */

}


static IOLogDumpRecMeta_t *meta_IOLogStats_v0_4 = 0;
void to_native_IOLogStats_v0_4(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogStats_v0_4->schema);
	int i = (drec.schema == meta_IOLogStats_v0_4->schema) ? 0 :
				drec.schema->get(meta_IOLogStats_v0_4->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* lun_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_type */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* intvl_msecs */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nreqs */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nreads */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nread_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_read_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cache_hits */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_cache_read_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nwrites */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nwrite_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_write_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* read_io_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* io_wait_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_requests */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_hits */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_miss_exp_hit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_hit_exp_miss */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_performed */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* idle_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_prefetchhit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_prefetch */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_hit2 */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_fetch */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_write */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_writehit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_prefetch_update */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_prefetch_updatehit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_total */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cacheblks_inuse */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* pad1 */

}

static IOLogDumpSchemas_t* 
load_IOLogDumpSchema_v0_4() 
{
	IOLogDumpSchemas_t *schemas_v0_4 = new IOLogDumpSchemas_t();
    meta_IOLogTraceCmdOut_v0_4 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdOut_v0_4->schema = new DRecordSchema("IOLogTraceCmdOut");
    meta_IOLogTraceCmdOut_v0_4->to_native_fn = to_native_IOLogTraceCmdOut_v0_4;
	meta_IOLogTraceCmdOut_v0_4->dumpsize = 24;
    meta_IOLogTraceCmdOut_v0_4->schema->add("ts_incr");
    meta_IOLogTraceCmdOut_v0_4->schema->add("seqid");
    meta_IOLogTraceCmdOut_v0_4->schema->add("op");
    meta_IOLogTraceCmdOut_v0_4->schema->add("lun_ssid");
    meta_IOLogTraceCmdOut_v0_4->schema->add("latency_ticks");
    meta_IOLogTraceCmdOut_v0_4->schema->add("host_id");
    meta_IOLogTraceCmdOut_v0_4->schema->add("host_lun");
    (*schemas_v0_4)["IOLogTraceCmdOut"] = meta_IOLogTraceCmdOut_v0_4;

    meta_IOLogTraceCmdIn_v0_4 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdIn_v0_4->schema = new DRecordSchema("IOLogTraceCmdIn");
    meta_IOLogTraceCmdIn_v0_4->to_native_fn = to_native_IOLogTraceCmdIn_v0_4;
	meta_IOLogTraceCmdIn_v0_4->dumpsize = 28;
    meta_IOLogTraceCmdIn_v0_4->schema->add("ts_incr");
    meta_IOLogTraceCmdIn_v0_4->schema->add("seqid");
    meta_IOLogTraceCmdIn_v0_4->schema->add("op");
    meta_IOLogTraceCmdIn_v0_4->schema->add("lun_ssid");
    meta_IOLogTraceCmdIn_v0_4->schema->add("nblks");
    meta_IOLogTraceCmdIn_v0_4->schema->add("lba");
    (*schemas_v0_4)["IOLogTraceCmdIn"] = meta_IOLogTraceCmdIn_v0_4;

    meta_IOLogDumpHdr_v0_4 = new IOLogDumpRecMeta();
	meta_IOLogDumpHdr_v0_4->schema = new DRecordSchema("IOLogDumpHdr");
    meta_IOLogDumpHdr_v0_4->to_native_fn = to_native_IOLogDumpHdr_v0_4;
	meta_IOLogDumpHdr_v0_4->dumpsize = 68;
    meta_IOLogDumpHdr_v0_4->schema->add("sig");
    meta_IOLogDumpHdr_v0_4->schema->add("version_minor");
    meta_IOLogDumpHdr_v0_4->schema->add("version_major");
    meta_IOLogDumpHdr_v0_4->schema->add("start_lba");
    meta_IOLogDumpHdr_v0_4->schema->add("end_lba");
    meta_IOLogDumpHdr_v0_4->schema->add("base_ts");
    meta_IOLogDumpHdr_v0_4->schema->add("ts_rate");
    meta_IOLogDumpHdr_v0_4->schema->add("dumpvol_size");
    meta_IOLogDumpHdr_v0_4->schema->add("dumpvol_ssid");
    meta_IOLogDumpHdr_v0_4->schema->add("rec_size");
    meta_IOLogDumpHdr_v0_4->schema->add("hdr_size");
    meta_IOLogDumpHdr_v0_4->schema->add("hdr_savethres");
    meta_IOLogDumpHdr_v0_4->schema->add("ctlr_id");
    (*schemas_v0_4)["IOLogDumpHdr"] = meta_IOLogDumpHdr_v0_4;

    meta_IOLogTraceCmdHdr_v0_4 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdHdr_v0_4->schema = new DRecordSchema("IOLogTraceCmdHdr");
    meta_IOLogTraceCmdHdr_v0_4->to_native_fn = to_native_IOLogTraceCmdHdr_v0_4;
	meta_IOLogTraceCmdHdr_v0_4->dumpsize = 16;
    meta_IOLogTraceCmdHdr_v0_4->schema->add("ts_incr");
    meta_IOLogTraceCmdHdr_v0_4->schema->add("seqid");
    meta_IOLogTraceCmdHdr_v0_4->schema->add("op");
    meta_IOLogTraceCmdHdr_v0_4->schema->add("lun_ssid");
    (*schemas_v0_4)["IOLogTraceCmdHdr"] = meta_IOLogTraceCmdHdr_v0_4;

    meta_IOLogStats_v0_4 = new IOLogDumpRecMeta();
	meta_IOLogStats_v0_4->schema = new DRecordSchema("IOLogStats");
    meta_IOLogStats_v0_4->to_native_fn = to_native_IOLogStats_v0_4;
	meta_IOLogStats_v0_4->dumpsize = 256;
    meta_IOLogStats_v0_4->schema->add("lun_ssid");
    meta_IOLogStats_v0_4->schema->add("rec_size");
    meta_IOLogStats_v0_4->schema->add("rec_type");
    meta_IOLogStats_v0_4->schema->add("intvl_msecs");
    meta_IOLogStats_v0_4->schema->add("ts");
    meta_IOLogStats_v0_4->schema->add("nreqs");
    meta_IOLogStats_v0_4->schema->add("nreads");
    meta_IOLogStats_v0_4->schema->add("nread_blks");
    meta_IOLogStats_v0_4->schema->add("cum_read_ticks");
    meta_IOLogStats_v0_4->schema->add("cache_hits");
    meta_IOLogStats_v0_4->schema->add("cum_cache_read_ticks");
    meta_IOLogStats_v0_4->schema->add("nwrites");
    meta_IOLogStats_v0_4->schema->add("nwrite_blks");
    meta_IOLogStats_v0_4->schema->add("cum_write_ticks");
    meta_IOLogStats_v0_4->schema->add("read_io_ticks");
    meta_IOLogStats_v0_4->schema->add("io_wait_ticks");
    meta_IOLogStats_v0_4->schema->add("prefetch_requests");
    meta_IOLogStats_v0_4->schema->add("prefetch_hits");
    meta_IOLogStats_v0_4->schema->add("prefetch_miss_exp_hit");
    meta_IOLogStats_v0_4->schema->add("prefetch_hit_exp_miss");
    meta_IOLogStats_v0_4->schema->add("prefetch_performed");
    meta_IOLogStats_v0_4->schema->add("prefetch_blks");
    meta_IOLogStats_v0_4->schema->add("idle_ticks");
    meta_IOLogStats_v0_4->schema->add("evict_prefetchhit");
    meta_IOLogStats_v0_4->schema->add("evict_prefetch");
    meta_IOLogStats_v0_4->schema->add("evict_hit2");
    meta_IOLogStats_v0_4->schema->add("evict_fetch");
    meta_IOLogStats_v0_4->schema->add("evict_write");
    meta_IOLogStats_v0_4->schema->add("evict_writehit");
    meta_IOLogStats_v0_4->schema->add("evict_prefetch_update");
    meta_IOLogStats_v0_4->schema->add("evict_prefetch_updatehit");
    meta_IOLogStats_v0_4->schema->add("evict_total");
    meta_IOLogStats_v0_4->schema->add("cacheblks_inuse");
    meta_IOLogStats_v0_4->schema->add("pad1");
    (*schemas_v0_4)["IOLogStats"] = meta_IOLogStats_v0_4;

	return schemas_v0_4;
}

static IOLogDumpRecMeta_t *meta_IOLogTraceCmdOut_v1_0 = 0;
void to_native_IOLogTraceCmdOut_v1_0(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdOut_v1_0->schema);
	int i = (drec.schema == meta_IOLogTraceCmdOut_v1_0->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdOut_v1_0->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = *bufptr; ++ bufptr; /* op */
    drec[i++].i = *bufptr; ++ bufptr; /* phase */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* latency_ticks */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* host_id */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* host_lun */

}


static IOLogDumpRecMeta_t *meta_IOLogTraceCmdIn_v1_0 = 0;
void to_native_IOLogTraceCmdIn_v1_0(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdIn_v1_0->schema);
	int i = (drec.schema == meta_IOLogTraceCmdIn_v1_0->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdIn_v1_0->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = *bufptr; ++ bufptr; /* op */
    drec[i++].i = *bufptr; ++ bufptr; /* phase */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* lba */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* nblks */

}


static IOLogDumpRecMeta_t *meta_IOLogDumpHdr_v1_0 = 0;
void to_native_IOLogDumpHdr_v1_0(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogDumpHdr_v1_0->schema);
	int i = (drec.schema == meta_IOLogDumpHdr_v1_0->schema) ? 0 :
				drec.schema->get(meta_IOLogDumpHdr_v1_0->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* sig */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* version_minor */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* version_major */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* start_byteoffset */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* end_byteoffset */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* start_ts */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* end_ts */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* start_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* end_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts_rate */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* dumpvol_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* dumpvol_ssid */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* rec_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* hdr_size */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* hdr_savethres */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* ctlr_id */
    string* s = new string(bufptr, 16);
    drec[i++].s = s; bufptr += 16; /* serialno */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ndroppedrecs */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* nvolumes */

}


static IOLogDumpRecMeta_t *meta_IOLogTraceCmdHdr_v1_0 = 0;
void to_native_IOLogTraceCmdHdr_v1_0(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogTraceCmdHdr_v1_0->schema);
	int i = (drec.schema == meta_IOLogTraceCmdHdr_v1_0->schema) ? 0 :
				drec.schema->get(meta_IOLogTraceCmdHdr_v1_0->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* seqid */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* lun_ssid */
    drec[i++].i = *bufptr; ++ bufptr; /* op */
    drec[i++].i = *bufptr; ++ bufptr; /* phase */

}


static IOLogDumpRecMeta_t *meta_IOLogStats_v1_0 = 0;
void to_native_IOLogStats_v1_0(struct IOLogDump *dump, void *buf, DRecordData& drec)
{

    if (! drec.schema) 
		drec.set_schema(meta_IOLogStats_v1_0->schema);
	int i = (drec.schema == meta_IOLogStats_v1_0->schema) ? 0 :
				drec.schema->get(meta_IOLogStats_v1_0->schema->fields[0]);
	char *bufptr = (char *)buf;
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* ts */
    drec[i++].i = dump->to_native_field<ULONG>(bufptr); /* intvl_ticks */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* obj_type */
    drec[i++].i = dump->to_native_field<USHORT>(bufptr); /* obj_id */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nreqs */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nreads */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nread_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_read_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cache_hits */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_cache_read_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nwrites */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* nwrite_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cum_write_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* read_io_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* io_wait_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_requests */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_hits */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_miss_exp_hit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_hit_exp_miss */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_performed */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* prefetch_blks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* idle_ticks */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_prefetchhit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_prefetch */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_hit2 */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_fetch */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_write */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_writehit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_prefetch_update */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_prefetch_updatehit */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* evict_total */
    drec[i++].i = dump->to_native_field<ULONG64>(bufptr); /* cacheblks_inuse */

}

static IOLogDumpSchemas_t* 
load_IOLogDumpSchema_v1_0() 
{
	IOLogDumpSchemas_t *schemas_v1_0 = new IOLogDumpSchemas_t();
    meta_IOLogTraceCmdOut_v1_0 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdOut_v1_0->schema = new DRecordSchema("IOLogTraceCmdOut");
    meta_IOLogTraceCmdOut_v1_0->to_native_fn = to_native_IOLogTraceCmdOut_v1_0;
	meta_IOLogTraceCmdOut_v1_0->dumpsize = 24;
    meta_IOLogTraceCmdOut_v1_0->schema->add("ts");
    meta_IOLogTraceCmdOut_v1_0->schema->add("seqid");
    meta_IOLogTraceCmdOut_v1_0->schema->add("lun_ssid");
    meta_IOLogTraceCmdOut_v1_0->schema->add("op");
    meta_IOLogTraceCmdOut_v1_0->schema->add("phase");
    meta_IOLogTraceCmdOut_v1_0->schema->add("latency_ticks");
    meta_IOLogTraceCmdOut_v1_0->schema->add("host_id");
    meta_IOLogTraceCmdOut_v1_0->schema->add("host_lun");
    (*schemas_v1_0)["IOLogTraceCmdOut"] = meta_IOLogTraceCmdOut_v1_0;

    meta_IOLogTraceCmdIn_v1_0 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdIn_v1_0->schema = new DRecordSchema("IOLogTraceCmdIn");
    meta_IOLogTraceCmdIn_v1_0->to_native_fn = to_native_IOLogTraceCmdIn_v1_0;
	meta_IOLogTraceCmdIn_v1_0->dumpsize = 28;
    meta_IOLogTraceCmdIn_v1_0->schema->add("ts");
    meta_IOLogTraceCmdIn_v1_0->schema->add("seqid");
    meta_IOLogTraceCmdIn_v1_0->schema->add("lun_ssid");
    meta_IOLogTraceCmdIn_v1_0->schema->add("op");
    meta_IOLogTraceCmdIn_v1_0->schema->add("phase");
    meta_IOLogTraceCmdIn_v1_0->schema->add("lba");
    meta_IOLogTraceCmdIn_v1_0->schema->add("nblks");
    (*schemas_v1_0)["IOLogTraceCmdIn"] = meta_IOLogTraceCmdIn_v1_0;

    meta_IOLogDumpHdr_v1_0 = new IOLogDumpRecMeta();
	meta_IOLogDumpHdr_v1_0->schema = new DRecordSchema("IOLogDumpHdr");
    meta_IOLogDumpHdr_v1_0->to_native_fn = to_native_IOLogDumpHdr_v1_0;
	meta_IOLogDumpHdr_v1_0->dumpsize = 104;
    meta_IOLogDumpHdr_v1_0->schema->add("sig");
    meta_IOLogDumpHdr_v1_0->schema->add("version_minor");
    meta_IOLogDumpHdr_v1_0->schema->add("version_major");
    meta_IOLogDumpHdr_v1_0->schema->add("start_byteoffset");
    meta_IOLogDumpHdr_v1_0->schema->add("end_byteoffset");
    meta_IOLogDumpHdr_v1_0->schema->add("start_ts");
    meta_IOLogDumpHdr_v1_0->schema->add("end_ts");
    meta_IOLogDumpHdr_v1_0->schema->add("start_ticks");
    meta_IOLogDumpHdr_v1_0->schema->add("end_ticks");
    meta_IOLogDumpHdr_v1_0->schema->add("ts_rate");
    meta_IOLogDumpHdr_v1_0->schema->add("dumpvol_size");
    meta_IOLogDumpHdr_v1_0->schema->add("dumpvol_ssid");
    meta_IOLogDumpHdr_v1_0->schema->add("rec_size");
    meta_IOLogDumpHdr_v1_0->schema->add("hdr_size");
    meta_IOLogDumpHdr_v1_0->schema->add("hdr_savethres");
    meta_IOLogDumpHdr_v1_0->schema->add("ctlr_id");
    meta_IOLogDumpHdr_v1_0->schema->add("serialno", DRecordSchema::MyStr);
    meta_IOLogDumpHdr_v1_0->schema->add("ndroppedrecs");
    meta_IOLogDumpHdr_v1_0->schema->add("nvolumes");
    (*schemas_v1_0)["IOLogDumpHdr"] = meta_IOLogDumpHdr_v1_0;

    meta_IOLogTraceCmdHdr_v1_0 = new IOLogDumpRecMeta();
	meta_IOLogTraceCmdHdr_v1_0->schema = new DRecordSchema("IOLogTraceCmdHdr");
    meta_IOLogTraceCmdHdr_v1_0->to_native_fn = to_native_IOLogTraceCmdHdr_v1_0;
	meta_IOLogTraceCmdHdr_v1_0->dumpsize = 16;
    meta_IOLogTraceCmdHdr_v1_0->schema->add("ts");
    meta_IOLogTraceCmdHdr_v1_0->schema->add("seqid");
    meta_IOLogTraceCmdHdr_v1_0->schema->add("lun_ssid");
    meta_IOLogTraceCmdHdr_v1_0->schema->add("op");
    meta_IOLogTraceCmdHdr_v1_0->schema->add("phase");
    (*schemas_v1_0)["IOLogTraceCmdHdr"] = meta_IOLogTraceCmdHdr_v1_0;

    meta_IOLogStats_v1_0 = new IOLogDumpRecMeta();
	meta_IOLogStats_v1_0->schema = new DRecordSchema("IOLogStats");
    meta_IOLogStats_v1_0->to_native_fn = to_native_IOLogStats_v1_0;
	meta_IOLogStats_v1_0->dumpsize = 240;
    meta_IOLogStats_v1_0->schema->add("ts");
    meta_IOLogStats_v1_0->schema->add("intvl_ticks");
    meta_IOLogStats_v1_0->schema->add("obj_type");
    meta_IOLogStats_v1_0->schema->add("obj_id");
    meta_IOLogStats_v1_0->schema->add("nreqs");
    meta_IOLogStats_v1_0->schema->add("nreads");
    meta_IOLogStats_v1_0->schema->add("nread_blks");
    meta_IOLogStats_v1_0->schema->add("cum_read_ticks");
    meta_IOLogStats_v1_0->schema->add("cache_hits");
    meta_IOLogStats_v1_0->schema->add("cum_cache_read_ticks");
    meta_IOLogStats_v1_0->schema->add("nwrites");
    meta_IOLogStats_v1_0->schema->add("nwrite_blks");
    meta_IOLogStats_v1_0->schema->add("cum_write_ticks");
    meta_IOLogStats_v1_0->schema->add("read_io_ticks");
    meta_IOLogStats_v1_0->schema->add("io_wait_ticks");
    meta_IOLogStats_v1_0->schema->add("prefetch_requests");
    meta_IOLogStats_v1_0->schema->add("prefetch_hits");
    meta_IOLogStats_v1_0->schema->add("prefetch_miss_exp_hit");
    meta_IOLogStats_v1_0->schema->add("prefetch_hit_exp_miss");
    meta_IOLogStats_v1_0->schema->add("prefetch_performed");
    meta_IOLogStats_v1_0->schema->add("prefetch_blks");
    meta_IOLogStats_v1_0->schema->add("idle_ticks");
    meta_IOLogStats_v1_0->schema->add("evict_prefetchhit");
    meta_IOLogStats_v1_0->schema->add("evict_prefetch");
    meta_IOLogStats_v1_0->schema->add("evict_hit2");
    meta_IOLogStats_v1_0->schema->add("evict_fetch");
    meta_IOLogStats_v1_0->schema->add("evict_write");
    meta_IOLogStats_v1_0->schema->add("evict_writehit");
    meta_IOLogStats_v1_0->schema->add("evict_prefetch_update");
    meta_IOLogStats_v1_0->schema->add("evict_prefetch_updatehit");
    meta_IOLogStats_v1_0->schema->add("evict_total");
    meta_IOLogStats_v1_0->schema->add("cacheblks_inuse");
    (*schemas_v1_0)["IOLogStats"] = meta_IOLogStats_v1_0;

	return schemas_v1_0;
}

void load_IOLogDumpSchemas() {
	All_IOLogDumpSchemas[0x00000002] = load_IOLogDumpSchema_v0_2();
	All_IOLogDumpSchemas[0x00000003] = load_IOLogDumpSchema_v0_3();
	All_IOLogDumpSchemas[0x00000004] = load_IOLogDumpSchema_v0_4();
	All_IOLogDumpSchemas[0x00010000] = load_IOLogDumpSchema_v1_0();
}
