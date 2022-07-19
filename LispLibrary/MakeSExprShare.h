#pragma once
#include "SExpr.h"
#include "CoreData.h"
#include "DotPair.h"
#include "Number.h"
#include "Symbol.h"

template<class T, class... Args>
SExprShare make_SExprShare(Args&&... args);











template<class T, class ...Args>
inline SExprShare make_SExprShare(Args && ...args)
{
	static auto& pool = CoreData::get_pn_pool<T>();
	return pool.construct(std::forward<Args&&>(args)...);
}