/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include "depAnalyser.h"
#include "writeGraphviz.h"

using namespace std;
long long unsigned int last_parent = 0; //last printed parent in the output trace file

// create a typedef for the Graph type
typedef map<double, Vertex, greater<double>> VertexSortedList;
// typedef map<double, Vertex > InFlightList;
typedef pair<double, Vertex> VSortedListPair;

void usage()
{
    cout << "Usage: dep_analyser -m <Min parent distance in Microsecond> -w <Max parent distance in Microsecond>  <iotrace_name>" << endl;
    cout << "       IO dependency information will be annotated in the <iotrace_name>.annot file" << endl;
    exit(1);
}

bool is_reachable(Graph &depGraph, size_t parent, size_t child)
{
    vector<int> distances(boost::num_vertices(depGraph), 0);
    Vertex start = boost::vertex(parent, depGraph);
    Vertex target = boost::vertex(child, depGraph);
    Graph::adjacency_iterator neighbourIt, neighbourEnd, childIt, childEnd;
    boost::tie(neighbourIt, neighbourEnd) = boost::adjacent_vertices(start, depGraph);

    for(; neighbourIt != neighbourEnd ; neighbourIt++) {
        Vertex neighbour = * neighbourIt;
        boost::tie(childIt, childEnd) = boost::adjacent_vertices(neighbour, depGraph);

        for(; childIt != childEnd ; childIt++) {
            Vertex targetCandidate = *childIt;

            if(targetCandidate == target) {
                return true;
            }
        }
    }

// 	boost::Visitor vis = boost::make_dfs_visitor(boost::record_distances(&distance[0], boost::on_tree_edge() ) );
    //boost::depth_first_search(depGraph,   boost::visitor(boost::make_dfs_visitor(boost::record_distances(&distances[0], boost::on_tree_edge()))).root_vertex( start) );
    /*

    boost::breadth_first_search(depGraph, start ,
    							boost::visitor(boost::make_bfs_visitor(boost::record_distances(&distances[0], boost::on_tree_edge()))));

    if(distances[child] != 0){
    	// it is reachable, do NOT add the edge
    // 		cout << "Cycle between "<< parent <<"->"<< child << endl;
    	return true;
    }*/
    return false;
}
void print_trace_line(GraphVectorNode v , vector<GraphVectorNode> pList, ofstream &out)
{
    // #DUMP_OFFSET.I,ABS_SECS.D,ELAPSED_USECS.D,ELAPSED_TICKS.I,CMD.S,INFLIGHT_IOS.I,TS.I,SEQID.I,LUN_SSID.I,OP.I,PHASE.I,LBA.I,NBLKS.I,LATENCY_TICKS.I,HOST_ID.I,HOST_LUN.I,LATENCY_USECS.D
    out << std::setprecision(3);
    out << std::fixed;
    out << v.dump_offset << "," << 0 <<"," << v.elapsed_usecs << "," << v.elapsed_ticks;
    out << "," << v.cmd << "," << v.inflight_ios << "," << v.ts << "," << v.seqid;
    out << "," << v.lun_ssid << "," << v.op << "," << v.phase;
    out << "," << v.lba << "," << v.nblks << "," << v.latency_ticks;
    out << "," << v.host_id << "," << v.host_lun << "," << v.latency_usecs;
    //print annotated dependency infomation
    

    if(last_parent == 0 ) {
        out << "," << pList.size() << ",";
    }
    else{
        int parentCount = 1;
        if(pList.size() != 0 ){
            parentCount = pList.size(); 
        }
        out << "," << parentCount << ",";
    }
    auto it = pList.begin();
    if(it != pList.end()) {
        while(true) {
            last_parent =  it->dump_offset; 
            out << it->dump_offset ;
            ++it;

            if(it != pList.end())
                out << "-" ;
            else
                break;
        }
    }
    else if(last_parent == 0 )
        out << "NA";
    else
        out << last_parent; 
    

    out << endl;
}
void print_trace_heading(ofstream &out)
{
    out << "#";
    out << "DUMP_OFFSET.I,ABS_SECS.D,ELAPSED_USECS.D,ELAPSED_TICKS.I,CMD.S,INFLIGHT_IOS.I,TS.I,SEQID.I,LUN_SSID.I,OP.I,PHASE.I,";
    out << "LBA.I,NBLKS.I,LATENCY_TICKS.I,HOST_ID.I,HOST_LUN.I,LATENCY_USECS.D,";
    out << "DEP_PARENT_COUNT.I,DEP_PARENT_LIST.S";
    out << endl;
}
void generate_trace(Graph depGraph, vector<GraphVectorNode> recordsVector, char *traceName)
{
    boost::graph_traits<Graph>::vertex_iterator vi, vi_end, next;
    boost::graph_traits<Graph>::adjacency_iterator ai, ai_end;
    tie(vi, vi_end) = vertices(depGraph);
    vector<GraphVectorNode> parents;
    ofstream annotatedTrace;
    annotatedTrace.open(string(traceName).append(".annot"));
    print_trace_heading(annotatedTrace);

    for(next = vi; vi != vi_end; vi = next) {
        ++next;
        parents.clear();
        tie(ai, ai_end) =  boost::adjacent_vertices(*vi, depGraph);

        while(ai != ai_end) {
            parents.push_back(recordsVector[*ai]);
            ++ai;
        }

        print_trace_line(recordsVector[*vi], parents, annotatedTrace);
    }

    annotatedTrace.close();
}

int main(int argc, char *argv[])
{
    if(argc != 6 )
        usage();
	if( strcmp(argv[3], "-w") != 0 && strcmp(argv[1],"-m") != 0)
		usage();

    double SearchTimeWindow = atof(argv[4]);
    double minDistance = atof(argv[2]);
    char *traceName = argv[5];
    DRecordData *rec;
    TextDataSet *trace = new TextDataSet();
    trace->open(traceName);
    assert(trace->start());
    VertexSortedList vSortedList;
    pair<VertexSortedList::iterator, bool> mapRet;
    Graph depGraph; //Main dependency graph
    Graph shadowGraph; //shadow dependency graph
    vector<GraphVectorNode> recordsVector; //keep track of trace records in the memory
    double currCompTime = 0;
    double currArrivTime = 0;
    Vertex lastCompletedVertex = boost::graph_traits<Graph>::null_vertex();
    unsigned maxQueueDepth = 0 ;

    // main loop to detect dependency and build graph, read one line (request) at a time
    while((rec = trace->next())) {
        // create and add new vertex in the dependency graph
        GraphVectorNode currRecord(rec);
        size_t currVertexIndexinVector = recordsVector.size();
        recordsVector.push_back(currRecord);
        Vertex currVertex = boost::add_vertex(depGraph);
        depGraph[currVertex].name = to_string(currRecord.dump_offset);
        
        // add vertex into shadowGraph
        Vertex currShadowVertex = boost::add_vertex(shadowGraph);
        
        
        currArrivTime = (*rec)["elapsed_usecs"].d ;
        //Clean-up search space and throw old vertex away
        auto mapRItr = vSortedList.rbegin(); // read last Vestex in the search space which has smallest completion time

        while(mapRItr != vSortedList.rend()) {
            if( (mapRItr->first + SearchTimeWindow < currArrivTime ) //delete all search space 
                && ( vSortedList.size() > 1)  // make sure at least one (potential) parent left in this list
                ) {
                vSortedList.erase(mapRItr->first);
                mapRItr = vSortedList.rbegin(); //nasti replacement for mapRItr++
            }
            else
                break;
        }

        // establish edges for the added vertex
        auto mapItr = vSortedList.begin(); // read first inFlight Vertex with largest completion time
        unsigned inEdges = 0;

        //record max queuedepth observer in the trace till now.
        if(currRecord.inflight_ios  > maxQueueDepth)
            maxQueueDepth = currRecord.inflight_ios ;
        for(; mapItr != vSortedList.end(); mapItr++) {
            if(mapItr->first + minDistance < currArrivTime) { //completion time dependecy condition check
                lastCompletedVertex =  mapItr->second;

                //add dependency edge if currVertex is not reachable from lastCompletedVertex
                if(! is_reachable(shadowGraph, currShadowVertex, lastCompletedVertex)) {
                    boost::add_edge(currVertex, lastCompletedVertex, depGraph);
                    inEdges++;
                }

                boost::add_edge(currShadowVertex, lastCompletedVertex, shadowGraph);

                if(inEdges == maxQueueDepth + 1)
 					break;// we have find all possible parents.
//                 if(inEdges == maxQueueDepth + 2)
//                     cerr<<"Logic dos not work"<<endl; 
            }
        }

        // insert current vertex in the inflight requests list
        currCompTime = currArrivTime + (*rec)["latency_usecs"].d ;
        mapRet = vSortedList.insert(VSortedListPair(currCompTime , currVertexIndexinVector));
        assert(mapRet.second);    // check no duplicated node with the same completion time is there
		trace->showProgress();
    }
    printf("\nAnalyser is done\n");

    generate_trace(depGraph, recordsVector, traceName);
    /*
    	Graph_writer graph_writer;
    	Vertex_Writer<Graph> vertex_writer(depGraph);


    	ofstream outf("net.gv");

    	boost::write_graphviz(outf, depGraph, vertex_writer, boost::default_writer(), graph_writer );
    	outf.close();
    */
    /*FILE * dotFile = fopen("net.gv", "r" );

    static GVC_t *gvc;
    // set up a graphviz context - but only once even for multiple graphs
    if (!gvc)
    	gvc = gvContext();

    Agraph_t *graphViz;
    graphViz = agopen("Dependency Graph", AGDIGRAPH);
    graphViz = agread(dotFile);
    fclose(dotFile);
    gvLayout(gvc, graphViz, "dot");
    // Output in .dot format ( or "png" for .png, etc)
    string outFileName(argv[1]);
    outFileName.append(".png");
    FILE *outPng = fopen( outFileName.c_str() , "w");
    gvRender(gvc, graphViz, "png", outPng);
    fclose(outPng);
    gvFreeLayout(gvc, graphViz);
    agclose(graphViz);*/
}
