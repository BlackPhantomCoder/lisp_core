#include "FSM_Num.h"

bool is_digit(char ch, int base)
{
    if(ch >= 'A' && ch <= 'Z'){
        return (ch - 'A' + 10) < base;
    }
    else if(ch >= '0' && ch <= '9'){
        return ch - '0' < base;
    }
    return false;
}

FSM_numb::FSM_numb(std::function<int(void)> read_base_val) : t_read_base(read_base_val) {}

bool FSM_numb::at_end() const
{
    return false;
}

void FSM_numb::restart(){
    t_state = states::start;
}

bool FSM_numb::next(char ch){
    switch (t_state) {
    case states::start:
        if(ch == '-')
        {
            t_state = states::start;
            return true;
        }
        if(is_digit(ch, t_read_base()))
        {
            t_state = states::z_numb;
            return true;
        }
        break;
    case states::z_numb:
        if(is_digit(ch, t_read_base()))
        {
            t_state = states::z_numb;
            return true;
        }
        if(ch == '.')
        {
            t_state = states::r_numb;
            return true;
        }
        break;
    case states::r_numb:
        if(is_digit(ch, t_read_base()))
        {
            t_state = states::r_numb;
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

bool FSM_numb::is_T() const{
    switch(t_state){
    case states::z_numb:
    case states::r_numb:
        return true;
    default:
        break;
    }
    return false;
}

FSM_numb::type FSM_numb::get_T_type() const{
    switch(t_state){
    case states::z_numb:
        return type::z_numb;
    case states::r_numb:
        return type::r_numb;
    default:
        break;
    }
    return type::none;
}
