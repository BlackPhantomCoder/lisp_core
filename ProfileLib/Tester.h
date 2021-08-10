#pragma once
#include <stdexcept>
#include <string>
#include <future>
#include <functional>

#include "test_runner.h"

namespace NATests {
    //Тест функции
    #define NATests_RUN_TEST_catch_ex(runner, func) \
        runner.add_test(func, #func, true)
    #define NATests_RUN_TEST_nocatch_ex(runner, func) \
        runner.add_test(func, #func, false)

    struct test_result {
        std::string name;
        bool fail = false;
        std::string output;
    };

    std::ostream& operator<<(std::ostream& os, const NATests::test_result& s);

    //Класс, запускающий тесты
    class Tester {
    public:
        Tester(bool async_mode);
        template <class TestFunc>
        void add_test(TestFunc func, const std::string& test_name, bool cath_exceptions = false);
        std::vector<test_result> execute();
    private:
        bool t_async_mode;
        std::vector<std::function<test_result(void)>> t_tests;
    };


    template<class TestFunc>
    inline void Tester::add_test(TestFunc func, const std::string& test_name, bool cath_exceptions)
    {
        auto fnc = [func, test_name, cath_exceptions]() -> test_result {
            test_result result = { test_name };
            if (cath_exceptions) {
                try {
                    func();
                    result.fail = false;
                }
                catch (std::exception& e) {
                    result.fail = true;
                    result.output = e.what();
                }
                catch (...) {
                    result.fail = true;
                    result.output = "Unknown exception caught";
                }
            }
            else {
                try {
                    func();
                    result.fail = false;
                }
                catch (std::runtime_error& e){
                    result.fail = true;
                    result.output = e.what();
                }
            }
            return result;
        };
        t_tests.push_back(fnc);
    }
}

