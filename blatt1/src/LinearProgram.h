#ifndef LINEARPROGRAM_H
#define LINEARPROGRAM_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// A class that describes a linear program
class LinearProgram {
public:
    LinearProgram(std::string input_file);
    LinearProgram(unsigned int cols) : cols(cols) { }
    unsigned int getRowCount();
    unsigned int getColCount();
    double getMatValue(unsigned int row, unsigned int col);
    double getObjectiveValue(unsigned int index);
    double getConstraint(unsigned int index);
    void addRow(std::vector<double>& rowVals, double constraint);
    std::vector<double>& getRow(unsigned int index);

private:
    unsigned int rows = 0;
    unsigned int cols = 0;
    std::vector<std::vector<double>> matrix;
    std::vector<double> constraints;
    std::vector<double> objectiveFunction;
};

#endif