#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <functional>

using namespace std;
using namespace std::chrono;

class LogDuration {
public:
    explicit LogDuration(const string& msg = "")
        : message(msg + ": ")
        , start(steady_clock::now())
    {
    }

    ~LogDuration() {
        auto finish = steady_clock::now();
        auto dur = finish - start;
        cerr << message
            << duration_cast<milliseconds>(dur).count()
            << " ms" << endl;
    }
private:
    string message;
    steady_clock::time_point start;
};

class DurationFunc {
public:
    DurationFunc(std::function<void(std::chrono::milliseconds)> fnc)
        : start(steady_clock::now())
        , t_fnc(fnc)
    {
    }

    ~DurationFunc() {
        auto finish = steady_clock::now();
        auto dur = finish - start;
        t_fnc(duration_cast<milliseconds>(dur));
    }
private:
    steady_clock::time_point start;
    std::function<void(std::chrono::milliseconds)> t_fnc;
};


#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) \
  LogDuration UNIQ_ID(__LINE__){message};