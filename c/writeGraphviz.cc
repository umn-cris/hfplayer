/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
#include <stdlib.h>
#include "writeGraphviz.h"

using namespace std;

#define NO_LAYOUT_OR_RENDERING

void writeGraphviz(vector<GraphVectorNode>& recordsVector, Graph &depGraph)
{
#ifdef NO_LAYOUT_OR_RENDERING
    aginit();
#else
    static GVC_t *gvc;

    /* set up a graphviz context - but only once even for multiple graphs */
    if(!gvc)
        gvc = gvContext();

#endif
    GraphVectorNode vecNode;
    Agraph_t *graphViz;
    graphViz = agopen("Dependency Graph", AGDIGRAPH);
    /* initialize graph general attributes*/
    agattr(graphViz,  "rankdir" , "LR");
    VertexIterator currIt, endIt;
    //Traverse the whole graph and visit each vertex one by one
    tie(currIt, endIt) = vertices(depGraph);

    for(; currIt != endIt; ++currIt) {
        vecNode =  recordsVector[*currIt];
// 		currArrivTime = (*rec)["elapsed_usecs"].d ;
        int offset = vecNode.dump_offset;
        string offset_str = to_string(offset);
        char *offset_cstr = new char [offset_str.length() + 1] ;
        offset_cstr = (char *) offset_str.c_str();
        agnode(graphViz, offset_cstr);
        //
    }

    Agedge_t *e;
    Agnode_t *n;
    Agnode_t *m;
    Agsym_t *a;
    /* Create a simple digraph */
    n = agnode(graphViz, "n");
    m = agnode(graphViz, "m");
    e = agedge(graphViz, n, m);

    /* Set an attribute - in this case one that affects the visible rendering */

    //     agset(g, "rankdir","LR");
    if(!(a = agfindattr(graphViz->proto->n, "color")))
        a = agnodeattr(graphViz, "color", "");

    agxset(n, a->index, "red");
#ifdef  NO_LAYOUT_OR_RENDERING
    /* Just write the graph without layout */
    agwrite(graphViz, stdout);
#else
    /* Use the directed graph layout engine */
    gvLayout(gvc, graphViz, "dot");
    /* Output in .dot format ( or "png" for .png, etc) */
    gvRender(gvc, graphViz, "dot", stdout);
    gvFreeLayout(gvc, graphViz);
#endif
    agclose(graphViz);
}
