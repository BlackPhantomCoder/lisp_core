#include "BIFuncsTest.h"
#include "test_runner.h"
#include "Core.h"
#include "PredLispFuncs.h"
#include "StdCoreInputMemoryStream.h"
#include "Tester.h"

#include <sstream>
#include <fstream>

using namespace CoreData;
using namespace std;


//загрузка файла с пердфанками в оперативу
static const StdCoreInputMemoryStream predfuncs_mem_stream_global = []() {
	ifstream f(predfuncs_filename);
	return StdCoreInputMemoryStream(f, stream_read_mode::s_expression);
}();

tuple< unique_ptr<empty_streams>, Core::result_type, string> assert_core_one(const string& input) {
	auto predfuncs_mem_stream = StdCoreInputMemoryStream(predfuncs_mem_stream_global);
	auto core_p = make_core_w_custom_predfuncs_and_empty_streams(predfuncs_mem_stream);
	auto result_p = core_p.first.execute_one(input);
	return { move(core_p.second), result_p.first, move(result_p.second) };
}

tuple< ostringstream, Core::result_type, string> assert_core_one_w_input_stream(const string& input_stream_data, const string& input) {
	auto predfuncs_mem_stream = StdCoreInputMemoryStream(predfuncs_mem_stream_global);
	istringstream in(input_stream_data);
	ostringstream out;
	auto core = make_core_w_custom_predfuncs(predfuncs_mem_stream, in, stream_read_mode::new_string, out);
	auto result_p = core.execute_one(input);
	return { move(out), result_p.first, move(result_p.second) };
}

tuple< unique_ptr<empty_streams>, Core::result_type, vector<string>> assert_core_all(vector<string>&& input) {
	auto predfuncs_mem_stream = StdCoreInputMemoryStream(predfuncs_mem_stream_global);
	auto core_p = make_core_w_custom_predfuncs_and_empty_streams(predfuncs_mem_stream);
	auto is = StdCoreInputValArrayInput(move(input), stream_read_mode::new_string); \
		auto result_p = core_p.first.execute_all(is);
	return { move(core_p.second), result_p.first, move(result_p.second) };
}

	//Тест со сравнением результата
#define simple_core_assert(input, output)\
{\
	auto [streams, result_reason, result] = assert_core_one(input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success);\
	ASSERT_EQUAL(result, output);\
}

//Тест со сравнением результата
#define simple_core_assert_several(input, output)\
{\
	auto [streams, result_reason, result] = assert_core_all(vector<string> input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success);\
	ASSERT_EQUAL(result, vector<string>output);\
}

//Тест со сравнением результата только последней команды
#define simple_core_assert_several_last_val(input, output)\
{\
	auto [streams, result_reason, result] = assert_core_all(vector<string> input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success);\
	ASSERT_EQUAL(result.back(), output);\
}

//Тест без сравнения результата (проверка на success/fail) (если ExceptionCaught не задефайнено - не выполняются)
#ifdef EX_CATCH
#define simple_core_assert_reason(input, reason)\
	{\
		auto [streams, result_reason, result] = assert_core_one(input);\
		ASSERT_EQUAL(result_reason,reason);\
	}
#endif
#ifndef EX_CATCH
#define simple_core_assert_reason(input, reason)
#endif


//тест вывода
#define test_output(input, output)\
{\
	auto [streams, result_reason, result] = assert_core_one(input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success); \
	ASSERT_EQUAL(streams->out.str(), output);\
}

//тест ввода
#define test_input(input,stream_input, output)\
{\
	auto [out, result_reason, result] = assert_core_one_w_input_stream(stream_input, input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success);\
	ASSERT_EQUAL(result, output);\
}

//тест ввода+вывода+результат
#define test_io_result(input, stream_input, stream_output, output)\
{\
	auto [out, result_reason, result] = assert_core_one_w_input_stream(stream_input, input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success);\
	ASSERT_EQUAL(result, output);\
	ASSERT_EQUAL(out.str(), stream_output);\
}

//тест ввода+причина (проверка на success/fail) (если ExceptionCaught не задефайнено - не выполняются)
#ifdef EX_CATCH
#define test_input_reason(input, stream_input, reason)\
	{\
			auto [out, result_reason, result] = assert_core_one_w_input_stream(stream_input, input);\
			ASSERT_EQUAL(result_reason, reason);\
	}
#endif
#ifndef EX_CATCH
#define test_input_reason(input, stream_input, reason)
#endif


//пример теста
void test_eval_base() {
	simple_core_assert("A", "A");
	simple_core_assert("1", "1");
	simple_core_assert("-1", "-1");
	simple_core_assert("+1", "1");
	simple_core_assert("long_text_With_some_shit1234567890", "LONG_TEXT_WITH_SOME_SHIT1234567890");
	simple_core_assert("((lambda (x) x) 1)", "1");
	simple_core_assert_several_last_val(({ "(setq a b)", "(setq b 1)", "((lambda (x) x) a)" }), "B");
	simple_core_assert_several_last_val(({ "(setq a nil)", "(if a t nil)" }), "NIL");
	simple_core_assert_several_last_val(({ "(setq a b)", "(setq b nil)", "(if a t nil)" }), "T");
	simple_core_assert_several_last_val(({ "(setq a b)", "(setq b nil)", "(if b t nil)" }), "NIL");
	simple_core_assert("(if a kok nekok)", "KOK");
	simple_core_assert("(not nil)", "T");
	simple_core_assert("(not T)", "NIL");
	simple_core_assert_several_last_val(({ "(setq a b)", "(setq b nil)", "(not a)" }), "NIL");
	simple_core_assert("((lambda (x) (cdr x)) '(1 2))", "(2)");
	simple_core_assert_reason("((nlambda (x) x) a)", Core::result_type::fail);
	simple_core_assert("(copy-tree '(1))", "(1)");
	simple_core_assert("(copy-tree '(1 2))", "(1 2)");
	simple_core_assert("(copy-tree '(1 2 . 3))", "(1 2 . 3)");
	simple_core_assert("(copy-tree '((1 2 . 3) (4 . 5)))", "((1 2 . 3) (4 . 5))");
	simple_core_assert("(copy-tree '((1 2 3) (4 (5 (6) 7) 8) . (10 (5))))", "((1 2 3) (4 (5 (6) 7) 8) 10 (5))");
	simple_core_assert("((lambda (x '(n) y) (list x n y)) 1 2 3)", "(1 N 3)");
}

//арифметические функции
void arifm() {
	simple_core_assert("(- 3)", "-3");
	simple_core_assert("(- 10.5 3)", "7.5");
	simple_core_assert("(/ 45.3 3)", "15.1");
	simple_core_assert("(+ 10.5 3)", "13.5");
	simple_core_assert("(* 10.5 3)", "31.5");
	simple_core_assert("(- 10 3)", "7");
	simple_core_assert("(+ 10 3)", "13");
	simple_core_assert("(* 10 3)", "30");
	simple_core_assert("(/ 45 3)", "15");
	simple_core_assert("(+ 10.3 (* -1.0 3) -0.1 (/ 5 2.5))", "9.2");
	simple_core_assert("(* 3 3 3 3)", "81");
	simple_core_assert("(/ 81 3 3 3)", "3");
	simple_core_assert("(+)", "0");
	simple_core_assert("(*)", "1");
}

//логические функции
void logic() {
	simple_core_assert("(< 2 3)", T_str);
	simple_core_assert("(> 2 3)", nil_str);
	simple_core_assert("(< 2 2.0)", nil_str);
	simple_core_assert("(> 2 2.0)", nil_str);
	simple_core_assert("(<= 2 2.0)", T_str);
	simple_core_assert("(>= 2 2.0)", T_str);
	simple_core_assert("(= 2 2.0)", T_str);
	simple_core_assert("(< 2 2.5)", T_str);
	simple_core_assert("(> 2 1.5)", T_str);

	simple_core_assert("(>)", T_str);
	simple_core_assert("(<)", T_str);
	simple_core_assert("(>=)", T_str);
	simple_core_assert("(<=)", T_str);
	simple_core_assert("(=)", T_str);

	simple_core_assert("(> 1)", T_str);
	simple_core_assert("(< 1)", T_str);
	simple_core_assert("(>= 1)", T_str);
	simple_core_assert("(<= 1)", T_str);
	simple_core_assert("(= 1)", T_str);

	simple_core_assert("(getd (defun kok (x) (* x x)))", "(LAMBDA (X) (* X X))");
	simple_core_assert("(getd)", nil_str);

	simple_core_assert("(eq 2 2)", T_str);
	simple_core_assert("(eq 2.0 2)", T_str);
	simple_core_assert("(eq 2 2.5)", nil_str);
	simple_core_assert("(eq aa aa)", T_str);
	simple_core_assert("(progn (setq a 2) (setq b 2) (eq a b))", T_str);
	simple_core_assert("(eq 2 a)", nil_str);
	simple_core_assert("(eq a 2)", nil_str);
	simple_core_assert("(eq '(1 1) '(1 1))", nil_str);
	simple_core_assert("(eq '(1 1) a)", nil_str);
	simple_core_assert("(eq '() nil)", T_str);
	simple_core_assert("(eq)", T_str);
	simple_core_assert("(eq nil nil)", T_str);

	simple_core_assert("(equal 2 2)", T_str);
	simple_core_assert("(equal 2.0 2)", T_str);
	simple_core_assert("(equal 2 2.5)", nil_str);
	simple_core_assert("(equal aa aa)", T_str);
	simple_core_assert("(progn (setq a 2) (setq b 2) (equal a b))", T_str);
	simple_core_assert("(equal 2 a)", nil_str);
	simple_core_assert("(equal a 2)", nil_str);
	simple_core_assert("(equal '(1 1) '(1 1))", T_str);
	simple_core_assert("(equal '(1 1) '(1 2))", nil_str);
	simple_core_assert("(equal '(1 1) a)", nil_str);
	simple_core_assert("(equal '() nil)", T_str);
	simple_core_assert("(equal nil nil)", T_str);
}

//списковые функции
void lists() {
	simple_core_assert("(car (quote (1 2 3)))", "1");
	simple_core_assert("(car (quote ((1 1) 2 3)))", "(1 1)");
	simple_core_assert("(cdr (quote (1 2 3)))", "(2 3)");
	simple_core_assert("(cdr (quote ((1 1) 2 3)))", "(2 3)");

	simple_core_assert("(append (quote (1 1)) (quote (2 2)) (quote (3 3)))", "(1 1 2 2 3 3)");
	simple_core_assert("(append 2 (quote (1 1)) (quote (2 2)) (quote (3 3)))", "(1 1 2 2 3 3)");
	simple_core_assert("(append (quote (1 1)) (quote (2 (4 4) 2)) (quote (3 3)))", "(1 1 2 (4 4) 2 3 3)");
	simple_core_assert("(append (quote ()) (quote (1 2)))", "(1 2)");

	simple_core_assert("(cons a (quote (b)))", "(A B)");
	simple_core_assert("(cons (quote (a)) (quote (b)))", "((A) B)");
	simple_core_assert("(list a b c)", "(A B C)");
	simple_core_assert("(list a (quote b) c)", "(A B C)");
	simple_core_assert("(list a (quote (s d)) c)", "(A (S D) C)");
	simple_core_assert("(list (+ 1 2) (quote (s d)) c)", "(3 (S D) C)");
	simple_core_assert("(list)", nil_str);
	simple_core_assert("(cons)", "(NIL)");
	simple_core_assert("(append)", nil_str);
	simple_core_assert("(car)", nil_str);
	simple_core_assert("(cdr)", nil_str);
	simple_core_assert("(last)", nil_str);
	simple_core_assert("(last '())", nil_str);
	simple_core_assert("(last '(1 2 3))", "(3)");
	simple_core_assert("(last '(1 2 3 (3 4)))", "((3 4))");
	simple_core_assert("(last (append '(1 2 3) '(3 2 1)))", "(1)");
	simple_core_assert("(length '(1 2 3))", "3");
	simple_core_assert("(length (append '(1 2 3) '(1 2 3)))", "6");
	//simple_core_assert("(length ace)","3");
	//simple_core_assert("(length (fib2 100))","5");    //так-то хз
}

//предикаты
void pred() {
	simple_core_assert("(null (list))", T_str);
	simple_core_assert("(null '())", T_str);
	simple_core_assert("(null nil)", T_str);
	//simple_core_assert("(null `())",T_str); //косой апостроф,
	simple_core_assert("(null '(0))", nil_str);
	simple_core_assert("(numberp 26)", T_str);
	simple_core_assert("(numberp 26.5)", T_str);
	simple_core_assert("(numberp -26.5)", T_str);
	simple_core_assert("(numberp 'a)", nil_str);
	simple_core_assert("(numberp '(1 1))", nil_str);
	simple_core_assert("(numberp (car '(2 a)))", T_str);
	simple_core_assert("(symbolp 26)", nil_str);
	simple_core_assert("(symbolp 'a)", T_str);
	simple_core_assert("(symbolp '(a a))", nil_str);
	simple_core_assert("(symbolp (car '(a 2)))", T_str);
	simple_core_assert("(listp 26)", nil_str);
	simple_core_assert("(listp 'a)", nil_str);
	simple_core_assert("(listp '(a a))", T_str);
	simple_core_assert("(listp (car '((1 a 2) a 2)))", T_str);
	simple_core_assert("(atom 26)", T_str);
	simple_core_assert("(atom 'a)", T_str);
	simple_core_assert("(atom '(a a))", nil_str);
	simple_core_assert("(atom (car '(a 2)))", T_str);
	simple_core_assert("(null)", T_str);
	simple_core_assert("(numberp)", nil_str);
	simple_core_assert("(listp)", T_str);
	simple_core_assert("(atom)", T_str);
	simple_core_assert("(symbolp)", T_str);
}

//условные функции
void usl() {
	simple_core_assert("(cond ((null '()) 'a) ((numberp 26) 'b) ((listp '(2)) 'c))", "A");
	simple_core_assert("(cond ((null '(1)) 'a) ((numberp 26) 'b) ((listp '(2)) 'c))", "B");
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '(2)) (car '(c 2))))", "C");
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c))", nil_str);
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (T 'da))", "DA");
	simple_core_assert("(cond)", nil_str);
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (a 'da))", "DA");
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (1 'da))", "DA");   //возможно, форис делает \1
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (nil 'da))", nil_str);
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) ('(1 2) 'da))", "DA");
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) ((cons a b) 'da))", "DA");

	simple_core_assert("(if T 'a 'b)", "A");
	simple_core_assert("(if NIL 'a 'b)", "B");
	simple_core_assert("(if (null '()) 'a 'b)", "A");
	simple_core_assert("(if (null '(2)) 'a 'b)", "B");
	simple_core_assert("(if (null '()) 'a)", "A");
	simple_core_assert("(if (null '(2)) 'a)", nil_str);
	simple_core_assert("(if (null '()))", nil_str);
	simple_core_assert("(if (null '()) (+ 1 2) 'b)", "3");
	simple_core_assert("(if (null '(2)) (+ 1 2) (+ 3 4))", "7");
	simple_core_assert("(if)", nil_str);
	simple_core_assert("(if '(1 2) 'a 'b)", "A");
	simple_core_assert("(if '(1 . 2) 'a 'b)", "A");
	simple_core_assert("(if 1 'a 'b)", "A");
}

//функции вычислений
void calcfun() {
	simple_core_assert("(eval)", nil_str);
	simple_core_assert("(eval a)", "A");
	simple_core_assert("(progn (setq a 2) (eval a))", "2");
	simple_core_assert("(progn (setq a 2) (setq b 'a) (eval b))", "2");
	simple_core_assert("(progn (setq a 2) (setq b 'a) (eval 'b))", "A");
	simple_core_assert("(progn (setq a 2) ((null nil) nil) (eval 'b))", nil_str);
	simple_core_assert("(progn (setq a 2) ((null T) nil) (eval 'b))", "B");
	simple_core_assert("(eval '(+ 1 2))", "3");
	simple_core_assert("(eval ((lambda (x) (* x x)) 3))", "9");
	//simple_core_assert("(eval ''''((lambda (x) (* x x)) 3))","9");    //мнение фориса
	simple_core_assert("(quote a)", "A");
	simple_core_assert("'(quote a)", "(QUOTE A)");
	simple_core_assert("'''a", "(QUOTE (QUOTE A))");
	//simple_core_assert("`a", "A");
	simple_core_assert("(quote)", nil_str);

	simple_core_assert_reason("(apply)", Core::result_type::fail);
	simple_core_assert_reason("(apply kok)", Core::result_type::fail);

	simple_core_assert("(apply '+ '(1 2 3))", "6");
	simple_core_assert("(apply 'append '((1 2) (3 4)))", "(1 2 3 4)");
	simple_core_assert("(apply '(lambda (X A) (* X A)) '(3 4))", "12");
	simple_core_assert("(apply '(lambda (X L) (* X (car L))) '(3 (4 5 6)))", "12");
}

//управление вычислениями
void control_calc() {
	simple_core_assert("(defun kek (x) (* x x))","KEK");
	simple_core_assert("(progn (defun kek (x) (* x x)) (kek 3))","9");
	simple_core_assert("(progn (defun kek (a 's d) d) (kek 1 2 3))", "3");
	simple_core_assert("(progn (defun kek (a '(kok nekok) d) d) (kek 1 2 3))", "3");
	simple_core_assert("(progn (defun kek (x a) (* x x) a) (kek 3))",nil_str);
	simple_core_assert("(progn (defun kok () (setq x 1000) (loop ((< x 0) x) (setq x (- x 7)))) (kok))", "-1"); 
	simple_core_assert("(defun)",nil_str);
	simple_core_assert("(defun kek)",nil_str);
	simple_core_assert("(defun kek (x))","KEK");
	simple_core_assert("(progn)",nil_str);
	simple_core_assert("(progn (+ 1 2) (+ 3 4))","7");
	simple_core_assert("(progn (setq a 2) (+ 2 a) )","4");
	simple_core_assert("(prog1)",nil_str);
	simple_core_assert("(prog1 (+ 1 2) (+ 3 4))","3");
	simple_core_assert("(prog1 (setq a 2) (+ 2 a) (setq a 3))","2");
	simple_core_assert("(setq)",nil_str);
	simple_core_assert("(setq a)",nil_str);
	simple_core_assert("(setq a 2)","2");
	simple_core_assert("(eval (setq a 2))","2");
	simple_core_assert("(progn (setq a 2) a)","2");
	simple_core_assert("(set)",nil_str);
	simple_core_assert("(set a)",nil_str);
	test_output("(progn (setq a 1 b 2) (print a) (print b))", "1\n2\n");

	simple_core_assert("(progn (setq a 2) (setq b 'a) (set b 3) a)", "3");
	simple_core_assert("(progn (setq L '(1 2 3 4)) (loop ((null L) 'da) (setq L (cdr L))))", "DA");
	simple_core_assert("(progn (setq N 0) (setq L '(1 2 3 4)) (loop ((null L) N) (setq N (+ 1 N)) (setq L (cdr L))))", "4");
	simple_core_assert("(progn (setq L '(1 a 2 q kek)) (loop ((null(cdr L)) (car L)) (setq L(cdr L))))","KEK");
	simple_core_assert(
		"(progn (setq lst '(1 (2 3) (2 . 3) a nil 4 5)) (loop ((not (car lst)) (cdr lst)) (print lst) (setq lst (cdr lst))))",
		"(4 5)");
	simple_core_assert_several_last_val(
		({
			"(setq L '(1 a 2 q kek))",
			"(loop ( (null (cdr L)) (car L)) (setq L (cdr L)) )"
		}),
		"KEK"
	);
	simple_core_assert_several_last_val(
		({
			"(setq lst '(1 (2 3) (2 . 3) a nil 4 5))",
			"(loop ((not (car lst)) (cdr lst)) (print lst) (setq lst (cdr lst)))"
		}),
		"(4 5)"
	);
	test_output(
		"(progn (setq lst '(1 (2 3) (2 . 3) a nil 4 5)) (loop ((not (car lst)) (cdr lst)) (print lst) (setq lst (cdr lst))))",
		"(1 (2 3) (2 . 3) A NIL 4 5)\n((2 3) (2 . 3) A NIL 4 5)\n((2 . 3) A NIL 4 5)\n(A NIL 4 5)\n");
}

//ввод/вывод
void io() {
	simple_core_assert("(print)",nil_str);  
	simple_core_assert("(print a)","A");
	simple_core_assert("(print (+ 1 2 3))", "6");
	test_output("(print 'а_вообще_хз_как_его_тестировать)", "а_вообще_хз_как_его_тестировать\n");
	test_input("(setq a (read))", "A", "A");
	test_io_result("(print (read))", "A", "A\n", "A");

	//тест на пустой ввод 
	//(не знаю нужно ли или нет, но если поток не интерактивный (как cin, например),
	//то не должно в бесконечном цикле ждать вроде как)
	test_io_result("(read)", "", "", nil_str);
	test_io_result("(read)", " ", "", nil_str);
	test_io_result("(read)", " ;a?", "", nil_str);

	//тест ввода-вывода без перевода в капс
	test_io_result(string("(progn (setq ") + read_up_case_str + " nil) (print (read)))", "aBc1", "aBc1\n", "aBc1");

	//тест | и "
	simple_core_assert("|AbC1|", "|AbC1|");
	simple_core_assert("\"AbC1\"", "|AbC1|");

	//тест на анализ ввода через read
	test_io_result("(eval '((lambda x (print (read))) nil))", "1b", string(nil_str) + "\n", nil_str);
	//тест на анализ ввода
	simple_core_assert_reason("1c2", Core::result_type::fail);

	simple_core_assert("a\\a12", "|Aa12|");
	simple_core_assert("|AAC1|", "AAC1");
	simple_core_assert("|1AAC1|", "|1AAC1|");
	simple_core_assert("\\ \\ \\ ", "|   |");

	//\(\)\ \'\\\;\#
	simple_core_assert("\\(\\)\\ \\'\\\\\\;\\#", "|() '\\\\;#|");
	simple_core_assert("A\\\\B.LSP", "|A\\\\B.LSP|");
	simple_core_assert("\\++++", "++++");
	
}

//идентификаторы
void id_repr() {
	simple_core_assert("\\ ", "| |");
	simple_core_assert("\\1", "\\1");
	simple_core_assert("\\1a", "|1A|");
	//simple_core_assert_reason("\\\\", Core::result_type::success);
	//simple_core_assert("\\\\", "\\\\");
	simple_core_assert_reason("\\\\\\a", Core::result_type::success);
	//simple_core_assert("\\\\\\\\", "|\\\\\\\\|");						//надо согласовать 
	//simple_core_assert("\\a\\|", "|a\\||");							//с методичкой стр.44, последний абзац
	simple_core_assert("(pack '(\\| \\|))", "|\\|\\||");				//
	//simple_core_assert("(pack '(\\| \\\\ \\|))", "|\\|\\\\\\||");

	//simple_core_assert(".", "\\.");
	//simple_core_assert(".qwe", ".QWE");
	//simple_core_assert(".12", ".12");
	simple_core_assert_reason(",", Core::result_type::fail);    //возможно, стоит обратить внимание на причину фейла
	simple_core_assert("\\,", "\\,");
	//simple_core_assert_reason("!@#,!##", Core::result_type::fail);
	simple_core_assert("!@#\\,!@#", "|!@#,!@#|");
	simple_core_assert("q\\q1\\1`~!@#$%^&*-_=+/\\;\\'\\\".\\,:", "|Qq11`~!@#$%^&*-_=+/;'\".,:|");
	simple_core_assert("q1~!@#$%^&*-_=+/:", "Q1~!@#$%^&*-_=+/:");
	//simple_core_assert("a`", "A`");       //возможно, рофл из коммона (в 85 не загружен)
	//simple_core_assert("q1`~!@#$%^&*-_=+/:", "Q1`~!@#$%^&*-_=+/:");

	simple_core_assert("\\a", "\\a");
	simple_core_assert("\\A", "A");
	simple_core_assert("||", "||");
	simple_core_assert("\"\"", "||");
	simple_core_assert("\\\"\\\"", "|\"\"|");
	simple_core_assert("\"|\"", "\\|");
	simple_core_assert(std::string("(progn (setq ") + read_up_case_str + " nil) \\a) ", "a");
	simple_core_assert(std::string("(progn (setq ") + read_up_case_str + " nil) \\A) ", "A");

}

//функции над идентификаторами
void id_func() {
	simple_core_assert("(numberp 345)", T_str);
	simple_core_assert("(numberp \\345)", nil_str);
	simple_core_assert("(numberp '\\345)", nil_str);
	simple_core_assert("(symbolp '\\345)", T_str);
	simple_core_assert("(atom '\\345)", T_str);
	simple_core_assert("(listp '\\345)", nil_str);
	simple_core_assert("(eq \\345 \\3\\4\\5)", T_str);
	simple_core_assert("(equal \\345 \\3\\4\\5)", T_str);
	simple_core_assert("(eq \\aa aa)", nil_str);
	simple_core_assert("(equal \\aa aa)", nil_str);
	simple_core_assert("(equal |AAC1| aac1)", T_str);
	simple_core_assert("(equal q1 q\\1)", T_str);
	simple_core_assert("(equal A \\A)", T_str);
	simple_core_assert("(equal \\a \\a)", T_str);
	simple_core_assert("(equal \\a\\a |aa|)", T_str);
	simple_core_assert("(equal || \"\")", T_str);
	simple_core_assert("(eq \"a12\" \"a12\")", T_str);     //методичка утверждает обратное (но там коммон)
	simple_core_assert("(equal \"a12\" \"a12\")", T_str);
	simple_core_assert("(progn (setq \\345 1) \\345)", "1");
	simple_core_assert("(progn (setq |kok| 2) \\k\\o\\k)", "2");

	simple_core_assert("(unpack kok)", "(K O K)");
	simple_core_assert("(unpack |kok|)", "(\\k \\o \\k)");
	simple_core_assert("(unpack q\\q1\\1)", "(Q \\q \\1 \\1)");
	simple_core_assert("(unpack)", "(N I L)");
	simple_core_assert("(unpack '(1 2 3))", nil_str);
	simple_core_assert("(unpack 1)", "(\\1)");
	simple_core_assert("(unpack 1.2)", "(\\1 \\. \\2)");
	simple_core_assert("(unpack \"kok11\")", "(\\k \\o \\k \\1 \\1)");
	simple_core_assert("(equal (unpack \\345) (unpack \\3\\4\\5))", T_str);
	simple_core_assert("(numberp (cadr (unpack a1a)))", nil_str);
	simple_core_assert("(pack)", "||");
	simple_core_assert("(pack '(1 2 3))", "|123|");
	simple_core_assert("(pack '(\\1 \\2 \\3))", "|123|");
	simple_core_assert("(equal (pack '(1 2 3)) (pack '(\\1 \\2 \\3)))", T_str);
	simple_core_assert("(pack '(kok nekok))", "KOKNEKOK");
	simple_core_assert("(pack '(kok |kekw| nekok))", "|KOKkekwNEKOK|");
	simple_core_assert("(pack '(kok \"kekw\" nekok))", "|KOKkekwNEKOK|");
	simple_core_assert("(pack '((1 2) (3 4) 5))", "\\5");
	simple_core_assert("(equal (pack '(1 2 3)) (pack '(123)))", T_str);

	simple_core_assert("(cadr (unpack (pack '(a 2 a))))", "\\2");
	simple_core_assert("(pack (unpack \\345))", "|345|");
	simple_core_assert("(pack '(| |))", "| |");
	simple_core_assert("(unpack |\\|\\||)", "(\\| \\|)");
}

//точечные пары
void dotpairs() {
	simple_core_assert("'(a . b)", "(A . B)");
	simple_core_assert("'(a.b)", "(A.B)");
	simple_core_assert("(car '(a . b))", "A");
	simple_core_assert("(cdr '(a . b))", "B");
	simple_core_assert("(cdr '(c a . b))", "(A . B)");
	simple_core_assert("(car '(a.b))", "A.B");
	simple_core_assert("'(1 . (2 . (3 . nil)))", "(1 2 3)");

	simple_core_assert("'(a . nil)", "(A)");
	simple_core_assert("'(nil . b)", "(NIL . B)");
	simple_core_assert("'(nil . nil)", "(NIL)");
	simple_core_assert_reason("'(1 2 3 . 4)", Core::result_type::success);
	simple_core_assert_reason("'(1 2 . 3 4)", Core::result_type::fail);

	simple_core_assert("'(1 2 (3 . 4) 5 . 6)", "(1 2 (3 . 4) 5 . 6)");
	simple_core_assert("(last '(1 2 (3 . 4) 5 . 6))", "(5 . 6)");
	simple_core_assert("(caddr '(1 2 (3 . 4) 5 . 6))", "(3 . 4)");
	simple_core_assert("(length '(1 2 (3 . 4) 5 . 6))", "4");
	simple_core_assert("(length '(1 . 2))", "1");

	simple_core_assert("(cons a b)", "(A . B)");
	simple_core_assert("(cons c (cons a b))", "(C A . B)");
	simple_core_assert("(car (cons a b))", "A");
	simple_core_assert("(cdr (cons a b))", "B");
	simple_core_assert("(cons '(a . b) c)", "((A . B) . C)");
	simple_core_assert("(cons a (list b))", "(A B)");
	simple_core_assert("(cons (list a) b)", "((A) . B)");
	simple_core_assert("(append '(1 . 2) '(3 . 4))", "(1 3 . 4)");

	simple_core_assert("(listp '(1 2 . 3))", T_str);
	simple_core_assert("(listp '(2 . 3))", T_str);
	simple_core_assert("(atom '(1 2 . 3))", nil_str);
	simple_core_assert("(null '(1 2 . 3))", nil_str);
	simple_core_assert("(null '(nil . 3))", nil_str);
	simple_core_assert("(null '(1 2 . nil))", nil_str);
	simple_core_assert("(null '(nil . nil))", nil_str);
	simple_core_assert("(equal '(1 2 . nil) '(1 2))", T_str);
	simple_core_assert("(eq '(1 2 . nil) '(1 2))", nil_str);

	simple_core_assert("(progn (setq b 2) (setq a '(b . c)) (car a))", "B");
	simple_core_assert("(progn (setq b 2) (setq a (cons b c)) (car a))", "2");
	test_output("(print '(1 . 2))", "(1 . 2)\n");
	simple_core_assert("(print '(1 . 2))", "(1 . 2)");


	simple_core_assert(
		"(progn (setq foo '(a b c)) (rplaca foo d) foo)",
		"(D B C)"
	);
	simple_core_assert(
		"(progn (setq foo '(a b c)) (rplacd foo d) foo)",
		"(A . D)"
	);
	simple_core_assert(
		"(progn (setq x (list 'a 'b)) (setq z1 (cons x x)) (defun stk (x) (rplaca (car x) 'kok) x) (stk z1))",
		"((KOK B) KOK B)"
	);
	simple_core_assert(
		"(progn (setq x (list 'a 'b)) (setq z2 (cons (list 'a 'b) (list 'a 'b))) (defun stk (x) (rplaca (car x) 'kok) x) (stk z2))",
		"((KOK B) A B)"
	);
	test_output(
		"(progn (setq lst '(1 (2 3) (2 . 3) a nil 4 5)) (loop ((not (car lst)) (cdr lst)) (print lst) (setq lst (cdr lst))))",
		"(1 (2 3) (2 . 3) A NIL 4 5)\n((2 3) (2 . 3) A NIL 4 5)\n((2 . 3) A NIL 4 5)\n(A NIL 4 5)\n");
}

//макросы
void macros() {
	simple_core_assert("(defmacro)", nil_str);
	simple_core_assert("(defmacro kek)", nil_str);
	simple_core_assert("(defmacro kek (x))", "KEK");
	simple_core_assert("(defmacro setqq (x y) (list 'setq x (list 'quote y)))", "SETQQ");
	simple_core_assert("(progn (defmacro kek (a 's d) d) (kek 1 2 3))", "3");
	simple_core_assert("(progn (setq nekok chel) (defmacro setqq (x y) (list 'setq x (list 'quote y))) (setqq kok nekok) kok)", "NEKOK");
	simple_core_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macrocdr (q w e r) 2))",
		"A");       //стоит подумать над примером
	test_output(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macrocdr (q w e r t) 3))",
		"(R T)\n");
	simple_core_assert(
		"(progn (defmacro macrocdr (X) X) (getd macrocdr))",
		"(MACRO BODY ((LAMBDA (X) X) (CAR (CDR BODY))))");
	simple_core_assert("(progn (defmacro len lst (length lst)) (len 1 2 3 4 5 6 7 8))", "8");
	simple_core_assert(
		"(progn (defmacro when (e . s) (list 'cond (append (list e) s))) (when T (+ 1 2)))",
		"3");
	test_output(
		"(progn (defmacro when (e . s) (list 'cond (append (list e) s))) (when T (print kok) (print (setq b 2))))",
		"KOK\n2\n");
	test_output(
		"(progn (defmacro when (e . s) (list 'cond (append (list e) (print s)))) (when T q w e))",
		"(Q W E)\n");


	simple_core_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macroexpand '(macrocdr (q w e r) 2)))",
		"A");
	test_output(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macroexpand '(macrocdr (q w e r) 2)))",
		"(E R)\n");
	simple_core_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macroexpand-1 '(macrocdr (q w e r) 2)))",
		"(MACROCDR (W E R) 1)");
	simple_core_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macroexpand-1 (macroexpand-1 '(macrocdr (q w e r) 2))))",
		"(MACROCDR (E R) 0)");  //вроде скобки на месте

	simple_core_assert("(macroexpand)", nil_str);
	simple_core_assert("(macroexpand '(q w e))", "(Q W E)");
	simple_core_assert("(macroexpand 1)", "1");
	simple_core_assert("(macroexpand-1)", nil_str);
	simple_core_assert("(macroexpand-1 '(q w e))", "(Q W E)");
	simple_core_assert("(macroexpand-1 1)", "1");
}

void xz_chto_za_categoria() {
	simple_core_assert_several(
		({
			"(SETQ STACK-LIST '(A B C D E F))",
			"(POP STACK-LIST)",
			"(POP STACK-LIST)",
			"(POP STACK-LIST)",
			"STACK-LIST"
		}),
		({
			"(A B C D E F)",
			"A",
			"B",
			"C",
			"(D E F)"
		})
	);
	simple_core_assert_several(
		({
			"(SETQ STACK-LIST NIL)",
			"(PUSH 'A STACK-LIST)",
			"(PUSH 'B STACK-LIST)",
			"(PUSH 'C STACK-LIST)",
			"STACK-LIST"
		}),
		({
			"NIL",
			"(A)",
			"(B A)",
			"(C B A)",
			"(C B A)"
		})
	);
}

void test_outs(NATests::Tester& tester){
	bool fail = false;
	for (const auto& result : tester.execute()) {
		cout << result << endl;
		if (result.fail) fail = true;
	}
	if (fail) exit(1);
}

void test_bifuncs() {
	//Дебаг
	#ifdef _DEBUG
		// true - асинхронно, false - последовательно
		NATests::Tester tester(true);
	#endif

	//релиз (в нём лучше без асинхронных)
	#ifdef NDEBUG
		// true - асинхронно, false - последовательно
		// Асинхронные тесты на релизе не поддерживаются (только если задефайнить TREAT_SAFE_MEMORY_CONTROL в LispLibrary)
			NATests::Tester tester(false);
	#endif

	//ExceptionCaught -- если задефайнено -- ловятся все исключения, выполняются тесты с проверкой на succses/fail
			//(дефайнить в настройках проекта (2х))

	#ifdef EX_CATCH
		#define RUN(t, f) NATests_RUN_TEST_catch_ex(t,f)
	#endif

	#ifndef EX_CATCH
		#define RUN(t, f) NATests_RUN_TEST_nocatch_ex(t,f)
	#endif

	RUN(tester, test_eval_base);
	RUN(tester, arifm);
	RUN(tester, logic);
	RUN(tester, lists);
	RUN(tester, pred);
	RUN(tester, usl);
	RUN(tester, calcfun);
	RUN(tester, control_calc);
	RUN(tester, io);
	RUN(tester, id_repr);
	RUN(tester, id_func);
	RUN(tester, dotpairs);
	RUN(tester, macros);
	RUN(tester, xz_chto_za_categoria);

	test_outs(tester);
}