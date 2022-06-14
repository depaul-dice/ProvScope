// Copyright (c) 2021 Yuta Nakamura. All rights reserved.
#include "ec.h"

std::ostream& operator << (std::ostream& os, const ErrorCode& ec)
{
    if(ec == ErrorCode::DIROPEN)
    {
        os << "DIROPEN";
    }
    else if(ec == ErrorCode::INVALID_VALUE)
    {
        os << "INVALID_VALUE";
    }
    else if(ec == ErrorCode::INVALID_FPTR)
    {
        os << "INVALID_FPTR";
    }
    else if(ec == ErrorCode::INVALID_INDEX)
    {
        os << "INVALID_INDEX";
    }
    else if(ec == ErrorCode::_EOF)
    {
        os << "EOF";
    } 
    else if(ec == ErrorCode::READERROR)
    {
        os << "READERROR";
    }
    else if(ec == ErrorCode::NOLINEFEED)
    {
        os << "NOLINEFEED";
    }
    else if(ec == ErrorCode::NOPATH)
    {
        os << "NOPATH";
    }
    else if(ec == ErrorCode::SUCCESS)
    {
        os << "SUCCESS";
    }
    else
    {
        os << "NA";
    }
    return os;
}
