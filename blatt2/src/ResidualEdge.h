#ifndef RESIDUALEDGE_H
#define RESIDUALEDGE_H

#include "NetworkSimplex.h"

class ResidualEdge {
public:
    ResidualEdge();
    ResidualEdge(Graph& g, edge_t e, node_t node, bool tail);
    ResidualEdge(Graph& g, edge_t e, bool forward);
    flow_t pushable_flow(std::vector<flow_t>& flow);
    void push(std::vector<flow_t>& flow, flow_t value);
    cost_t potential_cost(std::vector<cost_t>& pot);

    edge_t id;
    node_t v, w;
    flow_t cap;
    cost_t cost;
    bool forward = true;
};

#endif