/* Fourier Motzkin elimination from assignment sheet 1 */

#include "LinearProgram.h"
#include "fouriermotzkin.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// Check certificate for validity
bool checkCertificate(LinearProgram& lp, FourierMotzkinResult res, double eps) {
	if (res.feasible) {
		for (unsigned int i=0; i<lp.getRowCount(); i++) {
            double sum = scalarMult(res.certificate, lp.getRow(i));
            if (sum > lp.getConstraint(i)) {
                return false;
            }
        }
        return true;
	} else {
        // Check if the row of the matrix sum up to zero (with an error <= eps)
        for (unsigned int j=0; j<lp.getColCount(); j++) {
            double sum = 0;
            for (unsigned int i=0; i<lp.getRowCount(); i++) {
                sum += res.certificate[i] * lp.getMatValue(i, j);
            }

            if (abs(sum) > eps) {
                return false;
            }
        }

        double sum = 0;
        for (unsigned int i=0; i<lp.getRowCount(); i++) {
            sum += res.certificate[i] * lp.getConstraint(i);
        }

        if (sum < 0) {
            return true;
        } else {
            return false;
        }
    }
}

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
    } else {
        std::cout << "empty " << std::endl;
        for (double d : res.certificate) {
            std::cout << d << " ";
        }
    }

    std::cout << std::endl;

    // validity check
    if (checkCertificate(lp, res, 1e-8)) {
        std::cout << "Validity check passed" << std::endl;
    } else {
        std::cout << "Validity check failed" << std::endl;
    }

    return 0;
}