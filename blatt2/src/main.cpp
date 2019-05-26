#include <iostream>
#include "NetworkSimplex.h"

int main(int argc, char** argv) {
  std::string outputfile = "";
    std::string filename = "";
    bool filenameSpecified = false;
    bool outputfileSpecified = false;
    for (int i=1; i<argc; i++) {
        if (argv[i][0] == '-') {
            // Output file can be specified
            if (argv[i][1] == 'o') {
                if (i+1 < argc) {
                    outputfile = std::string(argv[i+1]);
                    outputfileSpecified = true;
                    i++;
                }
            }
        } else {
            filename = argv[i];
            filenameSpecified = true;
        }
    }

    if (!filenameSpecified) {
        std::cout << "Please specify your input filename." << '\n';
        return 0;
    }
    
    Graph g(filename);

    std::vector<flow_t> flow = network_simplex(g);

    std::fstream outfile;
    std::ostream *out;
    if (outputfileSpecified) {
      outfile = std::fstream(outputfile, std::ios_base::out);

      if (!outfile.is_open()) {
        throw(std::runtime_error("Output file could not be opened."));
      }

      out = &outfile;
    } else {
      out = &std::cout;
    }
    g.export_min_cost_flow(flow, *out);
}