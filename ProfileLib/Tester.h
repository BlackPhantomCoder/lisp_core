#pragma once
#include <stdexcept>
#include <string>
#include <thread>
#include <future>
#include <functional>

#include "test_runner.h"
#include "profile.h"


namespace NATests {
    //Тест функции
#define NATests_RUN_TEST_catch_ex(runner, func) \
        runner.add_test<true>(func, #func)
#define NATests_RUN_TEST_nocatch_ex(runner, func) \
        runner.add_test<false>(func, #func)

    struct test_result {
        std::string name;
        bool fail = false;
        std::string output;
    };

    std::ostream& operator<<(std::ostream& os, const NATests::test_result& s);

    //число процессоров в системе
    inline const auto processor_count = std::thread::hardware_concurrency();

    //Класс, запускающий тесты
    template<bool async_mode>
    class Tester {
    public:
        Tester() = default;
        template <bool cath_exceptions, class TestFunc>
        void add_test(TestFunc func, const std::string& test_name);
        template<unsigned threads = 1>
        std::vector<test_result> execute();
        size_t count() const;
    private:
        std::vector<std::function<test_result(void)>> t_tests;
    };











    template<bool async_mode>
    template <bool cath_exceptions, class TestFunc>
    inline void Tester<async_mode>::add_test(TestFunc func, const std::string& test_name)
    {
        if constexpr (!async_mode) {
            test_result result = { test_name };
            result.fail = true;
            try {
                //LogDuration d;
                func();
                result.fail = false;
            }
            catch (std::exception& e) {
                result.output = e.what();
            }
            catch (...) {
                if constexpr (cath_exceptions) {
                    result.output = "Unknown exception caught";
                }
                else {
                    throw;
                }
            }
            auto fnc = [result]() -> test_result {
                return result;
            };
            t_tests.push_back(fnc);
        }
        else {
            auto fnc = [func, test_name]() -> test_result {
                test_result result = { test_name };
                result.fail = true;
                try {
                    //LogDuration d;
                    func();
                    result.fail = false;
                }
                catch (std::exception& e) {
                    result.output = e.what();
                }
                catch (...) {
                    if constexpr (cath_exceptions) {
                        result.output = "Unknown exception caught";
                    }
                    else {
                        throw;
                    }
                }
                return result;
            };
            t_tests.push_back(fnc);
        }
    }

    template<bool async_mode>
    template<unsigned threads>
    std::vector<NATests::test_result> Tester<async_mode>::execute()
    {
        std::vector<NATests::test_result> result;
        result.reserve(t_tests.size());
        if constexpr (async_mode && threads > 1) {
            std::vector<future<std::vector<test_result>>> asyncs;
            asyncs.reserve(processor_count);
           
            const auto block_size = t_tests.size() / (threads);
            auto it = std::begin(t_tests);
            for (; it < (std::end(t_tests) - 2 * block_size); it += block_size) {
                asyncs.emplace_back(std::async(std::launch::async,
                    [block_size](auto it) -> auto {
                        //LogDuration d{};
                        auto result = std::vector<test_result>();
                        result.reserve(block_size);
                        for (auto i = 0; i != block_size; ++i, it++) {
                            //LogDuration d{};
                            result.push_back((*it)());
                        }
                        return result;
                    },
                    it
                        ));
            }

            {
                //LogDuration d{"1_"};
                for (; it != std::end(t_tests); ++it) {
                    //LogDuration d{"1"};
                    result.push_back((*it)());
                }
            }
            for (auto& f : asyncs) {
                auto pre_res = f.get();
                copy(move_iterator(std::begin(pre_res)), move_iterator(std::end(pre_res)), back_inserter(result));
            }
        }
        else {
            for (const auto& f : t_tests) {
                //LogDuration d{};
                result.push_back(f());
            }
        }
        t_tests.clear();
        return result;
    }

    template<bool async_mode>
    size_t Tester<async_mode>::count() const {
        return t_tests.size();
    }
}