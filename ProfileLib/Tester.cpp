#include "Tester.h"
using namespace std;
using namespace NATests;

NATests::Tester::Tester(bool async_mode):
	t_async_mode(async_mode)
{
}

std::vector<NATests::test_result> NATests::Tester::Tester::execute()
{
	if (t_async_mode) {
		std::vector<NATests::test_result> result;
		result.reserve(t_async_tests.size());
		for (auto& f : t_async_tests) {
			result.push_back(f.get());
		}
		return result;
	}
	else {
		return t_non_async_tests;
	}
}

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
