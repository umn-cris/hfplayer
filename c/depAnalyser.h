/* HFPlayer 3.0 Source Code
   © Regents of the University of Minnesota. 
   This software is licensed under GPL version 3.0 (https://www.gnu.org/licenses/gpl-3.0.en.html). */
#ifndef DEPAN_H
#define DEPAN_H
#include <fstream>
#include <iostream>
#include <assert.h>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include "TextDataSet.h"

struct vertexProperty
{
    string name;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, vertexProperty > Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;
typedef boost::graph_traits<Graph>::adjacency_iterator AdjacencyIterator;


struct Graph_writer
{
    void operator()(std::ostream& out) const
    {
        out << "graph [bgcolor=transparent]" << std::endl;
        out << "graph [rankdir=LR]" << std::endl;
// 		out << "node [shape=circle color=white]" << std::endl;
// 		out << "edge [style=dashed]" << std::endl;
    }
};


template <class Name>
class Vertex_Writer
{
public:
    Vertex_Writer(Name _name) : name(_name) {}
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const
    {
        out << " [label=\"" << name[v].name << "\"]";
    }
private:
    Name name;
};
#endif
