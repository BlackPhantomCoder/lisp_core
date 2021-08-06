#include "DotPair.h"
#include "Funcs.h"
using namespace std;

bool DotPair::empty() const
{
	return !t_core;
}

DotPair::DotPair(SymbolsProvider& provider):
	t_symbols(provider)
{
}

DotPair::DotPair(DotPairCore&& dpc, SymbolsProvider& provider) :
	t_symbols(provider)
{
	t_core = make_shared<DotPairCore>(move(dpc));
}

DotPair::DotPair(const DotPairCore& dpc, SymbolsProvider& provider) :
	t_symbols(provider)
{
	t_core = make_shared<DotPairCore>(dpc);
}
