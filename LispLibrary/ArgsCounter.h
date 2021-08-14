#pragma once
#include "CarCdrIterator.h"
struct ArgsCounter
{
public:
    bool operator==(long val) const;
    bool operator>=(long val) const;
    bool operator<=(long val) const;
    bool operator>(long val) const;
    bool operator<(long val) const;

    CarCdrConstIterator begin;
    CarCdrConstIterator end;
};

