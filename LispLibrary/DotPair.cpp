#include "DotPair.h"
#include "Funcs.h"

using namespace std;

DotPair::DotPair(SExprsFarm& farm):
	t_symbols(&farm)
{
}

DotPair::DotPair(const Cell& f, const Cell& s, SExprsFarm& farm) :
	t_first(f),
	t_second(s),
	t_symbols(&farm)
{

}

DotPair::DotPair():
	t_symbols(nullptr)
{

}

DotPair::~DotPair()
{
	clear();
}

DotPair::DotPair(DotPair&& rh)noexcept :
	t_first(move(rh.t_first)),
	t_second(move(rh.t_second)),
	t_symbols(rh.t_symbols)
{
	rh.t_symbols = nullptr;
}

bool DotPair::empty() const{
	return t_first.empty() && t_second.empty();
}

void DotPair::clear() {
	if (!empty()) {
		t_first.clear();
		t_second.clear();
		t_symbols = nullptr;
	}
}

DotPair& DotPair::operator=(DotPair&& rh)noexcept {
	if (this != &rh) {
		t_first = move(rh.t_first);
		t_second = move(rh.t_second);
		t_symbols = rh.t_symbols;
		rh.t_symbols = nullptr;
	}
	return *this;
}

bool is_null(const DotPair& lst)
{
	return lst.t_symbols && lst.t_first.empty();
}