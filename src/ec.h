// Copyright (c) 2021 Yuta Nakamura. All rights reserved.

#ifndef EC_H
#define EC_H
#include <iostream>

enum class ErrorCode
{
    DIROPEN,
    INVALID_VALUE,
    INVALID_FPTR,
    INVALID_TYPE,
    INVALID_INDEX,
    _EOF,
    READERROR,
    NOLINEFEED,
    NOPATH,
    SUCCESS
};

std::ostream& operator << (std::ostream& os, const ErrorCode&);
#endif
