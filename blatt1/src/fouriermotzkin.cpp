#include "LinearProgram.h"
#include "fouriermotzkin.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>

// Scalar multiplication between to double vectors
double scalarMult(std::vector<double> a, std::vector<double> b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Dimensions do not match");
    }

    double res = 0;
    for (unsigned int i=0; i<a.size(); i++) {
        res += a[i]*b[i];
    }

    return res;
}

// Fourier motzkin elimination algorithm to find a feasible solution of an LP or find a certificate that the LP is infeasible
FourierMotzkinResult fourierMotzkin(LinearProgram& lp) {

    // Trivial case, check if "0 < a" for an a < 0
    if (lp.getColCount() == 0) {
        // Check if any constraint is less than zero
        for (unsigned int i=0; i<lp.getRowCount(); i++) {
            if (lp.getConstraint(i) < 0) {
                std::vector<double> certificate(lp.getRowCount(), 0);
                certificate[i] = 1;

                return FourierMotzkinResult(false, certificate);
            }
        }
        std::vector<double> certificate;

        return FourierMotzkinResult(true, certificate);
    }

    // Create new LP
    LinearProgram newLP(lp.getColCount()-1) ;
    
    // Indices where the first coefficient is <0, =0 and >0
    std::vector<unsigned int> lt0;
    std::vector<unsigned int> eq0;
    std::vector<unsigned int> gt0;
    for (unsigned int i=0; i<lp.getRowCount(); i++) {
        if (lp.getMatValue(i, 0) < 0) {
            lt0.push_back(i);
        } else if (lp.getMatValue(i, 0) > 0) {
            gt0.push_back(i);
        } else {
            eq0.push_back(i);
        }
    }

    // Copy equations where the first coefficient is equal to 0
    for (unsigned int i : eq0) {
        std::vector<double> rowVals;
        for (int j=1; j<lp.getColCount(); j++) {
            rowVals.push_back(lp.getMatValue(i, j));
        }
        newLP.addRow(rowVals, lp.getConstraint(i));
    }
    // Construct new equations where the coeff. is < or > 0s
    for (unsigned int i : lt0) {
        for (unsigned int j : gt0) {
            std::vector<double> rowVals;
            for (unsigned int k=1; k<lp.getColCount(); k++) {
                rowVals.push_back(lp.getMatValue(j, k)/lp.getMatValue(j, 0) - lp.getMatValue(i, k)/lp.getMatValue(i, 0));
            }
            double constraint = lp.getConstraint(j)/lp.getMatValue(j, 0) - lp.getConstraint(i)/lp.getMatValue(i, 0);
            newLP.addRow(rowVals, constraint);
        }
    }

    FourierMotzkinResult res = fourierMotzkin(newLP);

    // check if the lp is feasible
    if (res.feasible) {
        std::vector<double> certificate;
        certificate.push_back(0);

        // append old certificate (already known variables)
        certificate.insert(certificate.end(), res.certificate.begin(), res.certificate.end());

        // Find a possible value for the next variable
        if (lt0.size() > 0) {
            double max = -INFINITY;
            double tmp = 0;
            for (unsigned int i : lt0) {
                if ((tmp = -scalarMult(certificate, lp.getRow(i))/lp.getMatValue(i, 0) + lp.getConstraint(i)/lp.getMatValue(i, 0)) > max) {
                    max = tmp;
                }
            }
            certificate[0] = max;
        } else if (gt0.size() > 0) {
            double min = INFINITY;
            double tmp = 0;
            for (unsigned int i : gt0) {
                if ((tmp = -scalarMult(certificate, lp.getRow(i))/lp.getMatValue(i, 0) + lp.getConstraint(i)/lp.getMatValue(i, 0)) < min) {
                    min = tmp;
                }
            }
            certificate[0] = min;
        }

        return FourierMotzkinResult(true, certificate);
    } else {
        std::vector<double> certificate(lp.getRowCount(), 0);

        // Copy scalars for rows that have been copied
        for (unsigned int i=0; i<eq0.size(); i++) {
            certificate[eq0[i]] += res.certificate[i];
        }

        for (unsigned int i=0; i<lt0.size(); i++) {
            for (unsigned int j=0; j<gt0.size(); j++) {
                certificate[lt0[i]] -= res.certificate[i*gt0.size() + j + eq0.size()] / lp.getMatValue(lt0[i], 0);
                certificate[gt0[j]] += res.certificate[i*gt0.size() + j + eq0.size()] / lp.getMatValue(gt0[j], 0);
            }
        }

        return FourierMotzkinResult(false, certificate);
    }
}