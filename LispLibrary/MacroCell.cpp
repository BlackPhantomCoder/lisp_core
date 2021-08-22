#include "MacroCell.h"
#include "Funcs.h"

using namespace std;

struct link_list_frame {
    CarCdrConstIterator b;
    CarCdrConstIterator e;
    std::vector<bool> link;
    bool last = false;
    bool sublist = false;
    size_t size = 0;
};

constexpr bool car_ = true;
constexpr bool cdr_ = false;

Cell link_params(const macro& m, CarCdrConstIterator b, CarCdrConstIterator e, SExprsFarm& farm)
{
    vector<Cell> result;
    for (auto& p : m.params) {
        if (b == e) {
            result.push_back(farm.nil());
        }
        else {
            auto it = b;
            for (auto i = rbegin(p.second); i != rend(p.second); ++i) {
                if (it == e) break;
                if (*i == car_) {
                    const auto& buf = *it;
                    it = begin(buf);
                }
                else {
                    ++it;
                }
            }
            result.push_back((it == e) ? farm.nil() :  it.get_elem());
        }
    }
    return farm.make_list_cell(begin(result), end(result));
}

Cell link_params(const macro& m, const Cell& param_list, SExprsFarm& farm)
{
    return link_params(m, begin(param_list), end(param_list), farm);
}

macro make_macro(lambda_args_types arg_type, const Cell& params, const Cell& body, SExprsFarm& farm)
{
    macro result;
    auto cbody = tree_copy(body, farm);
    if (arg_type == lambda_args_types::spread) {

        auto& lst = result.params;
        std::deque<link_list_frame> stack;
        stack.push_back({ begin(params), end(params) });
        while (!empty(stack)) {
            auto& frame = stack.back();
            auto& e = frame.e;
            auto& link = frame.link;
            auto& it = frame.b;

            if (frame.sublist && !frame.last) {
                frame.sublist = false;

                for (auto it = begin(lst) + frame.size; it != end(lst); ++it) {
                    it->second.push_back(car_);
                    copy(begin(link), end(link), back_inserter(it->second));
                }
                link.push_back(cdr_);
            }

            for (; next(it) != frame.e; ++it) {
                if (is_list(*it)) {
                    frame.sublist = true;
                    frame.size = lst.size();
                    const auto& buf = *it;
                    stack.push_back({ begin(buf), end(buf), {} });
                    ++it;
                    break;
                }
                else {
                    lst.push_back({ *it, link });
                    lst.back().second.insert(begin(lst.back().second), car_);
                }
                link.push_back(cdr_);
            }
            if (frame.sublist) continue;

            if (!frame.last) {
                if (is_list(*it)) {
                    frame.last = true;
                    frame.size = lst.size();
                    const auto& buf = *it;
                    stack.push_back({ begin(buf), end(buf), {} });
                    continue;
                }
                else {
                    lst.push_back({ *it, link });
                    lst.back().second.insert(begin(lst.back().second), car_);
                }
                link.push_back(cdr_);
            }
            else {
                for (auto it = begin(lst) + frame.size; it != end(lst); ++it) {
                    it->second.push_back(car_);
                    copy(begin(link), end(link), back_inserter(it->second));
                }
            }

            const auto& cd = cdr(it.get_elem());
            if (!is_null(cd)) {
                lst.push_back({ cd, link });
            }
            stack.pop_back();
        }

        std::vector<Cell> lambda_params;

        for (auto& [p, link] : lst) {
            lambda_params.push_back(p);
        }
        result.body = make_lambda(lambda_types::nlambda, lambda_args_types::spread, farm.make_list_cell(begin(lambda_params), end(lambda_params)), cbody, farm);
    }
    else {
        result.body = make_lambda(lambda_types::nlambda, lambda_args_types::spread, farm.make_list_cell(params), cbody, farm);
        result.params = { {params, {}} };
    }
    

    return result;
}

std::vector<Cell> params_w_link(const std::vector<std::pair<Cell, std::vector<bool>>>& params, SExprsFarm& farm) {
    std::vector<Cell> result;
    for (auto& [param, link] : params) {
        std::vector <Cell> p;
        for (const auto& v : link) {
            if (v == car_) {
                p.push_back(farm.make_symbol_cell("CAR"));
            }
            else {
                p.push_back(farm.make_symbol_cell("CDR"));
            }
        }
        Cell buf = farm.make_list_cell({ farm.make_symbol_cell("CDR"), farm.make_symbol_cell("BODY") });
        for (auto it = rbegin(p); it != rend(p); ++it) {
            buf = farm.make_list_cell({ *it, buf });
        }

        result.push_back(buf);
    }

    return result;
}

Cell gen_cell(const macro& l, SExprsFarm& farm)
{
    std::vector<Cell> lst;
    lst.reserve(3);

    lst.push_back(farm.macro_symbol());
    lst.push_back(farm.make_symbol_cell("BODY"));
    auto body = params_w_link(l.params, farm);
    body.insert(begin(body), gen_cell(l.body, farm));
    rplaca(to_list(body[0]), farm.lambda_symbol());
    lst.push_back(farm.make_list_cell(begin(body), end(body)));
        
    return farm.make_list_cell(begin(lst), end(lst));
}
