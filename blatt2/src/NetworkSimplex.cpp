/* Implementation of the network simplex algorithm */
#include "NetworkSimplex.h"

typedef Graph::Edge Edge;

void recursive_potential_and_distance_update(node_t node, std::vector<bool>& updated, Graph& g, simplex_vars& vars) {
    if (!updated[node]) {
        ResidualEdge prev_edge_resid = ResidualEdge(g, vars.prev[node], node, false);
        node_t previous_node = prev_edge_resid.v;
        recursive_potential_and_distance_update(previous_node, updated, g, vars);
        vars.dist[node] = vars.dist[previous_node] + 1;
        vars.pot[node] = vars.pot[previous_node] + prev_edge_resid.residual_cost();
        updated[node] = true;
    }
}

void update_potential_and_distance(Graph& g, simplex_vars& vars) {
    std::vector<bool> updated(g.node_count+1, false);
    updated[g.node_count] = true;

    for (node_t i = 0; i < g.node_count; i++) {
        recursive_potential_and_distance_update(i, updated, g, vars);
    }
}

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

    for (node_t i=0; i<g.node_count; i++) {
        // node i is a sink
        if (g.supply[i] < 0) {
            g.edges.push_back(Edge(g.node_count, i, -g.supply[i], 1+g.node_count*max_cost));
            vars.flow[g.edge_count+i] = -g.supply[i];

        } else { // node i is not
            g.edges.push_back(Edge(i, g.node_count, g.supply[i]+1, 1+g.node_count*max_cost));
            vars.flow[g.edge_count+i] = g.supply[i];
        }
        vars.prev[i] = g.edge_count+i;
    }
    update_potential_and_distance(g, vars);
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

// Gets the flow that can be augmented through the fundamental circuit containing the new edge
void get_augmentable_flow_on_fundamental_circuit(flow_t& augmentable_flow, ResidualEdge& last_limiting_edge, bool& before_new_edge, ResidualEdge& new_edge, Graph& g, simplex_vars& vars) {
    node_t v = new_edge.v;
    node_t w = new_edge.w;

    last_limiting_edge = new_edge;
    before_new_edge = false;
    augmentable_flow = new_edge.pushable_flow(vars.flow);

    ResidualEdge resid_e;
    while (v != w) {
        if (vars.dist[v] < vars.dist[w]) {
            resid_e = ResidualEdge(g, vars.prev[w], w, true);
            if (resid_e.pushable_flow(vars.flow) <= augmentable_flow) {
                last_limiting_edge = resid_e;
                before_new_edge = false;
                augmentable_flow = resid_e.pushable_flow(vars.flow);
            }
            w = resid_e.w;
        } else {
            resid_e = ResidualEdge(g, vars.prev[v], v, false);
            if (resid_e.pushable_flow(vars.flow) < augmentable_flow) {
                last_limiting_edge = resid_e;
                before_new_edge = true;
                augmentable_flow = resid_e.pushable_flow(vars.flow);
            }
            v = resid_e.v;
        }
    }
}

// augments the flow along the fundamental circuit and returns the last edge that limits the augmented value
void augment_flow_and_update_previous_edges(ResidualEdge& e, flow_t value, ResidualEdge& last_limiting_edge, bool before_new_edge, Graph& g, simplex_vars& vars) {
    node_t v = e.v;
    node_t w = e.w;

    bool invert_previous_before_new_edge = before_new_edge;
    bool invert_previous_after_new_edge = !before_new_edge;

    e.push(vars.flow, value);

    ResidualEdge resid_e;

    edge_t new_prev_after_new_edge = e.id;
    edge_t new_prev_before_new_edge = e.id;
    
    while (v != w) {
        if (vars.dist[v] < vars.dist[w]) {
            if (new_prev_after_new_edge == last_limiting_edge.id) {
                invert_previous_after_new_edge = false;
            }
            resid_e = ResidualEdge(g, vars.prev[w], w, true);
            if (invert_previous_after_new_edge) {
                vars.prev[w] = new_prev_after_new_edge;
                new_prev_after_new_edge = resid_e.id;
            }
            resid_e.push(vars.flow, value);
            w = resid_e.w;
        } else {
            if (new_prev_before_new_edge == last_limiting_edge.id) {
                invert_previous_before_new_edge = false;
            }
            resid_e = ResidualEdge(g, vars.prev[v], v, false);
            if (invert_previous_before_new_edge) {
                vars.prev[v] = new_prev_before_new_edge;
                new_prev_before_new_edge = resid_e.id;
            }
            resid_e.push(vars.flow, value);
            v = resid_e.v;
        }
    }
}


// Returns a flow vector that solves the min cost flow problem on the given graph
std::vector<flow_t> network_simplex(Graph& g) {
    simplex_vars vars(g);

    make_strongly_feasible_instance(g, vars);
    
    ResidualEdge new_edge;

    while(find_new_edge(new_edge, g, vars)) {
        flow_t augmentable_flow;
        ResidualEdge last_limiting_edge;
        bool before_new_edge;
        get_augmentable_flow_on_fundamental_circuit(augmentable_flow, last_limiting_edge, before_new_edge, new_edge, g, vars);
        augment_flow_and_update_previous_edges(new_edge, augmentable_flow, last_limiting_edge, before_new_edge, g, vars);

        // Add the edge that has been removed from the tree to L or U, respectively
        if (last_limiting_edge.forward) {
            vars.U.push_back(last_limiting_edge.id);
        } else {
            vars.L.push_back(last_limiting_edge.id);
        }

        update_potential_and_distance(g, vars);
    }

    for (node_t i=0; i<g.node_count; i++) {
        g.edges.pop_back();
        if (vars.flow.back() > 0) {
            throw(std::runtime_error("Did not find feasible solution."));
        }
        vars.flow.pop_back();
    }

    return vars.flow;
}