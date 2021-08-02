#pragma once
#include <stdexcept>
#include <string>
#include <future>

#include "test_runner.h"

namespace NATests {
    //Тест функции
    #define NATests_RUN_TEST(runner, func) \
        runner.add_test(func, #func)

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
        void add_test(TestFunc func, const std::string& test_name);
        std::vector<test_result> execute();
    private:
        bool t_async_mode;
        std::vector<std::future<test_result>> t_async_tests;
        std::vector<test_result> t_non_async_tests;
    };


    template<class TestFunc>
    inline void Tester::add_test(TestFunc func, const std::string& test_name)
    {
        auto fnc = [func, test_name]() -> test_result {
            test_result result = { test_name };
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
            return result;
        };
        if (t_async_mode) {
            t_async_tests.push_back(std::async(fnc));
        }
        else {
            t_non_async_tests.push_back(fnc());
        }
    }

}

