#include "Graph.h"

// Loads v graph from the file given wy filename
Graph::Graph(std::string filename) {
    std::fstream file(filename, std::ios_base::in);

    // Check if file is open
    if (!file.is_open()) {
        throw(std::runtime_error("File could not be opened."));
        return;
    }

    file >> node_count;
    node_t v, w;
    flow_t cap;
    cost_t cost;

    // Reserve memory
    supply.reserve(node_count);
    // Write supply values to supply vector
    for (node_t i=0; i<node_count; i++) {
        file >> supply[i];
    }

    // read edge count
    file >> edge_count;

    // add edges until no more left
    while (file >> v >> w >> cap >> cost) {
        edges.push_back(Edge(v, w, cap, cost));
    }
}

void Graph::export_min_cost_flow(std::vector<flow_t>& flow, std::ostream& out) {
    cost_t value = 0;
    for (edge_t i=0; i<edge_count; i++) {
      value += edges[i].cost * flow[i];
    }

    out << value << std::endl;

    for (edge_t i=0; i<edge_count; i++) {
        if (flow[i] > 0) {
            out << i << " " << flow[i] << std::endl;
        }
    }
}