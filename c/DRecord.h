/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/**
 **     File:  DRecord.h
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

#ifndef __DRECORD_H__
#define __DRECORD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include "HFPlayerTypes.h"

using namespace std;

extern void split(vector<string> &result, const string str, string delim);
extern void mystrtoupper(string& str);
extern void mystrtolower(string& str);

/*
 * DRecord (stands for Dynamic record structure)
 * A Generic class to manipulate C++ structures whose schema is determined at
 * run time.
 * The structure is stored as a vector of ULONG64 numbers, which are big
 * enough to hold most values of interest in IO activity logging.
 *
 * DRecordSchema holds field names, types and ordering.
 * DRecordData holds field values conforming to the schema.
 *
 * Importing trace and stats records into DRecords will enable writing generic
 * C++ trace analysis programs that efficiently handle all IO trace dump
 * versions.
 */

typedef struct DRecordSchema
{
    string type; // structure type name
    map<string, int> indices;
    vector<string> fields;
    typedef enum { MyInteger = 0, MyDouble, MyStr } FieldType;
    string typeSuffix[3];
    vector<FieldType> types;
    string mystr;

    DRecordSchema(string typeName)
    {
        type = typeName;
        typeSuffix[MyInteger] = ".i";
        typeSuffix[MyDouble] = ".d";
        typeSuffix[MyStr] = ".s";
    }

    DRecordSchema()
    {
        typeSuffix[MyInteger] = ".i";
        typeSuffix[MyDouble] = ".d";
        typeSuffix[MyStr] = ".s";
    }

    inline bool has(string field)
    {
        return indices.count(field) > 0;
    }

    inline int get(string field)
    {
        map<string, int>::iterator it = indices.find(field);

        if(it == indices.end())
        {
            if(field.compare("dep_parent_count") == 0)
            {
                fprintf(stderr, "Please prodive Annotated trace for Dependency Replay mode\n");
            }
            else
                fprintf(stderr, "unknown field %s referenced\n", field.c_str());

            exit(1);
        }

        return (*it).second;
    }

    inline void add(string field)
    {
        add(field, MyInteger);
    }

    inline void add(string field, FieldType type)
    {
        fields.push_back(field);
        types.push_back(type);
        indices[field] = fields.size() - 1;
    }

    /* Append fields (after nskip) from src_schema to myself */
    inline void append(DRecordSchema* src_schema, int nskip = 0)
    {
        for(unsigned i = nskip; i < src_schema->fields.size(); ++ i)
            add(src_schema->fields[i], src_schema->types[i]);
    }

    inline void set(string field, int index)
    {
        set(field, MyInteger, index);
    }

    inline void set(string field, FieldType type, int index)
    {
        fields[index] = field;
        types[index] = type;
        indices[field] = index;
    }

    inline const string& str(string delim)
    {
        size_t i = 0;
        mystr.clear();

        if(i < fields.size())
            mystr += fields[i] + typeSuffix[types[i]];

        for(i++ ; i < fields.size(); ++ i)
        {
            mystr += delim;
            mystr += fields[i] + typeSuffix[types[i]];
        }

        return mystr;
    }

    inline void import(vector<string>& infields)
    {
        fields = infields;
        types.clear();
        indices.clear();

        for(unsigned i = 0; i < fields.size(); ++ i)
        {
            int suffixlen = 2;
            size_t pos = fields[i].size() - suffixlen;

            if(fields[i].compare(pos, suffixlen, typeSuffix[MyInteger]) == 0)
            {
                fields[i].erase(pos);
                types.push_back(MyInteger);
            }
            else
                if(fields[i].compare(pos, suffixlen, typeSuffix[MyDouble]) == 0)
                {
                    fields[i].erase(pos);
                    types.push_back(MyDouble);
                }
                else
                    if(fields[i].compare(pos, suffixlen, typeSuffix[MyStr]) == 0)
                    {
                        fields[i].erase(pos);
                        types.push_back(MyStr);
                    }

            indices[fields[i]] = i;
        }
    }
} DRecordSchema_t;

typedef union
{
    ULONG64 i;
    double d;
    string* s;
} MyValue;

typedef struct DRecordData
{
    static map<char, ULONG64>& mult_factor;
    DRecordSchema* schema;
    vector<MyValue> value;
    string mystr;

    DRecordData(DRecordSchema* in_schema)
    {
        set_schema(in_schema);
    }

    DRecordData()
    {
        schema = 0;
    }

    static map<char, ULONG64>& set_multfactor();

    inline void set_schema(DRecordSchema* new_schema)
    {
        schema = new_schema;
        reserve();
    }

    inline MyValue& operator [](string field)
    {
        int i = schema->get(field);
//		fprintf(stderr, "%s = %d\n", field.c_str(), i);
        return (*this)[i];
    }

    inline MyValue& operator [](int ind)
    {
//		fprintf(stderr, "ind = %d\n", ind);
        return value[ind];
    }

    inline int size()
    {
        return (int) schema->fields.size();
    }

    inline void reserve()
    {
        if(schema)
            value.reserve(size());
    }

    inline void zero()
    {
        for(int i = 0; i < size(); ++ i)
            (*this)[i].i = 0;
    }

    inline void copy(DRecordData* rec, int start = 0, int nelems = 0)
    {
        if(nelems == 0)
            nelems = rec->size() - start;

        for(int i = start; i < start + nelems; ++ i)
        {
            if(has(rec->schema->fields[i]))
                (*this)[rec->schema->fields[i]] = (*rec)[i];
        }
    }

    inline bool has(string field)
    {
        return schema->has(field);
    }

    inline const string& str(string delim)
    {
        int i = 0;
        mystr.clear();

        if(i < size())
            print(i);

        for(++ i; i < size(); ++ i)
        {
            mystr += delim;
            print(i);
        }

        return mystr;
    }

    inline void print(int i, string* modstr = 0)
    {
        char buf[20];

        if(! modstr)
            modstr = &mystr;

        switch(schema->types[i])
        {
            case DRecordSchema::MyInteger:
                sprintf(buf, FMT_ULONG64, value[i].i);
                (*modstr) += buf;
                break;
            case DRecordSchema::MyDouble:
                sprintf(buf, "%.3f", value[i].d);
                (*modstr) += buf;
                break;
            case DRecordSchema::MyStr:
                (*modstr) += *value[i].s;
                break;
            default:
                ;
        }
    }

    inline void import(vector<string>& valuestr)
    {
        if(! schema)
            return;

        value.clear();

        for(unsigned i = 0; i < valuestr.size(); ++ i)
        {
            switch(schema->types[i])
            {
                case DRecordSchema::MyInteger:
                {
                    int last = valuestr[i].size() - 1;

                    if(last > 0)
                    {
                        char c = tolower(valuestr[i][last]);

                        if(mult_factor.count(c) > 0)
                        {
                            ULONG64 factor = DRecordData::mult_factor[c];
                            double d;
                            valuestr[i].erase(last);
                            sscanf(valuestr[i].c_str(), "%lf", &d);
                            value[i].i = (ULONG64)(d * factor);
                            break;
                        }
                    }

                    sscanf(valuestr[i].c_str(), FMT_ULONG64, &value[i].i);
                    break;
                }
                case DRecordSchema::MyDouble:
                    sscanf(valuestr[i].c_str(), "%lf", &value[i].d);
                    break;
                case DRecordSchema::MyStr:
                    value[i].s = &valuestr[i];
                    break;
            }
        }
    }
} DRecordData_t;

#endif /* __DRECORD_H__ */
