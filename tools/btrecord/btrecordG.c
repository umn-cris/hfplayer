/***************************************************
 * Objective: small utility program for converting
 *            WLC trace into btreplay trace.
 * Author: Weiping He
 * Date:2013-9-24
 * *************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "btrecord.h"

//TODO list
//1. command line option for BUNCH_SIZE and BUNCH_INTERVAL
//2. last bunch may not be full

/**************************************************
 *             Macros
 **************************************************/
#define BUNCH_SIZE 8;
#define BUNCH_INTERVAL 50000//ns

/**************************************************
 *             User Delarations
 **************************************************/
struct bunch{
    unsigned long long genesis;
    int n_req;
};

struct request{
    int op;
    unsigned long long lba, size, ts;
};

int bunch_is_done(struct bunch cur, unsigned long long t, int bsize, int btime);
void parse_line(char* line, struct request *r);



/**************************************************
 *             Main()
 **************************************************/
int main(int argc, char **argv)
{
    //TODO:parse command line options
    if(argc < 5){
        fprintf(stderr, "usage:%s <input file> <output file> <bunch size> <bunch time>\n", argv[0]);
        exit(-1);
    }
    int bunch_size, bunch_time;
    char ifilename[50], ofilename[50], tfilename[50];
    strcpy(ifilename, argv[1]);
    strcpy(ofilename, argv[2]);
    strcpy(tfilename, ofilename);
    strcat(tfilename, ".txt");
    bunch_size = atoi(argv[3]);
    bunch_time = atoi(argv[4]);

    FILE * fp = fopen(ifilename, "r");
    FILE * outf = fopen(tfilename,"w+");
    if( fp == NULL || outf == NULL )
    {
        printf("cannot open input or output file\n");
        exit(100);
    }

    struct io_file_hdr myfheader;
    myfheader.version   = 65536;
    myfheader.genesis   = 0;
    myfheader.nbunches  = 0;
    myfheader.total_pkts= 0;

    char line[2000];         //line buffer
    struct request *buf, *treq;  //each bunch can have at most BUNCH_SIZE request
    buf = (struct request *)malloc(bunch_size * sizeof(struct request));
    struct bunch cur;       //current bunch (with metadata)
    cur.genesis = 0;        //assume first bunch starts with time 0
    cur.n_req = 0;          //bunch size is 0 in the beginning for the very 1st bunch

    struct request rholder; //request holder: shelter for each request line

    //--------------------------------- PART I ------------------------------------------
    //first time iterate through input trace file

    fgets(line, sizeof(line), fp);//skip header line
    while( fgets(line, sizeof(line), fp) != NULL ){
        //fputs(line, stdout);
        parse_line(line, &rholder);

        //check if current bunch is full
        //  yes: write header+requests, process current request(1st request of new bunch)
        //  no:  add io to current bunch buffer
        if(bunch_is_done(cur, rholder.ts, bunch_size, bunch_time)){
            fprintf(outf, "------------------\n%7llu.%09llu %3llu\n------------------\n",
                                        (unsigned long long)cur.genesis / (1000 * 1000 * 1000 ),
                                        (unsigned long long)cur.genesis % (1000 * 1000 * 1000 ), 
                                        (unsigned long long)cur.n_req);
            int i;
            for(i = 0; i<cur.n_req; ++i){
                //output each request to replay trace
                fprintf(outf, "\t%1d %10llu\t%10llu\n", buf[i].op, 
                                        (unsigned long long)buf[i].lba,
                                        (unsigned long long)buf[i].size);
            }

            myfheader.nbunches++;
            myfheader.total_pkts += cur.n_req;

            //process current request
            cur.genesis = rholder.ts;
            cur.n_req = 1;

            treq = buf;
            treq->ts   = rholder.ts;
            treq->op   = rholder.op;
            treq->lba  = rholder.lba;
            treq->size = rholder.size;
        }
        else{
            treq = buf + cur.n_req;
            treq->ts   = rholder.ts;
            treq->op   = rholder.op;
            treq->lba  = rholder.lba;
            treq->size = rholder.size;
            cur.n_req ++;
        }
    }
    
    fclose(fp);
    fclose(outf);
    
    //---------------------------- PART II ----------------------------------------------
    FILE * outf2 = fopen(ofilename,"wb+");
    fp = fopen(ifilename, "r");

    //2.1 write trace file header
    if (fwrite(&myfheader, sizeof(myfheader), 1, outf2) != 1) {
        fprintf(stderr,"trace file header output error!\n");
    }

    //2.2 second time iteration on input trace file
    struct io_bunch     mybunch;
    struct io_pkt       *mypkt;
    //struct io_bunch_hdr mybheader;
    //int mysize;     //counter for #io in a bunch

    int isFirst=1;
    fgets(line, sizeof(line), fp);//skip header line
    while( fgets(line, sizeof(line), fp) != NULL ){
        parse_line(line, &rholder);
        if(bunch_is_done(cur, rholder.ts, bunch_size, bunch_time)){
            if(isFirst==0){
                //write bunch header
                if (fwrite(&(mybunch.hdr), sizeof(struct io_bunch_hdr), 1, outf2) != 1) {
                    fprintf(stderr,"bunch header output error!\n");
                }
                //write packets
                if (fwrite(mybunch.pkts, sizeof(struct io_pkt), mybunch.hdr.npkts, outf2) != 1) {
                    fprintf(stderr,"packet output error!\n");
                    fprintf(stderr, "This event occurs for the bunch with genesis time %llu\n", mybunch.hdr.time_stamp);
                }
            }
            else{
                isFirst=0;
            }
            //process current request (i.e., 1st io of next bunch)
            mybunch.hdr.npkts = 1;
            mybunch.hdr.time_stamp = rholder.ts;
            mypkt = mybunch.pkts;
            mypkt->rw = rholder.op;
            mypkt->sector = rholder.lba;
            mypkt->nbytes = rholder.size * 512;
        }
        else{
            mypkt = mybunch.pkts + mybunch.hdr.npkts;
            mypkt->rw = rholder.op;
            mypkt->sector = rholder.lba;
            mypkt->nbytes = rholder.size * 512;
            mybunch.hdr.npkts++;
        }
    }

    fclose(fp);
    fclose(outf2);

    return 0;
}

/**************************************************
 *             User Definitions
 **************************************************/
void parse_line(char* record, struct request *r){
    //example:
    //7296, 61.537, 0.0, 3079744,WR_IN,70,1778295561105480,1138590907,3,1,0,1,0,0,356352,1024,0,0,0,31558.526,1736,0.868,0

    //4124, 9.707, 20708, RD_IN, 1, 2782945036762592, 8618686, 6, 0, 0,  2,   2, 272412, 6, 6, 127.693
    //%d    %d.%d  %d     %s     %d       %llu         %llu   %d %d %d %llu %llu
    char *pch;
    int big,small,op;
    unsigned long long lba, size;
    int i = 1;
    pch = strtok (record,",");
    while(pch != NULL)
    {
        //printf ("%s,",pch);
        pch = strtok (NULL, ",");
        if(i== 2)
            sscanf(pch,"%d.%d",&big,&small);
        else if(i==9){
            sscanf(pch,"%d",&op);
	    if ( op == 0 )
		op = 1 ;
	    else if ( op == 1 )
		op = 0 ; 
	    else {
		printf("Operation code %d is not supported" , op ); 
		continue;
	    }
	}
        else if(i==14)
            sscanf(pch,"%llu",&lba);
        else if(i==15)
            sscanf(pch,"%llu",&size);

        i++;
    }

    r->ts = (unsigned long long)(big) * 1000 + small ; 
    r->op = op;
    r->lba = lba;
    r->size = size;
    //printf("%llu\t%d\t%llu\t%llu\n", r->ts, r->op, r->lba, r->size);
}

int bunch_is_done(struct bunch cur, unsigned long long t, int bsize, int btime){
    if(cur.n_req >= bsize) //condition 1 BUNCH_SIZE
        return 1;
    else if(t - cur.genesis >= btime)//condition 2 BUNCH_TIME
        return 1;
    else
        return 0;
}
