#include "ResidualEdge.h"

ResidualEdge::ResidualEdge() { }

// Creates a residual edge from the edge that has node as tail if tail == true or node as head otherwise
ResidualEdge::ResidualEdge(Graph& g, edge_t e, node_t node, bool tail) : id(e), cap(g.edges[e].cap), cost(g.edges[e].cost) {
    if (tail) {
        if (g.edges[e].v == node) {
            v = node;
            w =  g.edges[e].w;
            forward = true;
        } else {
            v = node;
            w = g.edges[e].v;
            forward = false;
        }
    } else {
        if (g.edges[e].w == node) {
            v = g.edges[e].v;
            w = node;
            forward = true;
        } else {
            v = g.edges[e].w;
            w = node;
            forward = false;
        }
    }
}

// Creates a residual edge from the edge that has tail as tail
ResidualEdge::ResidualEdge(Graph& g, edge_t e, bool forward) : id(e), cap(g.edges[e].cap), cost(g.edges[e].cost), forward(forward) {
    if (forward) {
        v = g.edges[e].v;
        w = g.edges[e].w;
    } else {
        v = g.edges[e].w;
        w = g.edges[e].v;
    }
}

// Gets the amount of flow that can be pushed through the residual edge
flow_t ResidualEdge::pushable_flow(std::vector<flow_t>& flow) {
    return forward ? cap - flow[id] : flow[id];
}

// pushes a certain amount of flow through the edge
void ResidualEdge::push(std::vector<flow_t>& flow, flow_t value) {
    if (value > pushable_flow(flow)) {
        throw(std::runtime_error("Pushing too much flow through edge id " + id));
    }

    if (forward) {
        flow[id] += value;
    } else {
        flow[id] -= value;
    }
}

cost_t ResidualEdge::potential_cost(std::vector<cost_t>& pot) {
    if (forward) { 
        return cost + pot[v] - pot[w];
    } else {
        return -cost + pot[v] - pot[w];
    }
}

cost_t ResidualEdge::residual_cost() {
    return forward ? cost : -cost;
}