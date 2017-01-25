/**
 **     File:  fidelitychk.cc
 **    Authors:  Weiping He, Jerry Fredin
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
//
//  usage:  fidelitychk <orig_trace.csv> <replay_trace.csv> <output_file> 
//  The trace files must be in single record per IO format.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
using namespace std;

#define MAXFLDS 20
#define MAXFLDSIZE 32

#define MAXTRACE 10  /* Maximum input records */

struct inrec
	{
	 long		in_delta;
	 char		in_op;
	 unsigned long long	in_lba;
	 long		in_numblks;
	};

void parse(  char *record,char *delim, char arr[][MAXFLDSIZE],int *fldcnt)
{
    char *p=strtok(record,delim);
    int  len;
    int fld=0;

    while(p)
    {
	len = strlen(p);
	if(p[len-1] == '\n') p[len-1] = 0;
        strcpy(arr[fld],p);
//              printf("Array field %d contains: %s\n",fld,arr[fld]);
                fld++;
                p=strtok('\0',delim);
        }
        *fldcnt=fld;
return;
}

int main(int argc, char *argv[])
{
	FILE	*orig_file;
	FILE	*replay_file;
	FILE	*output_file;
	char	TraceArrayO[MAXFLDS][MAXFLDSIZE]={0x0};
	char	TraceArrayR[MAXFLDS][MAXFLDSIZE]={0x0};
	char	prevTraceArrayO[MAXFLDS][MAXFLDSIZE]={0x0};
	char	prevTraceArrayR[MAXFLDS][MAXFLDSIZE]={0x0};
	char	tmpline[1024]={0x0};
	int	print_flag=0;
	int	fldcount=0;
	int	traceindex=0;
	
	int	i;
	int	recordCount=0;
	char *strptr;
	
	long	LBAerr=0;
	long	BLKSerr=0;
	int	IATcurrO=0;
	int	IATcurrR=0;
	int	IATdelta=0;
	int	IATmax=0;
	int	IATmin=0;
	int	IATsum=0;
	int	LATdelta=0;
	int	LATmax=0;
	int	LATmin=0;
	int	LATsum=0;
	int	OIOdelta;
	int	OIOsum=0;
	int	OIOmax=0;
	
	char* comma = ",";
	char* commaNL = ",\n";

	vector<inrec> inrec; 
	struct inrec tmpInrec;
	
	if (argc > 3) print_flag = 1;
	if (argc < 3) {
		printf("Incorrect number of input parameters, must be two or three. argc=%d\n",argc);
		return(-1);
	}

	orig_file = fopen(argv[1], "r");
	if (orig_file == NULL) {
		fprintf(stderr, "Cannot open original trace file %s for reading: %s.\n", argv[1], strerror(errno));
		perror("File open error");
		exit(EXIT_FAILURE);
	}

	replay_file = fopen(argv[2], "r");
	if (replay_file == NULL) {
		fprintf(stderr, "Cannot open replay trace file %s for reading: %s.\n", argv[2], strerror(errno));
		perror("File open error");
		exit(EXIT_FAILURE);
	}

	if (print_flag) {
		output_file = fopen(argv[3], "w");
		if (output_file == NULL) {
			fprintf(stderr, "Cannot open output file %s for writing: %s.\n", argv[3], strerror(errno));
			perror("File open error");
			exit(EXIT_FAILURE);
		}
	}

	fgets(tmpline,sizeof(tmpline), orig_file); /* get rid of header */	
	fgets(tmpline,sizeof(tmpline), replay_file); /* get rid of header */	
	
	fgets(tmpline,sizeof(tmpline),orig_file);/* get next line*/
	parse(tmpline,comma,TraceArrayO,&fldcount); 
	memcpy(prevTraceArrayO,TraceArrayO, sizeof(TraceArrayO));
		
	fgets(tmpline,sizeof(tmpline),replay_file);/* get next line*/
	parse(tmpline,commaNL,TraceArrayR,&fldcount); 
	memcpy(prevTraceArrayR,TraceArrayR, sizeof(TraceArrayR));
	if (print_flag) {
		fprintf(output_file,"ELAPUS-O,ELAPUS-R,LATUS-O,LATUS-R,EDELTA-O,LDELTA-R\n");
		fprintf(output_file,"%s,%s,%s,%s,%s,%s\n",prevTraceArrayO[1],prevTraceArrayR[1],prevTraceArrayO[15],prevTraceArrayR[15],prevTraceArrayO[1],prevTraceArrayR[1]);
	}

	while(!feof(orig_file) ) {	
		recordCount++;
		//printf("Record Count = %d\n",recordCount);	
		fgets(tmpline,sizeof(tmpline),orig_file);/* get next line*/
		parse(tmpline,comma,TraceArrayO,&fldcount); 
		
		fgets(tmpline,sizeof(tmpline),replay_file);/* get next line*/
		parse(tmpline,comma,TraceArrayR,&fldcount); 

		IATcurrO = strtol(TraceArrayO[1],&strptr,10) - strtol(prevTraceArrayO[1],&strptr,10);
		IATcurrR = strtol(TraceArrayR[1],&strptr,10) - strtol(prevTraceArrayR[1],&strptr,10);
		IATdelta = IATcurrR - IATcurrO;

		LATdelta = strtol(TraceArrayR[15],&strptr,10) - strtol(TraceArrayO[15],&strptr,10);

		OIOdelta = strtol(TraceArrayR[4],&strptr,10) - strtol(TraceArrayO[4],&strptr,10);

		if (strcmp(TraceArrayO[10],TraceArrayR[10])!=0) {
			LBAerr++;
			//printf("LBA error at record %d\n",recordCount);
		}

		if (strcmp(TraceArrayO[11],TraceArrayR[11])!=0) {
			BLKSerr++;
			printf("NBLKS error at record %d\n",recordCount);
		}
	
		if (IATdelta > IATmax) IATmax = IATdelta;
		if (IATdelta < IATmin) IATmin = IATdelta;
		IATsum = IATsum + IATdelta;

		if (LATdelta > LATmax) LATmax = LATdelta;
		if (LATdelta < LATmin) LATmin = LATdelta;
		LATsum = LATsum + LATdelta;

		if (OIOdelta > OIOmax) OIOmax = OIOdelta;
		OIOsum = OIOsum + OIOdelta;

		if (print_flag) {
			fprintf(output_file,"%s,%s,%s,%s,%d,%d\n",TraceArrayO[1],TraceArrayR[1],TraceArrayO[15],TraceArrayR[15],IATdelta,LATdelta);
		}
		
		memcpy(prevTraceArrayO,TraceArrayO, sizeof(TraceArrayO));
		memcpy(prevTraceArrayR,TraceArrayR, sizeof(TraceArrayR));

// 0=Offset, 1=Eusec, 2=Eticks, 3=CMD.S, 4=InFLTIO, 5=TS, 6=SEQid, 7=SSID, 8=OP, 9=PH, 10=LBA 11=NBLK, 12=LATtks, 13=HOST, 14=HOSTLUN, 15=LATus	
				/*tmpInrec.in_delta = strtol(tempTraceArray[1],&strptr,10);
				tmpInrec.in_lba = strtol(tempTraceArray[3],&strptr,10);
			
				tmpInrec.in_numblks = strtol(tempTraceArray[4],&strptr,10);
				
				inrec.push_back(tmpInrec);
				printf("%" PRIu64 "start_lba  %ld sum_in_delta %ld sum_in_blks\n",inrec[recordcount].in_lba,inrec[recordcount].in_delta,inrec[recordcount].in_numblks);
			   
				sum_in_delta = 0;
				sum_in_blks = 0;
				start_lba = strtol(TraceArray[3],&strptr,10);
				//
				//line_counter = recordcount;	
				memcpy(tempTraceArray, TraceArray, sizeof(TraceArray));
				recordcount++;
				line_counter++;
				//line_buffer++;
			}*/
	}
	printf("Number of LBA errors:  %ld\n",LBAerr);
	printf("Number of BLK errors:  %ld\n",BLKSerr);
	printf("IAT sum:  %d\n",IATsum);
	printf("Minimum IAT delta:  %d\n",IATmin);
	printf("Maximum IAT delta:  %d\n",IATmax);
	printf("Average IAT delta:  %d.4\n",IATsum/recordCount);
	printf("Minimum LAT delta:  %d\n",LATmin);
	printf("Maximum LAT delta:  %d\n",LATmax);
	printf("Average LAT delta:  %d.4\n",LATsum/recordCount);
	printf("Maximum OIO delta:  %d\n",OIOmax);
	printf("Average OIO delta:  %d.4\n",OIOsum/recordCount);
	fclose(orig_file);
	fclose(replay_file);
	if(print_flag) fclose(output_file);
}
