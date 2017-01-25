/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
/**
 **    File: stdDeviation.cc
 **    Authors:  Alireza Haghdoost
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

/*
 * Delta1: consecutive requests in the replay trace
 * Delta2: pair-wise (based on req order) IAT difference between original and replay trace
 * Delta3: pair-wise (based on req order) arrival time difference between original and replay trace
 *
 */

#include <cmath>
#include "stdDeviation.h"
#include "TextDataSet.h"
#include <queue>
#include <map>

using namespace std;

void usage()
{
    fprintf(stderr, "Usage: stdDeviation <iotrace_dump1>.csv <iotrace_dump2>.csv ...");
    exit(1);
}

double prevTime;
double calcDiff(DRecordData* rec)
{
    double ret = (*rec)["elapsed_usecs"].d - prevTime;
    prevTime = (*rec)["elapsed_usecs"].d;
    return ret;
}

void printDevOrig(int argc, char* argv[], double* stdDeviation, double* meanValue
                  , double* meanInFlight, double* stdDevInFlight,  uint32_t* maxInFlight
                  , double* meanLatency, double* stdDevLatency,  double* maxLatency
                  , int* missingIOsArray
                  , int* outOrderIOsArray
                  , double* displaceAvgArray
                  , int* displaceStdDevArray
                  , int* loadMAEarray
                  , double* IATmaeArray
                  , int origIndex)
{
    cout << "Trace file";
    cout << ", IAT Avg, IAT Dev, IAT MAE";
    cout << ", Load Avg, Load Dev, Load MAE, Load Max";
    cout << ", Lat Avg , Lat Dev , Lat Max";
    cout << ", OutOrderIOs, Displace Avg, Displace Dev";
    cout << ", MissingIOs";
    cout << endl;
    //print original trace first
    int i = origIndex;
    cout << argv[i] << " [Orig]";
    cout << ", " << meanValue[i] << ", " << stdDeviation[i] << ", " << IATmaeArray[i];
    cout << ", " << meanInFlight[i] << ", " << stdDevInFlight[i] << ", " << loadMAEarray[i] << ", " << maxInFlight[i];
    cout << ", " << meanLatency[i] << " , " << stdDevLatency[i] << ", " << maxLatency[i]; // latency stats
    cout << ", " << outOrderIOsArray[i] << ", " << displaceAvgArray[i] << ", " << displaceStdDevArray[i] ;
    cout << ", " << missingIOsArray[i] ;
    cout << endl;

    for(i = 1 ; i < argc ; i++)
    {
        if(i == origIndex || i == origIndex - 1)
            continue;

        cout << argv[i];
        cout << ", " << meanValue[i] << ", " << stdDeviation[i] << ", " << IATmaeArray[i];
        cout << ", " << meanInFlight[i] << ", " << stdDevInFlight[i] << ", " << loadMAEarray[i] << ", " << maxInFlight[i];
        cout << ", " << meanLatency[i] << " , " << stdDevLatency[i] << ", " << maxLatency[i]; // latency stats
        cout << ", " << outOrderIOsArray[i] << ", " << displaceAvgArray[i] << ", " << displaceStdDevArray[i] ;
        cout << ", " << missingIOsArray[i] ;
        cout << endl;
    }
}

void printDeviations(int argc, char* argv[], double* stdDeviation, double* meanValue
                     , double* meanInFlight, double* stdDevInFlight,  uint32_t* maxInFlight
                     , double* meanLatency, double* stdDevLatency,  double* maxLatency)
{
    cout << "Trace file";
    cout << " , Mean Intr-Arrival , Int-Arrival Dev";
    cout << " , Mean Load , Load Dev, maxInFlight";
    cout << " , Mean Latency , Latency Dev , maxLatency";
    cout << endl;

    for(int i = 1 ; i < argc ; i++)
    {
        if(strcmp("-o", argv[i]) != 0)
            cout << argv[i]; // trace file name

        cout << " , " << meanValue[i] << " , " << stdDeviation[i]; // inter arrival time stats
        cout << " , " << meanInFlight[i] << " , " << stdDevInFlight[i] << " , " << maxInFlight[i]; // Load stats
        cout << " , " << meanLatency[i] << " , " << stdDevLatency[i] << " , " << maxLatency[i]; // latency stats
        cout << endl;
    }
}

void calcDelta3(int argc,  char* argv[], int originalTraceIndex, double* meanValue)
{
    TextDataSet* trace = new TextDataSet();
    DRecordData* rec;
    //process originalTrace file
    trace->open(argv[originalTraceIndex]);

    if(trace->start() == false)
        exit(1);

    vector<double> origIssueTime;
    vector<double> tempIssueTime;

// 	trace->next(); //skip second line of trace file ( time 0)
    while((rec = trace->next()))
    {
        origIssueTime.push_back((*rec)["elapsed_usecs"].d);
    }

    trace->cleanup();
    cout << "Delta3: Trace file , MAE , StdDevAE, MASE" << endl;
    int i = 1; // while loop counter

    //process other replayed trace files
    while(i < argc)
    {
        if(strcmp("-o", argv[i]) == 0)
        {
            i += 2; // jump over orig trace
            continue;
        }

        tempIssueTime.clear();
        trace->open(argv[i]);

        if(trace->start() == false)
            exit(1);

// 		trace->next(); //skip second line of trace file ( time 0)
        while((rec = trace->next()))
        {
            tempIssueTime.push_back((*rec)["elapsed_usecs"].d);
        }

        trace->cleanup();

        if(tempIssueTime.size() != origIssueTime.size())
        {
            cerr << " Mismatch Error: " << endl;
            cerr << " original trace has " << origIssueTime.size() << " items" << endl;
            cerr << " " << argv[i] << " trace file has " << tempIssueTime.size() << " items" << endl;
            exit(1);
        }

        // calc MAE (mean absolute error of delta 3
        double absErrSum = 0;
        vector<double>::size_type vectorSize = origIssueTime.size();

        for(vector<double>::size_type j = 0 ; j < vectorSize ; ++ j)
        {
            absErrSum +=  abs(origIssueTime[j] - tempIssueTime[j]);  // absolute error
        }

        double mae_i = (absErrSum) / vectorSize;
        // calc std deviation for absolute error
        double powerTwoSum = 0;

        for(vector<double>::size_type j = 0 ; j < vectorSize ; ++ j)
        {
            double absErr =  abs(origIssueTime[j] - tempIssueTime[j]);  // absolute error
            double dev = mae_i - absErr; //deviation from mae
            double absErrPowerTwo = dev * dev;
            powerTwoSum	+=	absErrPowerTwo;
        }

        double stdDevAE = sqrt(powerTwoSum / vectorSize);
        cout << argv[i] << " , \t" << mae_i << " , \t" << stdDevAE << " , \t" << mae_i / meanValue[originalTraceIndex] << endl;
        ++ i ;
    }
}

void calcLoadIATDelta(int argc,  char* argv[], int originalTraceIndex,
                      int* missingIOsArray,
                      int* outOrderIOsArray,
                      double* displaceAvgArray,
                      int* displaceStdDevArray,
                      int* loadMAEarray,
                      double* IATmaeArray)
{
    vector<double> origInterTime;
    vector<double> replayInterTime;
    vector<int> origLoad;
    vector<int> replayLoad;
    vector<unsigned long> origLba;
    vector<unsigned long> replayLba;
    double prevTime = 0;
    bool isFirst = true;
    TextDataSet* trace = new TextDataSet();
    DRecordData* rec;
    //process originalTrace file
    trace->open(argv[originalTraceIndex]);

    if(trace->start() == false)
    {
        cerr << "cannot read original trace file" << endl;
        exit(1);
    }

    while((rec = trace->next()))
    {
        double currTime = (*rec)["elapsed_usecs"].d ;

        if(isFirst)
        {
            //skip second line of trace file, inter-arrival time for the first line does not have any meaning.
            prevTime = currTime;
            isFirst = false;
        }
        else
        {
            assert(currTime - prevTime >= 0);
            origInterTime.push_back(currTime - prevTime);
            prevTime = currTime ;
        }

        origLoad.push_back((*rec)["inflight_ios"].i);
        origLba.push_back((*rec)["lba"].i);
    }

    trace->cleanup();
    int i = 1; // while loop counter

    //process other replayed trace files
    while(i < argc)
    {
        if(strcmp("-o", argv[i]) == 0)
        {
            assert(i + 1 == originalTraceIndex);
            i += 2; // jump over orig trace file name and -o
            continue;
        }

        trace->open(argv[i]);

        if(trace->start() == false)
        {
            cerr << "can not open replay trace file: " << argv[i] << endl;
            exit(1);
        }

        // initialization
        long inOrderIOs = 0;
        long missingIOs = 0;
        long outOrderIOs = 0;
        long displaceSum = 0;
        queue<int> displaceDiffQ;
        long loadAbsErrSum = 0;
        queue<int> loadAbsErrQ;
        double IATabsErrSum = 0;
        queue<int> IATabsErrQ ;
        replayInterTime.clear();
        replayLoad.clear();
        replayLba.clear();
        prevTime = 0;
        isFirst = true;

        while((rec = trace->next()))
        {
            double currTime = (*rec)["elapsed_usecs"].d ;

            if(isFirst)
            {
                //skip second line of trace file, inter-arrival time for the first line does not have any meaning.
                prevTime = currTime;
                isFirst = false;
            }
            else
            {
                assert(currTime - prevTime >= 0);
                replayInterTime.push_back(currTime - prevTime);
                prevTime = currTime ;
            }

            replayLoad.push_back((*rec)["inflight_ios"].i);
            replayLba.push_back((*rec)["lba"].i);
        }

        if(replayLba.size() > origLba.size())
        {
            cerr << "ERROR: replay trace size is larger than original trace" << endl;
            exit(1);
        }

        trace->cleanup();
        size_t orig_index = 0;
        size_t replay_index = 0;

        //find comparison pairs based on Lba
        while(orig_index < origLba.size())
        {
            if(origLba[orig_index] ==  replayLba[replay_index])
            {
                orig_index++;

                do
                {
                    replay_index++;
                }
                while(~ replayLba[replay_index] == 0);

                inOrderIOs++;
                continue;
            }

            // else
            bool missingLba = false;
            size_t scan_index = replay_index;

            do
            {
                scan_index++;

                if(scan_index == replayLba.size())
                {
                    missingLba = true;
                    break;
                }
            }
            while(origLba[orig_index] != replayLba[scan_index]);

            if(missingLba)
                missingIOs++;
            else
                if(orig_index != scan_index)
                {
                    // mark to exclude this index in the future
                    replayLba[scan_index] = 0xffffffffffffffff;
                    // orig LBA is displaced in the replay trace
                    outOrderIOs++;
                    // calculate avg displacement
                    int displace = scan_index - orig_index ;
                    displaceSum += displace; // used later to calc avg for displacement
                    displaceDiffQ.push(displace); // used later to calc std dev for displacement
                }
                else
                    inOrderIOs++;

            ++orig_index;
        }

        assert(orig_index == inOrderIOs + outOrderIOs + missingIOs);   // make sure we count all IO requests
        size_t min_size = min<size_t>(origLoad.size() , replayLoad.size());

        for(size_t k = 0; k < min_size ; k++)
        {
            // orig Req is inorder in the replay trace
            // now compare Load and IAT and calc MAE
            int loadAbsErr = abs(replayLoad[k] - origLoad[k]);
            loadAbsErrSum += loadAbsErr;
            loadAbsErrQ.push(loadAbsErr); // is not used now

            if(k != replayInterTime.size() - 1)
                // do not calc abs Err for last IAT
            {
                double IATabsErr = abs(replayInterTime[k] - origInterTime[k]) ;
                IATabsErrSum += IATabsErr;
                IATabsErrQ.push(IATabsErr); // is not used now
            }
        }

        double displaceAvg = 0 ;
        int displaceStdDev = 0 ;

        if(outOrderIOs)
        {
            // calc displacement avg and deviation
            displaceAvg = (double) displaceSum / (double) outOrderIOs ;
            // calc deviation of interarrival time
            long powerTwoSum = 0 ;

            while(!displaceDiffQ.empty())
            {
                int deviation = displaceDiffQ.front() - displaceAvg ;
                long devPowerTwo = deviation * deviation;
                powerTwoSum += devPowerTwo;
                displaceDiffQ.pop();
            }

            displaceStdDev = powerTwoSum ? sqrt(powerTwoSum / outOrderIOs) : 0 ;
        }

        // displaceAvg and displaceStdDev are ready now
        // calc MAE for Load
        int loadMAE = loadAbsErrSum / loadAbsErrQ.size();
        // calc MAE for IAT
        double IATmae = IATabsErrSum / IATabsErrQ.size();
        // update output arrays
        missingIOsArray[i] = missingIOs;
        outOrderIOsArray[i] = outOrderIOs;
        displaceAvgArray[i] =  displaceAvg;
        displaceStdDevArray[i] = displaceStdDev;
        loadMAEarray[i] =  loadMAE;
        IATmaeArray[i] = IATmae;
        //increament while loop counter
        ++ i ;
    } // end of while loop on the replay trace files

    missingIOsArray[originalTraceIndex] = 0;
    outOrderIOsArray[originalTraceIndex] = 0;
    displaceAvgArray[originalTraceIndex] = 0;
    displaceStdDevArray[originalTraceIndex] = 0;
    loadMAEarray[originalTraceIndex] = 0;
    IATmaeArray[originalTraceIndex] = 0;
}

void calcDelta2(int argc,  char* argv[], int originalTraceIndex)
{
    TextDataSet* trace = new TextDataSet();
    DRecordData* rec;
    //process originalTrace file
    trace->open(argv[originalTraceIndex]);

    if(trace->start() == false)
        exit(1);

    vector<double> origInterTime;
    vector<double> tempInterTime;
    rec = trace->next(); //skip second line of trace file, inter-arrival time for the first line does not have any meaning.
    double prevTime = (*rec)["elapsed_usecs"].d ;

    while((rec = trace->next()))
    {
        double currTime = (*rec)["elapsed_usecs"].d ;
        assert(currTime - prevTime >= 0);
        origInterTime.push_back(currTime - prevTime);
        prevTime = currTime ;
    }

    trace->cleanup();
    cout << "Delta2: Trace file , MAE , StdDevAE, MASE" << endl;
    int i = 1; // while loop counter

    //process other replayed trace files
    while(i < argc)
    {
        if(strcmp("-o", argv[i]) == 0)
        {
            i += 2; // jump over orig trace file name and -o
            continue;
        }

        tempInterTime.clear();
        trace->open(argv[i]);

        if(trace->start() == false)
            exit(1);

        rec = trace->next(); //skip first line of trace file
        prevTime = (*rec)["elapsed_usecs"].d ;

        while((rec = trace->next()))
        {
            double currTime = (*rec)["elapsed_usecs"].d;
            assert(currTime - prevTime >= 0);
            tempInterTime.push_back(currTime - prevTime);
            prevTime = currTime;
        }

        trace->cleanup();

        if(tempInterTime.size() != origInterTime.size())
        {
            cerr << " Mismatch Error: " << endl;
            cerr << " original trace has " << origInterTime.size() << " inter-arrival items" << endl;
            cerr << " " << argv[i] << " trace file has " << tempInterTime.size() << " inter-arrival items" << endl;
            exit(1);
        }

        // calc MAE (mean absolute error) of delta 2
        double absErrSum = 0;
        vector<double>::size_type vectorSize = origInterTime.size();

        for(vector<double>::size_type j = 0 ; j < vectorSize ; ++ j)
        {
            absErrSum +=  abs(origInterTime[j] - tempInterTime[j]);  // absolute error
        }

        double mae_i = (absErrSum) / vectorSize; // MAE for tempInterTime
        // calc std deviation for absolute error
        double powerTwoSum = 0;

        for(vector<double>::size_type j = 0 ; j < vectorSize ; ++ j)
        {
            double absErr =  abs(origInterTime[j] - tempInterTime[j]);  // absolute error
            double dev = mae_i - absErr; //deviation from mae
            double absErrPowerTwo = dev * dev;
            powerTwoSum	+=	absErrPowerTwo;
        }

        double stdDevAE = sqrt(powerTwoSum / vectorSize);
        //calc MASE denominator
        double maseDenominatorSum = 0;
        unsigned debugCounter = 0;

        for(vector<double>::size_type j = 1 ; j < vectorSize ; ++ j)
        {
            double diff = abs(origInterTime[j] -  origInterTime[j - 1]);
            maseDenominatorSum	+=	diff;
            ++ debugCounter;
        }

        assert((debugCounter + 1)	== vectorSize);
        double maseDenominator =	maseDenominatorSum / (vectorSize - 1) ;
        cout << argv[i] << " , \t" << mae_i << " , \t" << stdDevAE << " , \t" << mae_i / maseDenominator << endl;
        ++ i ;
    }
}



void calcGeneralInt(int argc, char* argv[], double mean[], double stdDev[] , uint32_t max[], const char* eventName)
{
    TextDataSet* trace = new TextDataSet();
    DRecordData* rec;
    //#double meanInFlight[argc];
    //#double stdDevInFlight[argc];
    int i = 1; // while loop counter

    //process other replayed trace files
    while(i < argc)
    {
        if(strcmp("-o", argv[i]) == 0)
        {
            ++i;
            continue;
        }

        trace->open(argv[i]);

        if(trace->start() == false)
            exit(1);

        uint64_t sum = 0;
        uint32_t totalLines =  0;
        max[i] = 0;
        queue<uint32_t> eventQ;

        while((rec = trace->next()))
        {
            sum += (*rec)[eventName].i;
            eventQ.push((*rec)[eventName].i);

            if(max[i] < (*rec)[eventName].i)
                max[i] = (*rec)[eventName].i;

            ++ totalLines;
        }

        mean[i] = (double) sum / (double) totalLines ;
        double powerTwoSum = 0;

        // calc deviation of interarrival time
        while(!eventQ.empty())
        {
            double deviation = eventQ.front() - mean[i] ;
            double devPowerTwo = deviation * deviation;
            powerTwoSum += devPowerTwo;
            eventQ.pop();
        }

        stdDev[i] =  sqrt(powerTwoSum / totalLines);
        ++i;
    }
}

void calcGeneralDouble(int argc, char* argv[], double mean[], double stdDev[] , double max[] , const char* eventName)
{
    TextDataSet* trace = new TextDataSet();
    DRecordData* rec;
    //#double meanInFlight[argc];
    //#double stdDevInFlight[argc];
    int i = 1; // while loop counter

    //process other replayed trace files
    while(i < argc)
    {
        if(strcmp("-o", argv[i]) == 0)
        {
            ++i;
            continue;
        }

        trace->open(argv[i]);

        if(trace->start() == false)
            exit(1);

        uint64_t sum = 0;
        uint32_t totalLines =  0;
        max[i] = 0;
        queue<double> eventQ;

        while((rec = trace->next()))
        {
            sum += (*rec)[eventName].d;
            eventQ.push((*rec)[eventName].d);

            if(max[i] < (*rec)[eventName].d)
                max[i] = (*rec)[eventName].d;

            ++ totalLines;
        }

        mean[i] = (double) sum / (double) totalLines ;
        double powerTwoSum = 0;

        // calc deviation of interarrival time
        while(!eventQ.empty())
        {
            double deviation = eventQ.front() - mean[i] ;
            double devPowerTwo = deviation * deviation;
            powerTwoSum += devPowerTwo;
            eventQ.pop();
        }

        stdDev[i] =  sqrt(powerTwoSum / totalLines);
        ++i;
    }
}


int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        usage();
    }

    TextDataSet* trace = new TextDataSet();
    double stdDeviation[argc];
    double meanValue[argc];
    queue<double> diffQ;
    int i = 1;
    int originalTraceIndex = -1;
    bool origProvided = false;

    // calculate IAT mean and std dev fo all input files
    while(i < argc)
    {
        if(strcmp("-o", argv[i]) == 0)
        {
            assert(origProvided == false);
            origProvided = true;
            originalTraceIndex = i + 1;
            ++ i;
            continue;
        }

        trace->open(argv[i]);

        if(trace->start() == false)
            exit(1);

        DRecordData* rec;
        ULONG64 totalLines = 0; //total intervals
        double totalDiffSum = 0;
        rec = trace->next();// skip first entry in the trace (actually second entry with header). inter-arrival time is not meaningful for the first entry
        prevTime = (*rec)["elapsed_usecs"].d ;

        // calculate mean value for inter arrival time
        while((rec = trace->next()))
        {
            double diff = calcDiff(rec);
            /*  if( diff <0 ){
            cout<<" Error: diff value is negative "<< diff <<" when time is: "<<prevTime<<" in offset:"<<(*rec)["dump_offset"].i <<endl;
            exit(1);
            }*/
            diffQ.push(diff);
            totalDiffSum += diff;
            ++ totalLines;
        }

        meanValue[i] = totalDiffSum / (double) totalLines;
        ULONG64 debugLineCount = totalLines;
        totalLines = 0;
        double powerTwoSum = 0;

        // calc deviation of interarrival time
        while(!diffQ.empty())
        {
            double deviation = diffQ.front() - meanValue[i] ;
            double devPowerTwo = deviation * deviation;
            assert(devPowerTwo >= 0);
            powerTwoSum += devPowerTwo;
            diffQ.pop();
            ++ totalLines;
        }

        assert(totalLines == debugLineCount);
        stdDeviation[i] =  sqrt(powerTwoSum / totalLines);
        trace->cleanup();
        ++ i ; // prepare for next file
    }

    //arrays for storing inFlight (Load) stats
    double meanInFlight[argc];
    double stdDevInFlight[argc];
    uint32_t maxInFlight[argc];
    calcGeneralInt(argc, argv, meanInFlight, stdDevInFlight, maxInFlight, "inflight_ios");
    //arrays for storing Latency stats
    double meanLatency[argc];
    double stdDevLatency[argc];
    double maxLatency[argc];
    calcGeneralDouble(argc, argv, meanLatency, stdDevLatency, maxLatency, "latency_usecs");

    if(origProvided)
    {
        // calculate MAE and MASE for delta 3
        //calcDelta3(argc, argv , originalTraceIndex, meanValue);
        // calculate MAE and MASE for delta 2
        //calcDelta2(argc, argv , originalTraceIndex);
        int missingIOsArray[argc];
        int outOrderIOsArray[argc];
        double displaceAvgArray[argc];
        int displaceStdDevArray[argc];
        int loadMAEarray[argc];
        double IATmaeArray[argc];
        calcLoadIATDelta(argc, argv, originalTraceIndex,
                         missingIOsArray,
                         outOrderIOsArray,
                         displaceAvgArray,
                         displaceStdDevArray,
                         loadMAEarray,
                         IATmaeArray);
        printDevOrig(argc, argv, stdDeviation, meanValue
                     , meanInFlight, stdDevInFlight, maxInFlight
                     , meanLatency, stdDevLatency, maxLatency
                     , missingIOsArray, outOrderIOsArray , displaceAvgArray, displaceStdDevArray, loadMAEarray, IATmaeArray
                     , originalTraceIndex);
    }
    else
    {
        printDeviations(argc, argv, stdDeviation, meanValue
                        , meanInFlight, stdDevInFlight, maxInFlight
                        , meanLatency, stdDevLatency, maxLatency);
    }

    return 0;
}
