/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/**
 **    File: TextDataSet.h
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
#include <iostream>

#include "IOLogDumpSchema.h"

struct TextDataSet
{
private:
    ifstream* dumpin; // input file stream
    char* file; // file name
public:
    string delim; // delimiter
    DRecordSchema_t schema;
    DRecordData data;
    ULONG nrecords;
    ULONG recnum;
    int datastart;
    string buf;
    vector<string> fields;
    ULONG total_records;

    TextDataSet()
    {
        buf.clear();
    }

    /* open cfg file and set delimiter */
    inline int open(char* dumpfile, string delimiter = ",")
    {
        ifstream f(dumpfile);
        std::string line;

        for(total_records = 0 ; std::getline(f, line); ++total_records)
            ;

        f.close();
        file = dumpfile;
        dumpin = new ifstream(dumpfile);

        if(! dumpin->is_open())
        {
            fprintf(stderr, "Cannot open %s for reading.\n", dumpfile);
            return 1;
        }

        delim = delimiter;
        return 0;
    }
    /* Set the stream pointer back to the start of the data*/
    inline bool restart()
    {
        dumpin->clear();
        dumpin->seekg(datastart);
        recnum = 0;
        nrecords = 0;
        return NULL;
    }
    /* read the 1st line and parse out schema (field names) */
    inline bool start()
    {
        recnum = 0;
        nrecords = 0;
        buf.clear();
        getline(*dumpin, buf); //read the 1st line into buf

        if(dumpin->eof())
        {
            fprintf(stderr, "%s: premature end of file\n", file);
            return false;
        }

        datastart = dumpin->tellg();
        mystrtolower(buf);

        if(buf[0] == '#')  // if headerline with "#"
            buf.erase(0, 1);

        split(fields, buf, delim);
        schema.import(fields); // schema is set with headerline fields names
        data.set_schema(&schema);
        return true;
    }

    /* read next line and import parsed fields value into 'data' */
    inline DRecordData* next()
    {
        do
        {
            buf.clear();
            getline(*dumpin, buf);

            if(dumpin->eof())
                return NULL;
        }
        while(buf[0] == '#');    // skip line with '#'

        split(fields, buf, delim);
        data.import(fields); // import record line to 'data'
        ++ recnum;
        ++ nrecords;
        return &data;
    }

    inline void cleanup()
    {
        dumpin->close();
        delete dumpin;
    }

    inline void showProgress()
    {
        unsigned step = total_records / 100 ;

        if(step != 0)
            if((nrecords % (step / 100))  == 0)
            {
                printf("\r %.2f%% Progress...", (float) nrecords / (float) step);
                fflush(stdout);
            }
    }

};
