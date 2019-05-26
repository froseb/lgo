#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <list>

// Define types for ids and values
typedef unsigned int node_t;
typedef unsigned int edge_t;
typedef int flow_t;
typedef int supply_t;
typedef int cost_t;

class Graph {
public:
    class Edge {
    public:
        Edge(node_t v, node_t w, flow_t cap, cost_t cost) : v(v), w(w), cap(cap), cost(cost) { };
        node_t v, w;
        flow_t cap;
        cost_t cost;
    };

    Graph(std::string filename);

    node_t node_count = 0;
    edge_t edge_count = 0;
    std::vector<Edge> edges;
    std::vector<supply_t> supply;
};

#endif