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

//арифметические функции
void arifm() {
	/*{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(- 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "-3");
	}*/

	/*{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(- 10.5 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "7.5");
	}*/

	/*{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(/ 45.3 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "15.1");
	}
	
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(+ 10.5 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "13.5");
	}
	
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(* 10.5 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "31.5");
	}*/
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(- 10 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "7");
	}

	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(+ 10 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "13");
	}

	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(* 10 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "30");
	}

	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(/ 45 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "15");
	}

	/*{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(+ 10.3 (* -1.0 3) -0.1 (/ 5 2.5))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "9.2");
	}*/

	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(* 3 3 3 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "81");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(/ 81 3 3 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "3");
	}
}

//логические функции
void logic() {
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(< 2 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(> 2 3)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "nil");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(< 2 2.0)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "nil");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(> 2 2.0)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "nil");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(<= 2 2.0)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	/*{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(>= 2 2.0)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "T");
	}*/
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(= 2 2.0)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	/*{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(< 2 2.5)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "T");
	}*/
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(> 2 1.5)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	/*{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(getf (defun kok (x) (* x x)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(lambda (x) (* x x))");
	}*/
}

//списковые функции
void list() {
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(car (quote (1 2 3)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "1");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(car (quote ((1 1) 2 3)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(1 1)");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(cdr (quote (1 2 3)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(2 3)");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(cdr (quote ((1 1) 2 3)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(2 3)");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(append (quote (1 1)) (quote (2 2)) (quote (3 3)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(1 1 2 2 3 3)");
	}
	/*{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(append 2 (quote (1 1)) (quote (2 2)) (quote (3 3)))");
			//по версии фориса
		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(1 1 2 2 3 3)");
	}*/
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(append (quote (1 1)) (quote (2 (4 4) 2)) (quote (3 3)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(1 1 2 (4 4) 2 3 3)");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(append (quote ()) (quote (1 2)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(1 2)");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(cons a (quote (b)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(a b)");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(cons (quote (a)) (quote (b)))");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "((a) b)");
	}

	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(list a b c)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(a b c)");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(list a (quote b) c)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(a b c)");
	} {
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(list a (quote (s d)) c)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(a (s d) c)");
	}
	{
		SerialReplCore core;;
		auto [result_reason, result] = core.execute("(list (+ 1 2) (quote (s d)) c)");

		ASSERT_EQUAL(result_reason, SerialReplCore::result_type::success);
		ASSERT_EQUAL(result, "(3 (s d) c)");
	}
}

//тест встроренных функиций
void test_bifuncs()
{
	TestRunner r;
	RUN_TEST(r, test_eval_base);
	RUN_TEST(r, arifm);
	RUN_TEST(r, logic);
	RUN_TEST(r, list);
}
