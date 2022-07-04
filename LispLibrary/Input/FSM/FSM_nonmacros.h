#include <string>
class FSM_nonmacros
{
    enum states {
        s0,s1,s2,s3,s4,s5,s6,s7,s8
     };
public:
    enum class T_id {
        comment,str1,str2,none
     };
public:
    FSM_nonmacros() = default;

    void restart(){
        t_state =  s0;
    }
    //EOF symbol (if needed)
    bool at_end(){

        switch(t_state){
        case s1:
             {
                 t_state = s4;
                 return true;
             }
        default:
            break;
        }

        return false;
    }
    // reject/no
    bool next(unsigned char ch){

        switch(t_state){
        case s0:
            if(ch == 34) 
             {
                 t_state = s2;
                 return true;
             }
            if(ch == 59) 
             {
                 t_state = s1;
                 return true;
             }
            if(ch == 124) 
             {
                 t_state = s3;
                 return true;
             }
         break;
        case s1:
            if(ch >= 0 && ch <= 9)
             {
                 t_state = s1;
                 return true;
             }
            if(ch == 10) 
             {
                 t_state = s4;
                 return true;
             }
            if(ch >= 11 && ch <= 255)
             {
                 t_state = s1;
                 return true;
             }
             {
                 t_state = s4;
                 return true;
             }
         break;
        case s2:
            if(ch >= 0 && ch <= 33)
             {
                 t_state = s2;
                 return true;
             }
            if(ch == 34) 
             {
                 t_state = s5;
                 return true;
             }
            if(ch >= 35 && ch <= 91)
             {
                 t_state = s2;
                 return true;
             }
            if(ch == 92) 
             {
                 t_state = s6;
                 return true;
             }
            if(ch >= 93 && ch <= 255)
             {
                 t_state = s2;
                 return true;
             }
         break;
        case s3:
            if(ch >= 0 && ch <= 91)
             {
                 t_state = s3;
                 return true;
             }
            if(ch == 92) 
             {
                 t_state = s8;
                 return true;
             }
            if(ch >= 93 && ch <= 123)
             {
                 t_state = s3;
                 return true;
             }
            if(ch == 124) 
             {
                 t_state = s7;
                 return true;
             }
            if(ch >= 125 && ch <= 255)
             {
                 t_state = s3;
                 return true;
             }
         break;
        case s6:
            if(ch >= 0 && ch <= 255)
             {
                 t_state = s2;
                 return true;
             }
         break;
        case s8:
            if(ch >= 0 && ch <= 255)
             {
                 t_state = s3;
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
        case s4:
        case s5:
        case s7:
              return true;
        default:
            break;
        }

        return false;
    }
    // -1 for non T
    T_id get_T_id() const{

        switch(t_state){
        case s4:
              return T_id::comment;
        case s5:
              return T_id::str1;
        case s7:
              return T_id::str2;
        default:
            break;
        }

        return  T_id::none;
    }
    
private:
    states t_state = s0;
};