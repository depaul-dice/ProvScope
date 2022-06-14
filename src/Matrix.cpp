// Copyright (c) 2021 Yuta Nakamura. All rights reserved.
#include "Matrix.h"

// width is horizontal, length is vertical

Matrix::Matrix(unsigned length, unsigned width)
    :length(length), width(width), elements(nullptr)
{
    elements = (float *)calloc(length * width, sizeof(unsigned));
}

Matrix::Matrix(const Matrix& tmp)
    :length(tmp.length), width(tmp.width), elements(nullptr)
{
    elements = (float *)calloc(length * width, sizeof(unsigned));
}

Matrix& Matrix::operator = (const Matrix& tmp)
{
    length = tmp.length;
    width = tmp.width;

    // this is realloc because you want to change the size of the allocations
    elements = (float *)realloc(elements, sizeof(unsigned) * (length * width));
    for(unsigned i = 0; i < length * width; i++)
    {
        elements[i] = tmp.elements[i];
    }
    return *this; 
}

Matrix::~Matrix(void)
{
    free(elements);
}

float Matrix::getElement(unsigned row, unsigned col)
{
    if(row * width + col >= length * width)
        return -1;
    return elements[row * width + col];
}

int Matrix::setElement(unsigned row, unsigned col, float val)
{
    if(row * width + col >= length * width)
        return 1;
    elements[row * width + col] = val;
    return 0;
}

unsigned Matrix::getWidth()
{
    return width;
}

unsigned Matrix::getLength()
{
    return length;
}
std::ostream& operator << (std::ostream& os, const Matrix& m)
{
    for(unsigned i = 0; i < m.length; i++)
    {
        os << '|';
        for(unsigned j = 0; j < m.width; j++)
        {
            os << m.elements[i * m.width + j] << "\t|"; 
        }
        os << std::endl;
    }
    return os;
}
