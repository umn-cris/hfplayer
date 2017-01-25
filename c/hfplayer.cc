/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/**
 **    File:  hfplayer.cc
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
#include <fstream>
#include <map>
#include <vector>

#include "HFPlayerUtils.h"
#include "HFversion.h"

using namespace std;
static const char* delim = ",";
static const char* pgmname;
char* cfgfile;
unsigned int warmupIO = 0;
unsigned int WT = 2; // number of worker threads to launch, default is 2
int doCapture = 0;
int shortIATnsec = 0; //turns of bundling by default
int stopOnError = 0;
int stopOnOverload = 0;
int max_inflight = 8192; //large value is set in here for asap replay. You can always override this by -q command line option
int totalCores; //physical cores on all cpus
int numSockets; //number of physical cpu sockets
int cores_per_socket; //physical cores per socket
int debuglevel = 0;
bool dep_replay = false;
bool asap_replay = false;
bool load_replay = false;
double max_speed = 0;

void usage()
{
    fprintf(stderr,
            "Usage: %s -mode <replay_mode> -d -v -c <cpu_clock_speed> -so -se -wlc -cfg <config_file> -nt <threads> -q <max outstanding IOs> <trace_file>.csv\n\n\
    -mode <replay_mode> select from [hf,dep,afap,load] replay mode. Default mode is hf. \n\
    \t hf: High-Fidelity replay mode which reproduce exact the same a) request ordering b) arrival time c) controller load profile \n\
    \t dep: I/O Dependency replay mode which maintains dependencies between I/O requests to reproduce original application behaviour on faster storage\n\
    \t afap: As Fast As Possible replay mode which does not impose any limitation on the replay speed while issuing I/Os in-order\n\
    \t Load: I/O Load replay mode which replay I/Os if inflight I/O is smaller than target load in the original tracefile\n\
    -d <Debug level> \n\
    -c <cpu clock speed>\n\
    -cfg <replay_config_file>.csv\n\
          Read the Trace replay configuration parameters from given csv file\n\
    -nt <number of worker threads to launch>\n\
    -so Stop on overload (-q/WT reached) \n\
    -se Stop on error.  Stop if an error occurs, otherwise continue (default).\n\
    -wlc Initiate a workload capture trace \n\
    -q  <maximum total outstanding IOs>\n\
    -b  <short iat> If IAT less that this number of nanoseconds, the IO is bundled\n\
    -w  <number of warmup IOs to execute (default is none)\n\
    -v  Prints the current version of the replayer and exits\n\
    ", pgmname);
    exit(1);
}

void print_version()
{
    printf("You are running version %s of %s.\n", HF_VERSION, pgmname);
    exit(0);
}

int main(int argc, char* argv[])
{
    char* dumpfile = 0;
    cfgfile = 0;
    char* outprefix = 0;
    int i = 0;
    pgmname = argv[i++];
    totalCores = get_number_cores();
    numSockets = get_number_sockets();
    cores_per_socket = totalCores / numSockets;

    if(i >= argc)
    {
        usage();
    }

    while(i < argc)
    {
        if(strcmp(argv[i], "-v") == 0)
        {
            print_version();
        }

        if(strcmp(argv[i], "-mode") == 0)
        {
            ++i;

            if(i == argc)
            {
                fprintf(stderr, "Unknown replay mode is provided\n");
                usage();
            }

            if(strcmp(argv[i], "dep") == 0)
            {
                dep_replay =  true;
            }
            else
                if(strcmp(argv[i], "afap") == 0)
                {
                    asap_replay = true;
                }
                else
                    if(strcmp(argv[i], "load") == 0)
                    {
                        load_replay = true;
                    }
                    else
                        if(strcmp(argv[i], "hf") != 0)
                        {
                            fprintf(stderr, "Unknown replay mode is provided\n");
                            usage();
                        }

            ++i;
        }

        if(strcmp(argv[i], "-d") == 0)
        {
            if(++ i >= argc)
            {
                fprintf(stderr, "supply a debug level with -d\n");
                usage();
            }

            debuglevel = atoi(argv[i++]);
        }
        else
            if(strcmp(argv[i], "-c") == 0)
            {
                if(++ i >= argc)
                {
                    fprintf(stderr, "The -c option requires the CPU speed in GHz\n");
                    usage();
                }

                max_speed = atof(argv[i++]);
            }
            else
                if(strcmp(argv[i], "-cfg") == 0)
                {
                    if(++ i >= argc)
                    {
                        fprintf(stderr, "The -cfg option requires the configuration file name\n");
                        usage();
                    }

                    cfgfile = argv[i++];
                }
                else
                    if(strcmp(argv[i], "-q") == 0)
                    {
                        if(++ i >= argc)
                        {
                            fprintf(stderr, "supply number of outstanding IOs allowed\n");
                            usage();
                        }

                        max_inflight = atoi(argv[i++]);
                    }
                    else
                        if(strcmp(argv[i], "-nt") == 0)
                        {
                            if(++ i >= argc)
                            {
                                fprintf(stderr, "supply number of threads to -nt\n");
                                usage();
                            }

                            WT = atoi(argv[i++]);
                        }
                        else
                            if(strcmp(argv[i], "-b") == 0)
                            {
                                if(++ i >= argc)
                                {
                                    fprintf(stderr, "supply short IAT time in nanoseconds to -b\n");
                                    usage();
                                }

                                shortIATnsec = atoi(argv[i++]);
                            }
                            else
                                if(strcmp(argv[i], "-w") == 0)
                                {
                                    if(++ i >= argc)
                                    {
                                        fprintf(stderr, "supply number of warmup IOs to -w\n");
                                        usage();
                                    }

                                    warmupIO = atoi(argv[i++]);
                                }
                                else
                                    if(strcmp(argv[i], "-so") == 0)
                                    {
                                        stopOnOverload = 1;
                                        i++;
                                    }
                                    else
                                        if(strcmp(argv[i], "-se") == 0)
                                        {
                                            stopOnError = 1;
                                            i++;
                                        }
                                        else
                                            if(strcmp(argv[i], "-wlc") == 0)
                                            {
                                                doCapture = 1;
                                                i++;
                                            }
                                            else
                                            {
                                                dumpfile = argv[i];
                                                break;
                                            }
    }

    if(warmupIO < WT)
    {
        warmupIO = 0;
    }

    if(check_hyperthreading())
    {
        return 1;
    }

    if(max_speed == 0)
    {
        max_speed = get_cpu_speed();     /* Clock Speed if not specified by the user */

        if(max_speed == 0)               /** Cannot open the cpuinfo file or cannot read the cpu frequency **/
        {
            printf(" ===================================================\n ");
            printf(" Error!!!!! Cannot find the current machine speed   \n ");
            printf(" ===================================================\n ");
            exit(EXIT_FAILURE);
        }
    }

    if((! dumpfile) || (! cfgfile))
        usage();

    set_interrupt_affinity();
    TextDataSet* trace = new TextDataSet();
    int ret;

    if((ret = trace->open(dumpfile)) != 0)
        return ret;

    TraceReplayConfig replaycfg;

    if((ret = replaycfg.import(cfgfile)))
        return ret;

    return do_ioreplay(trace, &replaycfg);
}
