#include "RangeNBifunc.h"












RangeNBiFunc::RangeNBiFunc(CoreEnvironment& env, CarCdrIterator beg_it, CarCdrIterator end_it) :
	Func(env),
	t_args({ beg_it , end_it })
{
}


CarCdrIterator RangeNBiFunc::t_args_beg()
{
	return t_args.first;
}


CarCdrIterator RangeNBiFunc::t_args_end()
{
	return t_args.second;
}


CarCdrConstIterator RangeNBiFunc::t_args_beg() const
{
	return t_args.first;
}


CarCdrConstIterator RangeNBiFunc::t_args_end() const
{
	return t_args.second;
}


bool RangeNBiFunc::t_eval_args()
{
	return true;
}


void RangeNBiFunc::t_internal_execute()
{
	return t_execute_func();
}