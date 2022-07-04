#include "DotPair.h"
#include "Funcs.h"

using namespace std;
DotPair::DotPair(const Cell& f, const Cell& s):
	t_first(f),
	t_second(s)
{

}

DotPair::~DotPair()
{
	clear();
}


DotPair::DotPair(const DotPair& rh):
	t_first((rh.t_first)),
	t_second((rh.t_second))
{

}

DotPair::DotPair(DotPair&& rh)noexcept :
	t_first(move(rh.t_first)),
	t_second(move(rh.t_second))
{

}

bool DotPair::empty() const{
	return t_first.empty() && t_second.empty();
}

void DotPair::clear() {
	if (!empty()) {
		t_first.clear();
		t_second.clear();
	}
}

bool DotPair::is_list() const
{
	return true;
}

DotPair& DotPair::operator=(DotPair&& rh)noexcept {
	if (this != &rh) {
		t_first = move(rh.t_first);
		t_second = move(rh.t_second);
	}
	return *this;
}

DotPair& DotPair::operator=(const DotPair& rh) noexcept
{
	if (this != &rh) {
		t_first = (rh.t_first);
		t_second = (rh.t_second);
	}
	return *this;
}

bool is_null_list(const DotPair& lst)
{
	return lst.t_first.empty();
}


SExpr::del_func_ptr DotPair::get_del_fnc() const
{
	return &pool_delete_dp;
}
