#ifndef NETWORK_SIMPLEX_H
#define NETWORK_SIMPLEX_H

#include "Graph.h"
#include "ResidualEdge.h"

// Contains all simplex variables
typedef struct simplex_vars {
    std::vector<flow_t> flow;
    std::vector<edge_t> prev;
    std::list<edge_t> L, U;
    std::vector<cost_t> pot;
    std::vector<unsigned int> dist; // distance (numbers of edges on the path from the root to each node)
} simplex_vars;

std::vector<edge_t> network_simplex(Graph& g);

#endif