#include "ArgsCounter.h"

long presize_count(CarCdrConstIterator beg, CarCdrConstIterator end, long val) {
    long size = 0;
    while (size <= val && beg != end) {
        ++beg;
        ++size;
    }
    return size;
}

bool ArgsCounter::operator==(long val) const
{
    return presize_count(begin, end, val) == val;
}

bool ArgsCounter::operator>=(long val) const
{
    return presize_count(begin, end, val) >= val;
}

bool ArgsCounter::operator<=(long val) const
{
    return  presize_count(begin, end, val) <= val;
}

bool ArgsCounter::operator>(long val) const
{
    return presize_count(begin, end, val) > val;
}

bool ArgsCounter::operator<(long val) const
{
    return  presize_count(begin, end, val) < val;
}