#include "BIFuncsTest.h"
#include "SerialReplCore.h"
#include "test_runner.h"

//пример теста
void test_eval_base() {
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("a");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "a");
	}

	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("1");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "1");
	}

	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("long_text_with_some_sybols1234567890-");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "long_text_with_some_sybols1234567890-");
	}
	
}

//тест встроренных функиций
void test_bifuncs()
{
	TestRunner r;
	RUN_TEST(r, test_eval_base);
}
