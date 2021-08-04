#include "LambdaStorage.h"
using namespace std;

void LambdaStorage::add(const Symbol& symbol, const lambda& cell)
{
    t_lambdas[symbol] = cell;
}

void LambdaStorage::add(const Symbol& symbol, lambda&& cell)
{
    t_lambdas[symbol] = move(cell);
}

std::optional<std::reference_wrapper<const lambda>> LambdaStorage::find(const Symbol& symbol)const
{
    if (auto it = t_lambdas.find(symbol); it != end(t_lambdas)) {
        return { it->second };
    }
    return nullopt;
}
