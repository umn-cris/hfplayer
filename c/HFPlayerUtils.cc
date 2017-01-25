/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/**
 **    File:  HFPlayerUtils.cc
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

#include <sys/types.h>
#include <fcntl.h>
#include "HFPlayerUtils.h"

bool  rotate_outOfRange = false ;

int debug = 0;
void split(vector<string> &result, string str, string delim)
{
    result.clear();
    size_t prevpos = 0, pos;

    while(prevpos != string::npos)
    {
        pos = str.find(delim, prevpos);
        result.push_back(str.substr(prevpos, pos - prevpos));

        if(pos == string::npos)
            break;

        prevpos = pos + delim.size();
    }
}

void mystrtoupper(string& str)
{
    for(size_t j = 0; j < str.size(); ++j)
        str[j] = toupper(str[j]);
}

void mystrtolower(string& str)
{
    for(size_t j = 0; j < str.size(); ++j)
        str[j] = tolower(str[j]);
}

map<char, ULONG64>& DRecordData::set_multfactor()
{
    map<char, ULONG64>* mf = new map<char, ULONG64>();
    (*mf)['k'] = 1024;
    (*mf)['m'] = 1024 * 1024;
    (*mf)['g'] = 1024 * 1024 * 1024;
    (*mf)['s'] = 1000000;
    (*mf)['h'] = 60 * 60 * (ULONG64) 1000000;
    return *mf;
}

map<char, ULONG64>& DRecordData::mult_factor =
    DRecordData::set_multfactor();



int TraceReplayConfig::import(char* cfgfile)
{
    TextDataSet cfg; // general text data set containing multiple lun record
    int ret;

    if((ret = cfg.open(cfgfile)) != 0)
        return ret;

    if(cfg.start() == false)
        return 1;

    DRecordData* rec; // each data record
    lunCfgs.clear();  // clear map content, mapping int->LUNCfg

    // iterating through multiple lun record
    while((rec = cfg.next()))
    {
        int lun = (*rec)[LUN_FIELD].i;   // get LUN number for this record line
        string* file = (*rec)["file"].s; // corresponding record file for this LUN
        int fd;

        if(file->empty())
        {
            fprintf(stderr, "WARN: no file supplied for lun %d; skipping\n", lun);
            continue;
        }

        if((fd = open(file->c_str(), O_RDWR | O_DIRECT)) < 0)
        {
            // open record file for this LUN
            fprintf(stderr, "WARN: cannot open %s; skipping lun %d\n",
                    file->c_str(), lun);
            continue;
        }

        if(debug)
        {
            fprintf(stderr, "Using %s as lun %d: Replay Parameters:\n%s\n", file->c_str(), lun,
                    rec->str(cfg.delim).c_str());
        }

        // setup lun configuration according to each lun record
        lunCfgs[lun].filename = file;
        lunCfgs[lun].fd = fd;
        lunCfgs[lun].lba_start = (*rec)["start_offset"].i / DISK_BLOCK_SIZE;
        lunCfgs[lun].nblks = (*rec)["range_nbytes"].i / DISK_BLOCK_SIZE;
        lunCfgs[lun].lba_shift = (*rec)["offset_shift"].i / DISK_BLOCK_SIZE;
        lunCfgs[lun].lba_scale = (*rec)["offset_scale"].d;
        lunCfgs[lun].iosize_scale = (*rec)["iosize_scale"].d;
        lunCfgs[lun].start_usecs = (*rec)["start_usecs"].d; //changed to double, which it should be
        //lunCfgs[lun].start_usecs = (*rec)["start_usecs"].i;
        lunCfgs[lun].num_usecs = (*rec)["num_usecs"].i;
        lunCfgs[lun].await_reply = (*rec)["await_reply"].i;
        lunCfgs[lun].slack_scale = (*rec)["slack_scale"].d;
#if 0
        fprintf(stderr, FMT_ULONG64 " " FMT_ULONG64 " " FMT_ULONG64 " " FMT_ULONG64 "\n",
                lunCfgs[lun].lba_start,
                lunCfgs[lun].nblks,
                lunCfgs[lun].start_usecs, lunCfgs[lun].num_usecs);
#endif /* 0 */
    }

    cfg.cleanup();
    return 0;
}

/** Need to use that function either here or in the HFPlayerUtils.h */
/** double TraceReplayConfig::frequence_speed()*/

int TraceReplayConfig::rescale(
    DRecordData* iorec,
    int& fd, /* out */
    unsigned long long& slack_scale, /* out */
    unsigned long long& offset, /* out */
    unsigned long long& size, /* out */
    unsigned long long& iotime) /* inout in nanosecond */
{
    int lun = (*iorec)[LUN_FIELD].i;
    map<int, LUNCfg>::iterator it;
    it = lunCfgs.find(lun);

    if(it == lunCfgs.end())
        return -1;          //The lun is not specified in the cfg file, skip this recored

    LUNCfg& cfg = (*it).second;
    LBA lba = (*iorec)["lba"].i;
    ULONG64 nblks = (*iorec)["nblks"].i;
    double usecs = (*iorec)["elapsed_usecs"].d; // read useconds in double
    //ULONG64 usecs = (ULONG64) (*iorec)["elapsed_usecs"].d;
#ifdef DEBUG
    printf("usecs=%ld,lba=%ld,nblks=%ld,start_usecs=%ld,num_usecs=%ld,lba_start=%ld,cfg.nblks=%ld\n", usecs, lba, nblks, cfg.start_usecs, cfg.num_usecs, cfg.lba_start, cfg.nblks);
#endif

    // This is the first filter, check if trace record is in the time window specified in the cfg file.
    if((usecs < cfg.start_usecs) || (usecs >= (cfg.start_usecs + cfg.num_usecs)))
        return -1;

    //This is the second filter, check if the trace record is in the LBA range specified in the cfg file.
    if((lba < cfg.lba_start) || (lba >= (cfg.lba_start + cfg.nblks)))
    {
        if(rotate_outOfRange == true)
        {
            if(lba < cfg.lba_start)
            {
                lba += cfg.lba_start;
            }
            else
            {
                lba = lba % (cfg.lba_start + cfg.nblks);
            }
        }
        else
            return -1;
    }

    fd = cfg.fd;
    // Now we know the trace record should be executed, scale it as needed
    // Always shift the start time by the config file value, so the replay starts at zero
    usecs = usecs - cfg.start_usecs;
    usecs = usecs * cfg.slack_scale;  //TODO need to change cfg file to have time_scale value
    iotime = (unsigned long long)(usecs * 1000) ; // convert to nanoseconds
    // Don't do offset scaling for now  offset = (((ULONG64) (lba * cfg.lba_scale) + cfg.lba_shift) * DISK_BLOCK_SIZE;
    offset = lba * DISK_BLOCK_SIZE;
    // Don't do size scaling for now  size = nblks * cfg.iosize_scale * DISK_BLOCK_SIZE;
    size = nblks * DISK_BLOCK_SIZE;

    if(cfg.await_reply)
        slack_scale = cfg.slack_scale;
    else
        slack_scale = -1; /* send IO requests at full speed without waiting for replies */

    return 0;
}

int TraceReplayConfig::dep_rescale(
    DRecordData* iorec,
    int& fd, /* out */
    unsigned long long& slack_scale, /* out */
    unsigned long long& offset, /* out */
    unsigned long long& size, /* out */
    unsigned long long& dep_parent_completionTime, /* inout in nanosecond */
    unsigned int& slack_time)  /* inout in nanosecond */
{
    int lun = (*iorec)[LUN_FIELD].i;
    map<int, LUNCfg>::iterator it;
    it = lunCfgs.find(lun);

    if(it == lunCfgs.end())
        return -1;          //The lun is not specified in the cfg file, skip this recored

    LUNCfg& cfg = (*it).second;
    LBA lba = (*iorec)["lba"].i;
    ULONG64 nblks = (*iorec)["nblks"].i;
    double usecs = (*iorec)["elapsed_usecs"].d; // read useconds in double
    //ULONG64 usecs = (ULONG64) (*iorec)["elapsed_usecs"].d;
#ifdef DEBUG
    printf("usecs=%ld,lba=%ld,nblks=%ld,start_usecs=%ld,num_usecs=%ld,lba_start=%ld,cfg.nblks=%ld\n", usecs, lba, nblks, cfg.start_usecs, cfg.num_usecs, cfg.lba_start, cfg.nblks);
#endif

    // This is the first filter, check if trace record is in the time window specified in the cfg file.
    if((usecs < cfg.start_usecs) || (usecs >= (cfg.start_usecs + cfg.num_usecs)))
        return -1;

    //This is the second filter, check if the trace record is in the LBA range specified in the cfg file.
    if((lba < cfg.lba_start) || (lba >= (cfg.lba_start + cfg.nblks)))
    {
        if(rotate_outOfRange == true)
        {
            if(lba < cfg.lba_start)
            {
                lba += cfg.lba_start;
            }
            else
            {
                lba = lba % (cfg.lba_start + cfg.nblks);
            }
        }
        else
            return -1;
    }

    fd = cfg.fd;
    // Now we know the trace record should be executed, scale it as needed
    // Always shift the start time by the config file value, so the replay starts at zero
    dep_parent_completionTime = dep_parent_completionTime - (unsigned long long)(cfg.start_usecs * 1000);
    slack_time =  slack_time * cfg.slack_scale;
    // Don't do offset scaling for now  offset = (((ULONG64) (lba * cfg.lba_scale) + cfg.lba_shift) * DISK_BLOCK_SIZE;
    offset = lba * DISK_BLOCK_SIZE;
    // Don't do size scaling for now  size = nblks * cfg.iosize_scale * DISK_BLOCK_SIZE;
    size = nblks * DISK_BLOCK_SIZE;

    if(cfg.await_reply)
        slack_scale = cfg.slack_scale;
    else
        slack_scale = -1; /* send IO requests at full speed without waiting for replies */

    return 0;
}
