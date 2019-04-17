#ifndef FOURIERMOTZKIN_H
#define FOURIERMOTZKIN_H

#include "LinearProgram.h"

// Contains the result of the Fourier Motzkin elimination
class FourierMotzkinResult {
public:
    FourierMotzkinResult(bool feasible, std::vector<double>& certificate) : feasible(feasible), certificate(certificate) { }
    bool feasible;
    std::vector<double> certificate;    
};

FourierMotzkinResult fourierMotzkin(LinearProgram& lp);

#endif