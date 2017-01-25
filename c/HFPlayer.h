/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/**
 **    File:  HFPlayer.h
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

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "IOLogDumpSchema.h"

struct IOLogDump
{
private:
    FILE* fp;
public:
    ULONG64 curoffset;
    IOLogDumpSchemas_t* schema_defs;
    DRecordData_t header;
    int is_little_endian; // Is the dump stored in little-endian format?
    int host_little_endian;
    ULONG64 dumpvol_size;
    ULONG64 dumpsize;
    ULONG dump_recsize;
    ULONG dumpver;
    ULONG nrecords;
    ULONG recnum;
    ULONG64 start_ts;
    ULONG64 end_ts;
    ULONG64 ts_rate;
    BYTE buf[DATA_START_OFFSET];

    IOLogDump()
    {
        fp = NULL;
        curoffset = 0;
        {
            int i = 1;
            host_little_endian = ((char*)&i)[0] == 1;
        }
        load_IOLogDumpSchemas();
    }

    ~IOLogDump()
    {
        fclose(fp);
    }

    inline ULONG version(USHORT major, USHORT minor)
    {
        return (((ULONG)major) << 16) + minor;
    }
    inline int open(char* dumpfile)
    {
        bzero(buf, sizeof(buf));
        fp = fopen(dumpfile, "r");

        if(fp == NULL)
        {
            fprintf(stderr, "Cannot open %s for reading: %s.\n",
                    dumpfile, strerror(errno));
            return 1;
        }

        // read header data
        if(fread((void*)&buf[0], 1, sizeof(buf), fp) < sizeof(buf))
        {
            fprintf(stderr, "unable to read trace dump header.\n");
            return 1;
        }

        IOLogDumpFixedHeader_t fixed_hdr;
        memcpy((void*)&fixed_hdr, buf, sizeof(fixed_hdr));
        is_little_endian = (((char*)&fixed_hdr.version_minor)[0] != 0 ||
                            ((char*)&fixed_hdr.version_major)[0] != 0);
        fixed_hdr.sig = to_native(fixed_hdr.sig);
        fixed_hdr.version_major = to_native(fixed_hdr.version_major);
        fixed_hdr.version_minor = to_native(fixed_hdr.version_minor);
        dumpver = version(fixed_hdr.version_major, fixed_hdr.version_minor);

        if(All_IOLogDumpSchemas.count(dumpver) == 0)
        {
            fprintf(stderr, "Unknown dump version 0x%08x.\n", dumpver);
            return 1;
        }

        fprintf(stderr, "dump version %u_%u\n",
                fixed_hdr.version_major, fixed_hdr.version_minor);
        schema_defs = All_IOLogDumpSchemas[dumpver];

        if(schema_defs->count("IOLogDumpHdr") == 0)
        {
            fprintf(stderr, "Missing schema definition for IOLogDumpHdr.\n", dumpver);
            return 1;
        }

        (*schema_defs)["IOLogDumpHdr"]->to_native_fn(this, buf, header);
        ts_rate = header.has("ts_rate") ? header["ts_rate"].i : 1;
        return 0;
    }

    inline bool start()
    {
        ULONG64 start_byteoffset = 0;
        dumpvol_size = header["dumpvol_size"].i * DISK_BLOCK_SIZE;
        dumpsize = 0;

        if(header.has("start_byteoffset"))
        {
            start_byteoffset = header["start_byteoffset"].i;

            if(header["end_byteoffset"].i > header["start_byteoffset"].i)
                dumpsize = (header["end_byteoffset"].i - header["start_byteoffset"].i + 1);
            else
                if(header["end_byteoffset"].i < header["start_byteoffset"].i)
                    dumpsize = dumpvol_size - DATA_START_OFFSET;
        }
        else
        {
            start_byteoffset = header["start_lba"].i * DISK_BLOCK_SIZE;

            if(header["end_lba"].i > header["start_lba"].i)
                dumpsize = (header["end_lba"].i - header["start_lba"].i + 1) * DISK_BLOCK_SIZE;
            else
                if(header["end_lba"].i < header["start_lba"].i)
                    dumpsize = dumpvol_size - DATA_START_OFFSET;
        }

        dump_recsize = header["rec_size"].i;
        nrecords = dumpsize / dump_recsize;
        fprintf(stderr, "dump vol size " FMT_ULONG64 " MB, data size " FMT_ULONG64 " bytes, #records %u, \n",
                dumpvol_size / ((ULONG64) 1024 * 1024), dumpsize, nrecords);

        if(fseek(fp, start_byteoffset, SEEK_SET) < 0)
        {
            fprintf(stderr, "Can't seek to start record: %s\n", strerror(errno));
            return false;
        }

        start_ts = header.has("start_ts") ? header["start_ts"].i : 0;
        end_ts = header.has("end_ts") ? header["end_ts"].i : 0;
        curoffset = ftell(fp);
        recnum = 0;
        return true;
    }
    inline void* next()
    {
        if(recnum >= nrecords)
            return NULL;

        if(curoffset + dump_recsize > dumpvol_size)
        {
            curoffset = header.has("start_byteoffset")
                        ? header["start_byteoffset"].i
                        : header["start_lba"].i * DISK_BLOCK_SIZE;
        }

        if(fseek(fp, curoffset, SEEK_SET) < 0)
        {
            fprintf(stderr, "Can't seek to record: %s\n", strerror(errno));
            return NULL;
        }

        if(fread(buf, 1, dump_recsize, fp) < dump_recsize)
            return NULL;

        curoffset += dump_recsize;
        ++ recnum;
        return buf;
    }

    inline void* next(void* rec, int recsize)
    {
        if(recnum >= nrecords)
            return NULL;

        if(curoffset + dump_recsize > dumpvol_size)
        {
            curoffset = header.has("start_byteoffset")
                        ? header["start_byteoffset"].i
                        : header["start_lba"].i * DISK_BLOCK_SIZE;
        }

        if(fseek(fp, curoffset, SEEK_SET) < 0)
        {
            fprintf(stderr, "Can't seek to record: %s\n", strerror(errno));
            return NULL;
        }

        if(fread(rec, 1, recsize, fp) < (unsigned) recsize)
            return NULL;

        curoffset += dump_recsize;
        ++ recnum;
        return rec;
    }

    inline ULONG64 to_native(ULONG64 val)
    {
        ULONG64 newval;

        if(is_little_endian == host_little_endian)
            return val;
        else
            if(is_little_endian)
            {
                ULONG* inv = (ULONG*)&val;
                ULONG* outv = (ULONG*)&newval;
                outv[0] = htonl(inv[1]);
                outv[1] = htonl(inv[0]);
            }
            else
            {
                ULONG* inv = (ULONG*)&val;
                ULONG* outv = (ULONG*)&newval;
                outv[0] = ntohl(inv[1]);
                outv[1] = ntohl(inv[0]);
            }

        return newval;
    }

    inline ULONG to_native(ULONG val)
    {
        if(is_little_endian == host_little_endian)
            return val;
        else
            if(is_little_endian)
                return htonl(val);
            else
                return ntohl(val);
    }

    inline USHORT to_native(USHORT val)
    {
        if(is_little_endian == host_little_endian)
            return val;
        else
            if(is_little_endian)
                return htons(val);
            else
                return ntohs(val);
    }

    template <class T> ULONG64 to_native_field(char *&buf)
    {
        T field;
        memcpy(&field, buf, sizeof(field));
        buf += sizeof(field);
        return (ULONG64) to_native(field);
    }

    inline double ticks2us(ULONG64 ticks)
    {
        return ticks * 1000000.0 / ts_rate;
    }

    inline double ticks2sec(ULONG64 ticks)
    {
        return ticks / (double) ts_rate;
    }
};
