#include "ParserHelper.h"
#include "CoreEnv.h"
#include "Funcs.h"
using namespace std;

ParserHelper::ParserHelper(CoreEnvironment& env):
    t_env(&env) 
{
}

void ParserHelper::set_env(CoreEnvironment& env)
{
    t_env = &env;
}

ParserHelper::sp_id ParserHelper::make_empty_sp()
{
    return t_push_resourse(sp{});
}

ParserHelper::sp_id ParserHelper::make_sp(ParserHelper::sp_id base, ParserHelper::data_id elem)
{
    t_get_sp(base).push_front(t_get_cell(elem));
    t_remove_resourse(elem);
    return base;
}

ParserHelper::data_id ParserHelper::make_list(ParserHelper::sp_id sp)
{
    auto& sp_data = t_get_sp(sp);
    auto v = data_type{ t_env->farm().make_list_cell(begin(sp_data), end(sp_data)) };
    t_remove_resourse(sp);
    return t_push_resourse(move(v));
}

ParserHelper::data_id ParserHelper::make_dp(ParserHelper::sp_id f, ParserHelper::data_id s)
{
    auto& container = t_get_sp(f);
    if (empty(container))
        throw logic_error("make_dpair: must be 2+ elems");
    auto prev_it = (prev(end(container)));
    Cell result = t_env->farm().make_list_cell(*prev_it, t_get_cell(s));

    t_remove_resourse(s);

    auto r_end = std::make_reverse_iterator(begin(container));
    auto r_beg = std::make_reverse_iterator(prev_it);

    for (; r_beg != r_end; ++r_beg) {
        result = cons(*r_beg, result, t_env->farm());
    }

    t_remove_resourse(f);

    return t_push_resourse(result);
}

bool ParserHelper::save_if_macro(char ch)
{
    if (auto opt = t_env->macrotable().is_macro_char(ch); opt) {
        t_macro = true;
        save(push_cell(t_env->farm().make_list_cell(opt->get().func)));
        return true;
    }
    return false;
}

void ParserHelper::save_symbol_or_macro(char ch)
{
    if (!save_if_macro(ch)) {
        save(make_symbol((""s + ch).c_str()));
    }
}

void ParserHelper::save(ParserHelper::data_id cell)
{
    t_saved = &t_get_cell(cell);
}

void ParserHelper::saved_to_final()
{
    t_final = true;
}

bool ParserHelper::is_saved_macro()
{
    return t_macro;
}

bool ParserHelper::is_saved_final()
{
    return t_final;
}

Cell& ParserHelper::get_saved()
{
    if (!t_saved) throw std::logic_error("ParserHelper::get_saved");
    return *t_saved;
}

ParserHelper::id_type ParserHelper::t_push_resourse(std::variant<Cell, sp>&& res)
{
    auto id = 0;
    {
        if (empty(t_ids)) {
            id = t_max_id++;
        }
        else {
            id = t_ids.back();
            t_ids.pop_back();
        }
    }
    t_data.emplace(id, move(res));
    return id;
}

void ParserHelper::t_remove_resourse(id_type id)
{
    t_data.erase(id);
    if (id == (t_max_id - 1)) {
        --t_max_id;
    }
    else
        t_ids.push_back(id);
}

Cell& ParserHelper::t_get_cell(id_type id)
{
    return get<Cell>(t_data.at(id));
}

ParserHelper::sp& ParserHelper::t_get_sp(id_type id)
{
    return get<sp>(t_data.at(id));
}


ParserHelper::data_id ParserHelper::push_cell(const Cell& cell)
{
    return t_push_resourse(cell);
}

ParserHelper::data_id ParserHelper::make_symbol(const char* str)
{
    return t_push_resourse(t_env->farm().make_symbol_cell(str));
}

void ParserHelper::output_sp(const char* str, ParserHelper::sp_id sp)
{
    //cout << str << endl;
    // for (const auto& el : t_get_sp(sp)) {
    //     cout << t_env->output_control().to_string(el) <<endl;
    // }
}

void ParserHelper::output_cell(const char* str, ParserHelper::data_id cell)
{
    //cout << str << t_env->output_control().to_string(t_get_cell(cell)) <<endl;
}

extern "C" unsigned long make_empty_sp(void* ph) {
    return ((ParserHelper*)ph)->make_empty_sp();
}

extern "C" unsigned long make_sp(void* ph, unsigned long base, unsigned long elem)
{
    return ((ParserHelper*)ph)->make_sp(base, elem);
}

extern "C" unsigned long make_list(void* ph, unsigned long sp)
{
    return ((ParserHelper*)ph)->make_list(sp);
}

extern "C" unsigned long make_dp(void* ph, unsigned long f, unsigned long s)
{
    return ((ParserHelper*)ph)->make_dp(f, s);
}

extern "C" void save(void* ph, unsigned long cell)
{
    ((ParserHelper*)ph)->save(cell);
}

extern "C" unsigned long make_symbol(void* ph, const char* str) {
    return ((ParserHelper*)ph)->make_symbol(str);
}

extern "C" void output_sp(void* ph, const char* str, unsigned long sp) {
    ((ParserHelper*)ph)->output_sp(str, sp);
}

extern "C" void output_cell(void* ph, const char* str, unsigned long cell) {
    ((ParserHelper*)ph)->output_cell(str, cell);
}

extern "C" void saved_to_final(void* ph) {
    ((ParserHelper*)ph)->saved_to_final();
}


extern "C" void save_symbol_or_macro(void* ph, char ch) {
    ((ParserHelper*)ph)->save_symbol_or_macro(ch);
}

void save_if_or_macro(void* ph, char ch, int* result)
{
    *result = ((ParserHelper*)ph)->save_if_macro(ch);
}
