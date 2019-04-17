#include "LinearProgram.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// Constructs a linear program object by the content of an input file
LinearProgram::LinearProgram(std::string input_file) {
    std::ifstream file(input_file);

    file >> rows;
    file >> cols;

    double tmp;
    for (unsigned int i=0; i<cols; i++) {
        file >> tmp;
        objectiveFunction.push_back(tmp);
    }
    for (unsigned int i=0; i<rows; i++) {
        file >> tmp;
        constraints.push_back(tmp);
    }
    for (unsigned int i=0; i<rows; i++) {
        matrix.push_back(std::vector<double>());
        for (unsigned int j=0; j<cols; j++) {
            file >> tmp;
            matrix[i].push_back(tmp);
        }
    }
}

// Gets the amount of rows
unsigned int LinearProgram::getRowCount() {
    return rows;
}

// Gets the amount of cols
unsigned int LinearProgram::getColCount() {
    return cols;
}

// Gets the value of the matrix in the `row`th row and `col`th column
double LinearProgram::getMatValue(unsigned int row, unsigned int col) {
    if (row >= getRowCount() || col >= getColCount()) {
        throw std::invalid_argument("Index out of bounds");
    }

    return matrix[row][col];
}

// Gets the `index`th coefficient of the objective function
double LinearProgram::getObjectiveValue(unsigned int index) {
    if (index >= cols) {
        throw std::invalid_argument("Index out of bounds");
    }

    return objectiveFunction[index];
}

// Gets the `index`th constraint
double LinearProgram::getConstraint(unsigned int index) {
    if (index >= rows) {
        throw std::invalid_argument("Index out of bounds");
    }

    return constraints[index];
}

// Adds a new side condition to the LP
void LinearProgram::addRow(std::vector<double>& rowVals, double constraint) {
    if (rowVals.size() != cols) {
        throw std::invalid_argument("Size of new row does not equal the width of the matrix");
    }

    matrix.push_back(rowVals);
    constraints.push_back(constraint);
    rows++;
}