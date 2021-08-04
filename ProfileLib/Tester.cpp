#include "Tester.h"
using namespace std;
using namespace NATests;

NATests::Tester::Tester(bool async_mode):
	t_async_mode(async_mode)
{
}

std::vector<NATests::test_result> NATests::Tester::Tester::execute()
{
	std::vector<NATests::test_result> result;
	result.reserve(t_tests.size());
	if (t_async_mode) {
		std::vector<future<test_result>> asyncs;
		for (const auto& fnc : t_tests) {
			asyncs.emplace_back(async(std::launch::async, fnc));
		}
		for (auto& f : asyncs) {
			result.push_back(f.get());
		}
	}
	else {
		for (const auto& f : t_tests) {
			result.push_back(f());
		}
	}
	t_tests.clear();
	return result;
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
