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

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("((lambda (x) x) 1)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "1");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("((nlambda (x) x) a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("((nlambda (x) x) 1)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "1");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("((nlambda (x) x) a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}

	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if a kok nekok)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "KOK");
	}*/
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
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(+)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "0");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(*)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "1");
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

	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(>)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(<)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(>=)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(<=)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(=)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}*/

	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(> 1)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(< 1)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(>= 1)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(<= 1)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(= 1)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}*/

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(getd (defun kok (x) (* x x)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(LAMBDA (X) (* X X))");
	}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(getd)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}*/

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eq 2 2)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eq 2.0 2)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eq 2 2.5)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eq aa aa)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) (setq b 2) (eq a b))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eq 2 a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eq a 2)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}*/
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eq '(1 1) '(1 1))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eq '(1 1) a");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}*/



	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal 2 2)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal 2.0 2)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal 2 2.5)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal aa aa)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) (setq b 2) (equal a b))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal 2 a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal a 2)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal '(1 1) '(1 1))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::T_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal '(1 1) '(1 2))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(equal '(1 1) a");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, CoreEnvironment::nil_str);
	}*/
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
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(append (quote (1 1)) (quote (2 2)) (quote (3 3)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1 1 2 2 3 3)");
	}
	//...
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(append 2 (quote (1 1)) (quote (2 2)) (quote (3 3)))");
			//по версии фориса
		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1 1 2 2 3 3)");
	}
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
	}*/
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
		auto cell = core.execute_to_cell("(list (+ 1 2) (quote (s d)) c)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(3 (S D) C)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(list)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cons)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(NIL)");
	} 
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(append)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}*/
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(car)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cdr)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}

	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(last)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(last '())");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(last '(1 2 3))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(3)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(last '(1 2 3 (3 4)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "((3 4))");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(last (append '(1 2 3) '(3 2 1)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(length '(1 2 3))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "3");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(length (append '(1 2 3) '(1 2 3)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "6");
	}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(length ace)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "3");
	}*/
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(length (fib2 100))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "5");    //так-то хз
	}*/
}

//предикаты
void pred() {
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(null (list))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(null '())");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(null `())");         //косой апостроф

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}*/
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(null '(0))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(numberp 26)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(numberp 26.5)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(numberp -26.5)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(numberp 'a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(numberp '(1 1))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(numberp (car '(2 a)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(symbolp 26)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(symbolp 'a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(symbolp '(a a))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(symbolp (car '(a 2)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(listp 26)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(listp 'a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(listp '(a a))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(listp (car '((1 a 2) a 2)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(atom 26)");    

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(atom 'a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(atom '(a a))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(atom (car '(a 2)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}

	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(null)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(numberp)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(listp)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(atom)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(symbolp)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "T");
	}*/

}

//условные функции
void usl() {
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cond ((null '()) 'a) ((numberp 26) 'b) ((listp '(2)) 'c))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cond ((null '(1)) 'a) ((numberp 26) 'b) ((listp '(2)) 'c))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "B");
	} 
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '(2)) (car '(c 2))))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "C");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (T 'da))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "DA");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(cond)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if T 'a 'b)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if NIL 'a 'b)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "B");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if (null '()) 'a 'b)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if (null '(2)) 'a 'b)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "B");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if (null '()) 'a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if (null '(2)) 'a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if (null '()))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if (null '()) (+ 1 2) 'b)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "3");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if (null '(2)) (+ 1 2) (+ 3 4))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "7");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(if)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
}

//функции вычислений
void calcfun() {
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eval)");            //тут вообще в минус

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eval a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) (eval a))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "2");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) (setq b 'a) (eval b))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "2");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) (setq b 'a) (eval 'b))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) ((null nil) nil) (eval 'b))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) ((null T) nil) (eval 'b))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "B");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eval '(+ 1 2))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "3");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eval ((lambda (x) (* x x)) 3))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "9");
	}
	//{
	//	auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
	//	auto [result_reason, result] = core.execute("(eval ''''((lambda (x) (* x x)) 3))");    //мнение фориса

	//	ASSERT_EQUAL(result_reason, Core::result_type::success);
	//	ASSERT_EQUAL(result, "9");
	//}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(quote a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("'(quote a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(QUOTE A)");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("'''a");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(QUOTE (QUOTE A))");
	}
	//{
	//	auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
	//	auto [result_reason, result] = core.execute("`a");

	//	ASSERT_EQUAL(result_reason, Core::result_type::success);
	//	ASSERT_EQUAL(result, "A");
	//}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(quote)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}*/
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(apply)");

		ASSERT_EQUAL(result_reason, Core::result_type::fail);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(apply kok)");

		ASSERT_EQUAL(result_reason, Core::result_type::fail);
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(apply '+ '(1 2 3))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "6");
	}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(apply 'append '((1 2) (3 4)))"); //составной лист ломает

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "(1 2 3 4)");
	}*/
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(apply '(lambda (X A) (* X A)) '(3 4))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "12");
	}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(apply '(lambda (X L) (* X (car L))) '(3 (4 5 6)))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "12");
	}*/
}

//управление вычислениями
void control_calc() {
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(defun kek (x) (* x x))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "KEK");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (defun kek (x) (* x x)) (kek 3))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "9");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (defun kek (x a) (* x x) a) (kek 3))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(defun)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(defun kek)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(defun kek (x))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "KEK");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (+ 1 2) (+ 3 4))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "7");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) (+ 2 a) )");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "4");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(prog1)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(prog1 (+ 1 2) (+ 3 4))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "3");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(prog1 (setq a 2) (+ 2 a) (setq a 3))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "2");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(setq)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(setq a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(setq a 2)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "2");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(eval (setq a 2))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "2");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "2");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(set)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(set a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq a 2) (setq b 'a) (set b 3) a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "3");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq L '(1 2 3 4)) (loop ((null L) 'da) (setq L (cdr L))))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "DA");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq N 0) (setq L '(1 2 3 4)) (loop ((null L) N) (setq N (+ 1 N)) (setq L (cdr L))))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "4");
	}
	
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(progn (setq L '(1 a 2 q kek)) (loop ((null(cdr L)) (car L)) (setq L(cdr L))))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "KEK");
	}
}

//ввод/вывод
void io() {
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(print)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");  //формат вывода под вопросом
	}*/
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(print a)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "A");
	}
	{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(print (+ 1 2 3))");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "6");
	}
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(print 'а_вообще_хз_как_его_тестировать)");

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "а_вообще_хз_как_его_тестировать");
	}*/
	/*{
		auto [core, streams] = make_core_w_predfuncs_and_empty_streams();
		auto [result_reason, result] = core.execute("(setq a (read))");         //крч тоже хз, вроде работает

		ASSERT_EQUAL(result_reason, Core::result_type::success);
		ASSERT_EQUAL(result, "NIL");
	}*/ 
}

//тест встроренных функиций
void test_bifuncs()
{
	TestRunner r;
	RUN_TEST(r, test_eval_base);
	RUN_TEST(r, arifm);
	RUN_TEST(r, logic);
	RUN_TEST(r, list);
	RUN_TEST(r, pred);
	RUN_TEST(r, usl);
	RUN_TEST(r, calcfun);
	RUN_TEST(r, control_calc);
	RUN_TEST(r, io);
}
