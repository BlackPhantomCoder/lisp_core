#include "Input.h"

#include "CoreEnv.h"
#include "CoreData.h"
#include "Funcs.h"
#include "parser.h"
using namespace std;


Input::Input(CoreEnvironment& env):
    t_env(env),
    //t_scanner(env),
    //t_parser(t_scanner),
    t_read_upcase(env.farm().make_symbol_cell(CoreData::read_up_case_str)),
    t_read_base(env.farm().make_symbol_cell(CoreData::read_base_str))
{

}

bool Input::alive() {
    if (!t_cis)
        throw logic_error("Input: empty cis");
    //cout << "alive" << t_cis->get().alive() << endl;
    return t_cis->get().alive();
}

int Input::read_char(bool flag) {
    if (!t_cis)
        throw logic_error("Input: empty cis");

    if (t_ratom_read_buf) {
        t_cis->get().get_char();
        t_ratom_read_buf = false;
    }
    if (!t_cis->get().alive()) 
        throw errors::eos{};
    if (flag) {
        t_ratom_read_buf = true;

        //auto ch = t_cis->get().peek_char();
        //cout << "read-char + flag: " << char(ch) << "[" << ch << "]" << endl;
        //return ch;

        return t_cis->get().peek_char();
    }
    else {
        //auto ch = t_cis->get().get_char();
        //cout << "read-char: " << char(ch) << "[" << ch << "]" << endl;
        //return ch;

        return t_cis->get().get_char();
    }
}

int Input::peek_char() {
    if (!t_cis)
        throw logic_error("Input: empty cis");
    if (!t_cis->get().alive())
        throw errors::eos{};
    
    //auto ch = t_cis->get().peek_char();
    //cout << "peek-char: " << char(ch) << "[" << ch << "]" << endl;
    //return ch;

    return t_cis->get().peek_char();
}

void Input::unread_char() {
    if (!t_cis)
        throw logic_error("Input: empty cis");

    t_cis->get().unread_last();

    //cout << "unread-char" << endl;
}

bool Input::read_upcase_val()
{
    auto c = t_env.envs().get(to_symbol(t_read_upcase));
    return (c.empty() || !is_null(c));
}

int Input::read_base_val()
{
    auto c = t_env.envs().get(to_symbol(t_read_base));
    // нужно ли?
    if (!is_number(c) || !to_number(c).is_real() || to_integer(to_number(c)) < 2 || to_integer(to_number(c)) > 36) {
        return 10;
    }
    return to_integer(to_number(c)).to_int();
}

void Input::change_cis(CoreInputStreamInt& new_cis)
{
    t_cis = new_cis;
    t_ratom_read_buf = false;
}