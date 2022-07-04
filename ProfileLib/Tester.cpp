#include "Tester.h"
using namespace std;

std::ostream& NATests::operator<<(std::ostream& os, const NATests::test_result& s)
{
    if (s.fail) {
        os << "test: " << s.name << " fail: " << s.output;
    }
    else {
        os << "test: " << s.name << " OK.";
    }
    return os;
}