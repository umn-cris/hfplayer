/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
#ifndef WRITEGRAPH_H
#define WRITEGRAPH_H
#include "depAnalyser.h"


class GraphVectorNode
{
public:
    unsigned long long dump_offset;
    double elapsed_usecs;
    unsigned long long elapsed_ticks;
    string cmd;
    unsigned short inflight_ios;
    unsigned long long ts;
    unsigned seqid;
    unsigned short lun_ssid;
    unsigned short op;
    unsigned short phase;
    unsigned long long lba;
    unsigned nblks;
    unsigned short host_id;
    unsigned short host_lun;
    double latency_usecs;
    unsigned long long latency_ticks;
// #DUMP_OFFSET.I,ELAPSED_USECS.D,ELAPSED_TICKS.I,CMD.S,INFLIGHT_IOS.I,TS.I,SEQID.I,LUN_SSID.I,OP.I,PHASE.I,LBA.I,NBLKS.I,LATENCY_TICKS.I,HOST_ID.I,HOST_LUN.I,LATENCY_USECS.D
    GraphVectorNode() {
        dump_offset = 0;
        elapsed_usecs = 0;
        elapsed_ticks = 0;
        inflight_ios = 0;
        seqid = 0;
        ts = 0;
        lun_ssid = 0;
        op = 0;
        phase = 0;
        lba = 0;
        nblks = 0;
        host_id = 0;
        host_lun = 0;
        latency_usecs = 0;
        latency_ticks = 0;
    }

    GraphVectorNode(DRecordData *rec) {
        dump_offset = (*rec)["dump_offset"].i;
        elapsed_usecs = (*rec)["elapsed_usecs"].d;
        elapsed_ticks = (*rec)["elapsed_ticks"].i;
        cmd = *((string *)((*rec)["cmd"].s));
        inflight_ios = (*rec)["inflight_ios"].i;
        ts = (*rec)["ts"].i;
        seqid = (*rec)["seqid"].i;
        lun_ssid = (*rec)["lun_ssid"].i;
        op = (*rec)["op"].i;
        phase = (*rec)["phase"].i;
        lba = (*rec)["lba"].i;
        nblks = (*rec)["nblks"].i;
        host_id = (*rec)["host_id"].i;
        host_lun = (*rec)["host_lun"].i;
        latency_usecs = (*rec)["latency_usecs"].d;
        latency_ticks = (*rec)["latency_ticks"].i;
    }

    bool operator==(const GraphVectorNode &other) const {
        if(this->seqid == other.seqid)
            return true;
        else
            return false;
    }
};
void writeGraphviz(std::vector<GraphVectorNode>& recordsVector, Graph &depGraph);
/*
bool GraphVectorNode::operator==(const GraphVectorNode& other) const
{
	if( this->seqid == other.seqid)
		return true;
	else
		return false;
}
*/
#endif
