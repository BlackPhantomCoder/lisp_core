#include <string>
#include <functional>
class FSM_numb{
public:
    enum class type {
        z_numb, r_numb, none
    };
private:
    enum class states {
        start,z_numb,r_numb
    };
public:
    FSM_numb(std::function<int(void)> read_base_val);

    //EOF symbol (if needed)
    bool at_end()const;

    void restart();
    // reject/no
    bool next(char ch);
    // is in Terminal
    bool is_T() const;
    // возвращает чем является объект
    // none == не терминальная вершина
    type get_T_type() const;
private:
    states t_state = states::start;
    std::function<int(void)> t_read_base;
};
