/**
 **     File:  IOLogDumpSchema.h
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
#ifndef __IOLogDumpSchema_H__
#define __IOLogDumpSchema_H__
#define FMT_LBA "%lu"
#define FMT_ULONG64 "%lu"
#define FMT_ULONG "%u"
#include "DRecord.h"

#define IOLOG_TRACE_SIGNATURE 0x50434c57
#define IOLOG_STATS_SIGNATURE 0x50435356
#define MAX_IO_SIZE			(2 * 1024 * 1024)
#define DISK_BLOCK_SIZE		512
#define DATA_START_OFFSET	4096
#define LUN_FIELD			"lun_ssid"

typedef void (*RecParser_fn_t)(struct IOLogDump *dump, void *buf, DRecordData& drecdata);
typedef struct IOLogDumpRecMeta {
    DRecordSchema *schema;
    RecParser_fn_t to_native_fn;
    int dumpsize;
} IOLogDumpRecMeta_t;

typedef struct IOLogDumpFixedHeader {
    ULONG sig;
    USHORT version_minor;
    USHORT version_major;
} IOLogDumpFixedHeader_t;

typedef unsigned long SchemaVersion_t;
typedef string TypeName_t;
typedef map<TypeName_t, IOLogDumpRecMeta*> IOLogDumpSchemas_t;
extern map<SchemaVersion_t, IOLogDumpSchemas_t*> All_IOLogDumpSchemas;
extern void load_IOLogDumpSchemas();
struct IOLogDump;

#endif /* __IOLogDumpSchema_H__ */
