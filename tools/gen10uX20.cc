/**
 **     File:  gen10uX20.cc
 **    Authors:  Jerry Fredin
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
//  usage:  replay <mapfile> <tracefile> 
//

//#include <cstring>
//#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include <fcntl.h>

#define MBUFSIZ 1024 * 4096
#define READOP 1
#define WRITEOP 2
#define NOOP 0
#define MAXFLDS 11
#define MAXFLDSIZE 32
#define HOST 9
#define LUN 0

int main(int argc, char *argv[])
{
	FILE	*tracefile;
	int	dumpoffset = 4096;
	int	eusecs = 30000000;
	int	eticks = eusecs *2133333333;
	int	intervalusec = 10;
	char	cmdr[] = "RD";
	char	cmdw[] = "WR";
	int	inflt = 0;
	int	ts = eticks;
	int	seqid = 5000;
	int	ssid = 6;
	int	op = 0;
	int	phase = 0;
	int	lba = 0;
	int	nblks = 2;
	int	latticks = 200*2133;
	int	hostid = 7;
	int	hostlun = 6;
	long	latusecs = 200;
	
	int	totallines = 100000;
	int	lines;

	tracefile = fopen(argv[1], "w");
	if (tracefile == NULL) {
		fprintf(stderr, "Cannot open trace file %s for reading: %s.\n", argv[1], strerror(errno));
		perror("File open error");
		exit(EXIT_FAILURE);
	}

	// First put the header line in the output file

	fprintf(tracefile,"#DUMP_OFFSET.I,ELAPSED_USECS.D,ELAPSED_TICKS.I,CMD.S,INFLIGHT_IOS.I,TS.I,SEQID.I,LUN_SSID.I,OP.I,PHASE.I,LBA.I,NBLKS.I,LATENCY_TICKS.I,HOST_ID.I,HOST_LUN.I,LATENCY_USECS.D\n");
	
	//The first 1/4 will be writes increasing lba
	op=0;
//	lines=totallines/4;
	lines=25;
	while (lines > 0)
	{
	dumpoffset = dumpoffset + 28;
	eusecs = eusecs + intervalusec;
	eticks = eticks + (2133*intervalusec);
	ts = ts + (2133*intervalusec);
	seqid++;
	lba = lba + 1;
	lines--;
		// Now put the current line values in the output file
		if (op = 1) 
		{
		fprintf(tracefile,"%d,%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n",dumpoffset,eusecs,eticks,cmdw,inflt,ts,seqid,ssid,op,phase,lba,nblks,latticks,hostid,hostlun,latusecs);
		}
		else
		{
		fprintf(tracefile,"%d,%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n",dumpoffset,eusecs,eticks,cmdr,inflt,ts,seqid,ssid,op,phase,lba,nblks,latticks,hostid,hostlun,latusecs);
		}
	}
fclose(tracefile);
exit(0);	
	//The second 1/4 will be reads decreasing lba
	op=1;
	lines=totallines/4;
	while (lines > 0)
	{
	dumpoffset = dumpoffset + 28;
	eusecs = eusecs + intervalusec;
	eticks = eticks + (2133*intervalusec);
	ts = ts + (2133*intervalusec);
	seqid++;
	lba = lba - 1;
	lines--;
		// Now put the current line values in the output file
		if (op = 1) 
		{
		fprintf(tracefile,"%d,%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n",dumpoffset,eusecs,eticks,cmdw,inflt,ts,seqid,ssid,op,phase,lba,nblks,latticks,hostid,hostlun,latusecs);
		}
		else
		{
		fprintf(tracefile,"%d,%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n",dumpoffset,eusecs,eticks,cmdr,inflt,ts,seqid,ssid,op,phase,lba,nblks,latticks,hostid,hostlun,latusecs);
		}
	}
		
	//The third 1/4 will be reads increasing lba
	op=1;
	lines=totallines/4;
	while (lines > 0)
	{
	dumpoffset = dumpoffset + 28;
	eusecs = eusecs + intervalusec;
	eticks = eticks + (2133*intervalusec);
	ts = ts + (2133*intervalusec);
	seqid++;
	lba = lba + 1;
	lines--;
		// Now put the current line values in the output file
		if (op = 1) 
		{
		fprintf(tracefile,"%d,%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n",dumpoffset,eusecs,eticks,cmdw,inflt,ts,seqid,ssid,op,phase,lba,nblks,latticks,hostid,hostlun,latusecs);
		}
		else
		{
		fprintf(tracefile,"%d,%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n",dumpoffset,eusecs,eticks,cmdr,inflt,ts,seqid,ssid,op,phase,lba,nblks,latticks,hostid,hostlun,latusecs);
		}
	}
		
	//The fourth 1/4 will be writes decreasing lba
	op=0;
	lines=totallines/4;
	while (lines > 0)
	{
	dumpoffset = dumpoffset + 28;
	eusecs = eusecs + intervalusec;
	eticks = eticks + (2133*intervalusec);
	ts = ts + (2133*intervalusec);
	seqid++;
	lba = lba - 1;
	lines--;
		// Now put the current line values in the output file
		if (op = 1) 
		{
		fprintf(tracefile,"%d,%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n",dumpoffset,eusecs,eticks,cmdw,inflt,ts,seqid,ssid,op,phase,lba,nblks,latticks,hostid,hostlun,latusecs);
		}
		else
		{
		fprintf(tracefile,"%d,%ld,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%ld\n",dumpoffset,eusecs,eticks,cmdr,inflt,ts,seqid,ssid,op,phase,lba,nblks,latticks,hostid,hostlun,latusecs);
		}
	}
exit(0);
}	
