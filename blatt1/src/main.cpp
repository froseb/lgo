/* Fourier Motzkin elimination from assignment sheet 1 */

#include "LinearProgram.h"
#include "fouriermotzkin.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// main function
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

    LinearProgram lp(filename);

    FourierMotzkinResult res = fourierMotzkin(lp);

    if (res.feasible) {
        for (double d : res.certificate) {
            std::cout << d << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}