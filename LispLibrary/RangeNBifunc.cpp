#include "RangeNBifunc.h"

RangeNBiFunc::RangeNBiFunc(func_id id, CarCdrIterator beg_it, CarCdrIterator end_it) :
	Func(id, true),
	t_args({ beg_it , end_it })
{
}

CarCdrIterator RangeNBiFunc::args_beg()
{
	return t_args.first;
}

CarCdrIterator RangeNBiFunc::args_end()
{
	return t_args.second;
}

CarCdrConstIterator RangeNBiFunc::args_beg() const
{
	return t_args.first;
}

CarCdrConstIterator RangeNBiFunc::args_end() const
{
	return t_args.second;
}