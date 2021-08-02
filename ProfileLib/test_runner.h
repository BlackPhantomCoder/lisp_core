#pragma once

#include "Outputs.h"
#include <sstream>
#include <stdexcept>
#include <string>

#define FandL "In " + std::string(__FILE__) + ", line " + std::to_string(__LINE__) + ": "


    //���� �������
#define RUN_TEST(runner, func) \
        runner.RunTest(func, #func)


    //��������� ��������
#define ASSERT_EQUAL(a,b) \
        AssertEqual(a,b, FandL)


    //��������� � �������
#define ASSERT(a) \
        Assert(a, FandL)


    //�������, ������������ ��������
template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {});

    //�������, ������������ �������� � �������
void Assert(bool b, const std::string& hint);


    //�����, ����������� �����
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
