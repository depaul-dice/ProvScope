// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

#ifndef MATRIX_H
#define MATRIX_H

#include <cstdlib>
#include <iostream>

// this has int inside
class Matrix
{
public: 
    // Big4
    Matrix() = default;
    Matrix(const Matrix&);
    Matrix& operator = (const Matrix&);
    ~Matrix(); // you need to delete elements

    // all elements are set to zero at first
    Matrix(unsigned length, unsigned width);
    
    // rows and columns are from 0s and NOT 1s
    // returns -1 for an error
    float getElement(unsigned row, unsigned col);
    
    // returns non-zero in case of error
    int setElement(unsigned row, unsigned col, float val);

    unsigned getWidth(void);
    unsigned getLength(void);

    friend std::ostream& operator << (std::ostream&, const Matrix&);

private:
    unsigned length;
    unsigned width;
    float *elements;
};

#endif /* MATRIX_H */
