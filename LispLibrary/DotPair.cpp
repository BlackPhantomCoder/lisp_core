#include "DotPair.h"
#include "Funcs.h"

using namespace std;

DotPair::DotPair(SExprsFarm& farm):
	t_farm(&farm)
{
}

DotPair::DotPair(const Cell& f, const Cell& s, SExprsFarm& farm) :
	t_first(f),
	t_second(s),
	t_farm(&farm)
{

}

DotPair::DotPair():
	t_farm(nullptr)
{

}

DotPair::~DotPair()
{
	clear();
}

DotPair::DotPair(DotPair&& rh)noexcept :
	t_first(move(rh.t_first)),
	t_second(move(rh.t_second)),
	t_farm(rh.t_farm)
{
	rh.t_farm = nullptr;
}

bool DotPair::empty() const{
	return t_first.empty() && t_second.empty();
}

void DotPair::clear() {
	if (!empty()) {
		t_first.clear();
		t_second.clear();
		t_farm = nullptr;
	}
}

DotPair& DotPair::operator=(DotPair&& rh)noexcept {
	if (this != &rh) {
		t_first = move(rh.t_first);
		t_second = move(rh.t_second);
		t_farm = rh.t_farm;
		rh.t_farm = nullptr;
	}
	return *this;
}

bool is_null_list(const DotPair& lst)
{
	return lst.t_farm && lst.t_first.empty();
}