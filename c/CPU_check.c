/**
 **    File:  CPU_check.c
 **    Authors:  Ibra Fall
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

struct CpuInfo
{
    int processor;
    char vendor_id[50];
    int family;
    int model;
    char model_name[50];
    int stepping;
    float freq;
    char cache[20];
};

int main()
{
    struct CpuInfo info = {0, "", 0, 0, "", 0, 0.0, ""};
    FILE* cpuInfo;

    if((cpuInfo = fopen("/proc/cpuinfo", "rb")) == NULL)
    {
        printf("Error!! Cannot open the file");
    }
    else
    {
        while(!feof(cpuInfo))
        {
            fread(&info, sizeof(struct CpuInfo), 1, cpuInfo);

            //while(fread(&info, sizeof(struct CpuInfo), 1, cpuInfo) != NULL)

            if(info.processor != 0)
            {
                printf("%f\n", freq);
            }

            //printf("%d\n%s\n%d\n%d\n%s\n%d\n%.2lf\n%s\n", info.processor, info.vendor_id, info.family, info.model, info.model_name, info.stepping, info.freq, info.cache);		}
            break;
        }
    }

    return 0;
}
