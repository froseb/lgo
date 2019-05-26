#ifndef NETWORK_SIMPLEX_H
#define NETWORK_SIMPLEX_H

#include "Graph.h"
#include "ResidualEdge.h"

// Contains all simplex variables
class simplex_vars {
public:
    simplex_vars(Graph& g) : flow(g.edge_count + g.node_count, 0), prev(g.node_count+1, 0), pot(g.node_count+1, 0), dist(g.node_count+1, 0) { }

    std::vector<flow_t> flow;
    std::vector<edge_t> prev;
    std::list<edge_t> L, U;
    std::vector<cost_t> pot;
    std::vector<unsigned int> dist; // distance (numbers of edges on the path from the root to each node)
};

std::vector<flow_t> network_simplex(Graph& g);

#endif