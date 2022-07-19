#include "RangeNBifunc.h"

RangeNBiFunc::RangeNBiFunc(func_id id, CarCdrIterator beg_it, CarCdrIterator end_it) :
	Func(id, true),
	t_args_beg(beg_it),
	t_args_end(end_it)
{
}

CarCdrIterator RangeNBiFunc::args_beg()
{
	return t_args_beg;
}

CarCdrIterator RangeNBiFunc::args_end()
{
	return t_args_end;
}

CarCdrConstIterator RangeNBiFunc::args_beg() const
{
	return t_args_beg;
}

CarCdrConstIterator RangeNBiFunc::args_end() const
{
	return t_args_end;
}