/* Implementation of the network simplex algorithm */
#include "NetworkSimplex.h"

typedef Graph::Edge Edge;

void make_strongly_feasible_instance(Graph& g, simplex_vars& vars) {
    // Find max edge cost
    cost_t max_cost = abs(g.edges[0].cost);
    for (edge_t e = 0; e < g.edge_count; e++) {
        if (g.edges[e].cost > max_cost) {
            max_cost = abs(g.edges[e].cost);
        }
    }


    // Add all edges from the graph to L
    for (edge_t i=0; i<g.edge_count; i++) {
        vars.L.push_back(i);
    }

    for (node_t i=0; i<g.node_count-1; i++) {
        // node i is a sink
        if (g.supply[i] < 0) {
            g.edges.push_back(Edge(g.node_count, i, -g.supply[i], 1+g.node_count*max_cost));
            vars.flow[g.edge_count+i] = -g.supply[i];

        } else { // node i is not
            g.edges.push_back(Edge(i, g.node_count, g.supply[i]+1, 1+g.node_count*max_cost));
            vars.flow[g.edge_count+i] = g.supply[i];
        }
        vars.pot[i] = 1+g.node_count*max_cost;
        vars.dist[i] = 1;
        vars.prev[i] = g.edge_count+i;
    }
}

// Finds a new edge to be added to the basis and returns whether a better edge has been found in L or U
bool find_new_edge(ResidualEdge& new_edge, Graph& g, simplex_vars& vars) {
    // look for better basis edge in L
    for (auto i = vars.L.begin(); i != vars.L.end(); i++) {
        ResidualEdge res_e = ResidualEdge(g, *i, true);
        if (res_e.potential_cost(vars.pot) < 0) {
            new_edge = res_e;
            vars.L.erase(i); // Edge will be added to basis
            return true;
        }
    }

    // look for better basis edge in U
    for (auto i = vars.U.begin(); i != vars.U.end(); i++) {
        ResidualEdge res_e = ResidualEdge(g, *i, false);
        if (res_e.potential_cost(vars.pot) < 0) {
            new_edge = res_e;
            vars.U.erase(i); // Edge will be added to basis
            return true;
        }
    }

    return false;
}

flow_t get_augmentable_flow_on_w_v_path(node_t v, node_t w, Graph& g, simplex_vars& vars) {
    cost_t pushable_flow;
    cost_t other_augmentable_flow;
    ResidualEdge resid_e;
    if (vars.dist[v] < vars.dist[w]) {
        resid_e = ResidualEdge(g, vars.prev[w], w, true);
        pushable_flow = resid_e.pushable_flow(vars.flow);
        if (v == resid_e.w) {
            return pushable_flow;
        } else {
            other_augmentable_flow = get_augmentable_flow_on_w_v_path(v, resid_e.w, g, vars);
            return pushable_flow < other_augmentable_flow ? pushable_flow : other_augmentable_flow;
        }
    } else {
        resid_e = ResidualEdge(g, vars.prev[v], v, false);
        pushable_flow = resid_e.pushable_flow(vars.flow);
        if (resid_e.v == w) {
            return pushable_flow;
        } else {
            other_augmentable_flow = get_augmentable_flow_on_w_v_path(resid_e.v, w, g, vars);
            return pushable_flow < other_augmentable_flow ? pushable_flow : other_augmentable_flow;
        }
    }
}

// Gets the flow that can be augmented through the fundamental circuit containing the edge from v to w
flow_t get_augmentable_flow_on_fundamental_circuit(ResidualEdge& e, Graph& g, simplex_vars& vars) {
    flow_t augmentable_flow_on_w_v_path = get_augmentable_flow_on_w_v_path(e.v, e.w, g, vars);
    return e.pushable_flow(vars.flow) < augmentable_flow_on_w_v_path ? e.pushable_flow(vars.flow) : augmentable_flow_on_w_v_path;
}

// augments the flow along the fundamental circuit and returns the last edge that limits the augmented value
edge_t augment_flow_and_get_limiting_edge(ResidualEdge& e, flow_t value, Graph& g, simplex_vars& vars) {
    node_t v = e.v;
    node_t w = e.w;

    bool found_limiting_edge = false;
    edge_t limiting_edge;

    // Check if the edge e itself is a limiting edge
    if (e.pushable_flow(vars.flow) == value) {
        found_limiting_edge = true;
        limiting_edge = e.id;
    }
    e.push(vars.flow, value);

    ResidualEdge resid_e;
    
    while (v != w) {
        if (vars.dist[v] < vars.dist[w]) {
            resid_e = ResidualEdge(g, vars.prev[w], w, true);
            // Change last limiting edge if resid_e is limiting
            if (resid_e.pushable_flow(vars.flow) == value) {
                found_limiting_edge = true;
                limiting_edge = resid_e.id;
            }
            resid_e.push(vars.flow, value);
            w = resid_e.w;
        } else {
            resid_e = ResidualEdge(g, vars.prev[v], v, false);
            // Change last limiting edge if resid_e is limiting and no last edge is already found
            if (!found_limiting_edge && resid_e.pushable_flow(vars.flow) == value) {
                found_limiting_edge = true;
                limiting_edge = resid_e.id;
            }
            resid_e.push(vars.flow, value);
        }
    }

    return limiting_edge;
}


// Returns a flow vector that solves the min cost flow problem on the given graph
std::vector<edge_t> network_simplex(Graph& g) {
    simplex_vars vars;

    // contains the flow for each edge
    vars.flow = std::vector<flow_t>(g.edge_count + g.node_count-1, 0);

    // contains previous edges in the tree for each node
    vars.prev = std::vector<edge_t>(g.node_count+1, 0);

    // potential for each node
    vars.pot = std::vector<cost_t>(g.node_count+1, 0);

    // distance for each node
    vars.dist = std::vector<unsigned int>(g.node_count+1, 0);

    // add edges for having a strongly feasible start and set initial values
    make_strongly_feasible_instance(g, vars);
    
    ResidualEdge new_edge;
    while(find_new_edge(new_edge, g, vars)) {
        flow_t augmentable_flow = get_augmentable_flow_on_fundamental_circuit(new_edge, g, vars);

    }

    return vars.flow;
}