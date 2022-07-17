#pragma once
#include "Func.h"

#define simple_rangenbifunc(name, id)\
default_func(name)\
name(CarCdrIterator beg_it, CarCdrIterator end_it) : RangeNBiFunc(id, beg_it, end_it) {}

//������� before_args_eval � after_args_eval �� ��������
class RangeNBiFunc : public Func {
public:
	RangeNBiFunc() = default;
	RangeNBiFunc(func_id id, CarCdrIterator beg_it, CarCdrIterator end_it);

	CarCdrIterator args_beg() ;
	CarCdrIterator args_end() ;

	CarCdrConstIterator args_beg() const;
	CarCdrConstIterator args_end() const;
private:
	std::pair<CarCdrIterator, CarCdrIterator> t_args;
};

