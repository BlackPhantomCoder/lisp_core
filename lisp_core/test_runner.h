#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <set>
#include <map>
#include <vector>

#define FandL "In " + std::string(__FILE__) + ", line " + std::to_string(__LINE__) + ": "


    //Тест функции
#define RUN_TEST(runner, func) \
        runner.RunTest(func, #func)


    //Сравнение значений
#define ASSERT_EQUAL(a,b) \
        AssertEqual(a,b, FandL)


    //Сравнение с истиной
#define ASSERT(a) \
        Assert(a, FandL)


    //Функция, сравнивающая значение
template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {});

    //Функция, сравнивающая значение с истиной
void Assert(bool b, const std::string& hint);


    //Класс, запускающий тесты
class TestRunner {
public:
    template <class TestFunc>
    void RunTest(TestFunc func, const std::string& test_name);
    ~TestRunner();
private:
    int fail_count = 0;
};










template<class T, class U>
inline void AssertEqual(const T& t, const U& u, const std::string& hint)
{
    if (t != u) {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if (!hint.empty()) {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

template<class TestFunc>
inline void TestRunner::RunTest(TestFunc func, const std::string& test_name)
{
    try {
        func();
        std::cerr << test_name << " OK" << std::endl;
    }
    catch (std::exception& e) {
        ++fail_count;
        std::cerr << test_name << " fail: " << e.what() << std::endl;
    }
    catch (...) {
        ++fail_count;
        std::cerr << "Unknown exception caught" << std::endl;
    }
}

template<class T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& s)
{
	os << "{";
	bool first = true;
	for (const auto& x : s) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}

template<class T>
inline std::ostream& operator<<(std::ostream& os, const std::set<T>& s)
{
	os << "{";
	bool first = true;
	for (const auto& x : s) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << x;
	}
	return os << "}";
}

template<class K, class V>
inline std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m)
{
	os << "{";
	bool first = true;
	for (const auto& kv : m) {
		if (!first) {
			os << ", ";
		}
		first = false;
		os << kv.first << ": " << kv.second;
	}
	return os << "}";
}