#include "BIFuncsTest.h"
#include "test_runner.h"
#include "Core.h"
#include "PredLispFuncs.h"
//пример теста
void test_eval_base() {
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("A");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("1");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "1");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("long_text_With_some_shit1234567890-");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "LONG_TEXT_WITH_SOME_SHIT1234567890-");
	}

	
}

//арифметические функции
void arifm() {
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(- 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "-3");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(- 10.5 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "7.5");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(/ 45.3 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "15.1");
	}
	
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(+ 10.5 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "13.5");
	}
	
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(* 10.5 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "31.5");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(- 10 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "7");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(+ 10 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "13");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(* 10 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "30");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(/ 45 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "15");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(+ 10.3 (* -1.0 3) -0.1 (/ 5 2.5))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "9.2");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(* 3 3 3 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "81");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(/ 81 3 3 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "3");
	}
}

//логические функции
void logic() {
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(< 2 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(> 2 3)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(< 2 2.0)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(> 2 2.0)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(<= 2 2.0)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(>= 2 2.0)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(= 2 2.0)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(< 2 2.5)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(> 2 1.5)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(getd (defun kok (x) (* x x)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(LAMBDA (X) (* X X))");
	}
}

//списковые функции
void list() {
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(car (quote (1 2 3)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "1");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(car (quote ((1 1) 2 3)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1 1)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cdr (quote (1 2 3)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(2 3)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cdr (quote ((1 1) 2 3)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(2 3)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(append (quote (1 1)) (quote (2 2)) (quote (3 3)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1 1 2 2 3 3)");
	}
	//...
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(append 2 (quote (1 1)) (quote (2 2)) (quote (3 3)))");
			//по версии фориса
		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1 1 2 2 3 3)");
	}*/
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(append (quote (1 1)) (quote (2 (4 4) 2)) (quote (3 3)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1 1 2 (4 4) 2 3 3)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(append (quote ()) (quote (1 2)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1 2)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cons a (quote (b)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(A B)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cons (quote (a)) (quote (b)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "((A) B)");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(list a b c)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(A B C)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(list a (quote b) c)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(A B C)");
	} {
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(list a (quote (s d)) c)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(A (S D) C)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(list (+ 1 2) (quote (s d)) c)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(3 (S D) C)");
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
