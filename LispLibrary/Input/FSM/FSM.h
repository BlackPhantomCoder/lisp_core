#include <string>
class FSM
{
    enum states {
        s0,s1,s2,s3,s4,s5,s6
     };
public:
    enum class T_id {
        witespace,non_macros,numb_or_symbol,numb,symbol,none
     };
public:
    FSM() = default;

    void restart(){
        t_state =  s0;
    }
    //EOF symbol (if needed)
    bool at_end(){

        return false;
    }
    // reject/no
    bool next(unsigned char ch){

        switch(t_state){
        case s0:
            if(ch >= 0 && ch <= 8)
             {
                 t_state = s4;
                 return true;
             }
            if(ch >= 9 && ch <= 10)
             {
                 t_state = s6;
                 return true;
             }
            if(ch >= 11 && ch <= 12)
             {
                 t_state = s4;
                 return true;
             }
            if(ch == 13) 
             {
                 t_state = s6;
                 return true;
             }
            if(ch >= 14 && ch <= 31)
             {
                 t_state = s4;
                 return true;
             }
            if(ch == 32) 
             {
                 t_state = s6;
                 return true;
             }
            if(ch == 33) 
             {
                 t_state = s4;
                 return true;
             }
            if(ch == 34) 
             {
                 t_state = s1;
                 return true;
             }
            if(ch >= 35 && ch <= 38)
             {
                 t_state = s4;
                 return true;
             }
            if(ch >= 42 && ch <= 44)
             {
                 t_state = s4;
                 return true;
             }
            if(ch == 45) 
             {
                 t_state = s2;
                 return true;
             }
            if(ch >= 46 && ch <= 47)
             {
                 t_state = s4;
                 return true;
             }
            if(ch >= 48 && ch <= 57)
             {
                 t_state = s3;
                 return true;
             }
            if(ch == 58) 
             {
                 t_state = s4;
                 return true;
             }
            if(ch == 59) 
             {
                 t_state = s1;
                 return true;
             }
            if(ch >= 60 && ch <= 91)
             {
                 t_state = s4;
                 return true;
             }
            if(ch == 92) 
             {
                 t_state = s5;
                 return true;
             }
            if(ch >= 94 && ch <= 123)
             {
                 t_state = s4;
                 return true;
             }
            if(ch == 124) 
             {
                 t_state = s1;
                 return true;
             }
            if(ch >= 125 && ch <= 255)
             {
                 t_state = s4;
                 return true;
             }
         break;
        case s2:
            if(ch >= 48 && ch <= 57)
             {
                 t_state = s3;
                 return true;
             }
         break;
        case s5:
            if(ch >= 0 && ch <= 255)
             {
                 t_state = s4;
                 return true;
             }
         break;
        case s6:
            if(ch >= 9 && ch <= 10)
             {
                 t_state = s6;
                 return true;
             }
            if(ch == 13) 
             {
                 t_state = s6;
                 return true;
             }
            if(ch == 32) 
             {
                 t_state = s6;
                 return true;
             }
         break;
        default:
            break;
        }

        return false;
    }
    // is in Terminal
    bool is_T() const{

        switch(t_state){
        case s1:
        case s2:
        case s3:
        case s4:
        case s6:
              return true;
        default:
            break;
        }

        return false;
    }
    // -1 for non T
    T_id get_T_id() const{

        switch(t_state){
        case s1:
              return T_id::non_macros;
        case s2:
              return T_id::numb_or_symbol;
        case s3:
              return T_id::numb;
        case s4:
              return T_id::symbol;
        case s6:
              return T_id::witespace;
        default:
            break;
        }

        return  T_id::none;
    }
    
private:
    states t_state = s0;
};