#include "BIFuncsTest.h"
#include "test_runner.h"
#include "Core.h"
#include "PredLispFuncs.h"
#include "StdCoreInputMemoryStream.h"
#include <sstream>
#include <fstream>

#include "Tester.h"
using namespace CoreData;
using std::istringstream;
using std::ostringstream;

//загрузка файла с пердфанками в оперативу
static const StdCoreInputMemoryStream predfuncs_mem_stream_global = []() {
	std::ifstream f(predfuncs_filename);
	return StdCoreInputMemoryStream(f, stream_read_mode::s_expression);
}();

//Тест со сравнением результата
#define simple_core_assert(input, output)\
{\
	StdCoreInputMemoryStream predfuncs_mem_stream = predfuncs_mem_stream_global;\
	auto [core, streams] = make_core_w_custom_predfuncs_and_empty_streams(predfuncs_mem_stream);\
	auto [result_reason, result] = core.execute_one(input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success);\
	ASSERT_EQUAL(result, output);\
}

//Тест без сравнения результата (проверка на success/fail)
#define simple_core_assert_reason(input, reason)\
{\
	StdCoreInputMemoryStream predfuncs_mem_stream = predfuncs_mem_stream_global;\
	auto [core, streams] = make_core_w_custom_predfuncs_and_empty_streams(predfuncs_mem_stream);\
	auto [result_reason, result] = core.execute_one(input);\
	ASSERT_EQUAL(result_reason,reason);\
}

//тест вывода
#define test_output(input, output)\
{\
	StdCoreInputMemoryStream predfuncs_mem_stream = predfuncs_mem_stream_global;\
	auto [core, streams] = make_core_w_custom_predfuncs_and_empty_streams(predfuncs_mem_stream); \
	auto [result_reason, result] = core.execute_one(input); \
	ASSERT_EQUAL(result_reason, Core::result_type::success); \
	ASSERT_EQUAL(streams->out.str(), output);\
}

//тест ввода
#define test_input(input,stream_input, output)\
{\
	StdCoreInputMemoryStream predfuncs_mem_stream = predfuncs_mem_stream_global;\
	istringstream in (stream_input);\
	ostringstream out;\
	auto core = make_core_w_custom_predfuncs(predfuncs_mem_stream, in, stream_read_mode::new_string, out);\
	auto [result_reason, result] = core.execute_one(input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success);\
	ASSERT_EQUAL(result, output);\
}

//тест ввода+вывода+результат
#define test_io_result(input, stream_input, stream_output, output)\
{\
	StdCoreInputMemoryStream predfuncs_mem_stream = predfuncs_mem_stream_global;\
	istringstream in (stream_input);\
	ostringstream out;\
	auto core = make_core_w_custom_predfuncs(predfuncs_mem_stream,in, stream_read_mode::new_string, out);\
	auto [result_reason, result] = core.execute_one(input);\
	ASSERT_EQUAL(result_reason, Core::result_type::success);\
	ASSERT_EQUAL(result, output);\
	ASSERT_EQUAL(out.str(), stream_output);\
}

//тест ввода+причина
#define test_input_reason(input, stream_input, reason)\
{\
	StdCoreInputMemoryStream predfuncs_mem_stream = predfuncs_mem_stream_global;\
	istringstream in (stream_input);\
	ostringstream out;\
	auto core = make_core_w_custom_predfuncs(predfuncs_mem_stream,in, stream_read_mode::new_string, out);\
	auto [result_reason, result] = core.execute_one(input);\
	ASSERT_EQUAL(result_reason, reason);\
}


//пример теста
void test_eval_base() {
	simple_core_assert("A", "A");
	simple_core_assert("1", "1");
	simple_core_assert("-1", "-1");
	simple_core_assert("+1", "1");
	simple_core_assert("long_text_With_some_shit1234567890", "LONG_TEXT_WITH_SOME_SHIT1234567890");
	simple_core_assert("((lambda (x) x) 1)", "1");
	simple_core_assert("((nlambda (x) x) a)", "A");
	simple_core_assert("((nlambda (x) x) 1)", "1");
	simple_core_assert("(if a kok nekok)", "KOK");
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
	simple_core_assert("(< 2 3)",T_str);
	simple_core_assert("(> 2 3)",nil_str);
	simple_core_assert("(< 2 2.0)",nil_str);
	simple_core_assert("(> 2 2.0)",nil_str);
	simple_core_assert("(<= 2 2.0)",T_str);
	simple_core_assert("(>= 2 2.0)",T_str);
	simple_core_assert("(= 2 2.0)",T_str);
	simple_core_assert("(< 2 2.5)",T_str);
	simple_core_assert("(> 2 1.5)",T_str);

	simple_core_assert("(>)",T_str);
	simple_core_assert("(<)",T_str);
	simple_core_assert("(>=)",T_str);
	simple_core_assert("(<=)",T_str);
	simple_core_assert("(=)",T_str);

	simple_core_assert("(> 1)",T_str);
	simple_core_assert("(< 1)",T_str);
	simple_core_assert("(>= 1)",T_str);
	simple_core_assert("(<= 1)",T_str);
	simple_core_assert("(= 1)",T_str);

	simple_core_assert("(getd (defun kok (x) (* x x)))","(LAMBDA (X) (* X X))");
	simple_core_assert("(getd)",nil_str);

	simple_core_assert("(eq 2 2)",T_str);
	simple_core_assert("(eq 2.0 2)",T_str);
	simple_core_assert("(eq 2 2.5)",nil_str);
	simple_core_assert("(eq aa aa)",T_str);
	simple_core_assert("(progn (setq a 2) (setq b 2) (eq a b))",T_str);
	simple_core_assert("(eq 2 a)",nil_str);
	simple_core_assert("(eq a 2)",nil_str);
	simple_core_assert("(eq '(1 1) '(1 1))",nil_str);
	simple_core_assert("(eq '(1 1) a)",nil_str);

	simple_core_assert("(equal 2 2)",T_str);
	simple_core_assert("(equal 2.0 2)",T_str);
	simple_core_assert("(equal 2 2.5)",nil_str);
	simple_core_assert("(equal aa aa)",T_str);
	simple_core_assert("(progn (setq a 2) (setq b 2) (equal a b))",T_str);
	simple_core_assert("(equal 2 a)",nil_str);
	simple_core_assert("(equal a 2)",nil_str);
	simple_core_assert("(equal '(1 1) '(1 1))",T_str);
	simple_core_assert("(equal '(1 1) '(1 2))",nil_str);
	simple_core_assert("(equal '(1 1) a)",nil_str);
}

//списковые функции
void list() {
	simple_core_assert("(car (quote (1 2 3)))","1");
	simple_core_assert("(car (quote ((1 1) 2 3)))","(1 1)");
	simple_core_assert("(cdr (quote (1 2 3)))","(2 3)");
	simple_core_assert("(cdr (quote ((1 1) 2 3)))", "(2 3)");

	simple_core_assert("(append (quote (1 1)) (quote (2 2)) (quote (3 3)))", "(1 1 2 2 3 3)");
	simple_core_assert("(append 2 (quote (1 1)) (quote (2 2)) (quote (3 3)))", "(1 1 2 2 3 3)");
	simple_core_assert("(append (quote (1 1)) (quote (2 (4 4) 2)) (quote (3 3)))","(1 1 2 (4 4) 2 3 3)");
	simple_core_assert("(append (quote ()) (quote (1 2)))","(1 2)");

	simple_core_assert("(cons a (quote (b)))","(A B)");
	simple_core_assert("(cons (quote (a)) (quote (b)))", "((A) B)");
	simple_core_assert("(list a b c)","(A B C)");
	simple_core_assert("(list a (quote b) c)", "(A B C)");
	simple_core_assert("(list a (quote (s d)) c)","(A (S D) C)");
	simple_core_assert("(list (+ 1 2) (quote (s d)) c)","(3 (S D) C)");
	simple_core_assert("(list)",nil_str);
	simple_core_assert("(cons)", "(NIL)");
	simple_core_assert("(append)",nil_str);
	simple_core_assert("(car)",nil_str);
	simple_core_assert("(cdr)", nil_str);
	simple_core_assert("(last)",nil_str);
	simple_core_assert("(last '())",nil_str);
	simple_core_assert("(last '(1 2 3))","(3)");
	simple_core_assert("(last '(1 2 3 (3 4)))", "((3 4))");
	simple_core_assert("(last (append '(1 2 3) '(3 2 1)))","(1)");
	simple_core_assert("(length '(1 2 3))", "3");
	simple_core_assert("(length (append '(1 2 3) '(1 2 3)))","6");
	//simple_core_assert("(length ace)","3");
	//simple_core_assert("(length (fib2 100))","5");    //так-то хз
}

//предикаты
void pred() {
	simple_core_assert("(null (list))",T_str);
	simple_core_assert("(null '())", T_str);
	//simple_core_assert("(null `())",T_str); //косой апостроф,
	simple_core_assert("(null '(0))",nil_str);
	simple_core_assert("(numberp 26)",T_str);
	simple_core_assert("(numberp 26.5)",T_str);
	simple_core_assert("(numberp -26.5)",T_str);
	simple_core_assert("(numberp 'a)",nil_str);
	simple_core_assert("(numberp '(1 1))",nil_str);
	simple_core_assert("(numberp (car '(2 a)))",T_str);
	simple_core_assert("(symbolp 26)",nil_str);
	simple_core_assert("(symbolp 'a)",T_str);
	simple_core_assert("(symbolp '(a a))",nil_str);
	simple_core_assert("(symbolp (car '(a 2)))",T_str);
	simple_core_assert("(listp 26)",nil_str);
	simple_core_assert("(listp 'a)",nil_str);
	simple_core_assert("(listp '(a a))",T_str);
	simple_core_assert("(listp (car '((1 a 2) a 2)))",T_str);
	simple_core_assert("(atom 26)",T_str);
	simple_core_assert("(atom 'a)",T_str);
	simple_core_assert("(atom '(a a))",nil_str);
	simple_core_assert("(atom (car '(a 2)))", T_str);
	simple_core_assert("(null)",T_str);
	simple_core_assert("(numberp)",nil_str);
	simple_core_assert("(listp)",T_str);
	simple_core_assert("(atom)",T_str);
	simple_core_assert("(symbolp)", T_str);
}

//условные функции
void usl() {
	simple_core_assert("(cond ((null '()) 'a) ((numberp 26) 'b) ((listp '(2)) 'c))","A");
	simple_core_assert("(cond ((null '(1)) 'a) ((numberp 26) 'b) ((listp '(2)) 'c))", "B");
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '(2)) (car '(c 2))))","C");
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c))",nil_str);
	simple_core_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (T 'da))","DA");
	simple_core_assert("(cond)",nil_str);
	simple_core_assert("(if T 'a 'b)","A");
	simple_core_assert("(if NIL 'a 'b)","B");
	simple_core_assert("(if (null '()) 'a 'b)","A");
	simple_core_assert("(if (null '(2)) 'a 'b)","B");
	simple_core_assert("(if (null '()) 'a)","A");
	simple_core_assert("(if (null '(2)) 'a)",nil_str);
	simple_core_assert("(if (null '()))",nil_str);
	simple_core_assert("(if (null '()) (+ 1 2) 'b)","3");
	simple_core_assert("(if (null '(2)) (+ 1 2) (+ 3 4))","7");
	simple_core_assert("(if)",nil_str);
}

//функции вычислений
void calcfun() {
	simple_core_assert("(eval)",nil_str);
	simple_core_assert("(eval a)","A");
	simple_core_assert("(progn (setq a 2) (eval a))","2");
	simple_core_assert("(progn (setq a 2) (setq b 'a) (eval b))","2");
	simple_core_assert("(progn (setq a 2) (setq b 'a) (eval 'b))","A");
	simple_core_assert("(progn (setq a 2) ((null nil) nil) (eval 'b))",nil_str);
	simple_core_assert("(progn (setq a 2) ((null T) nil) (eval 'b))","B");
	simple_core_assert("(eval '(+ 1 2))","3");
	simple_core_assert("(eval ((lambda (x) (* x x)) 3))", "9");
	//simple_core_assert("(eval ''''((lambda (x) (* x x)) 3))","9");    //мнение фориса
	simple_core_assert("(quote a)","A");
	simple_core_assert("'(quote a)","(QUOTE A)");
	simple_core_assert("'''a", "(QUOTE (QUOTE A))");
	//simple_core_assert("`a", "A");
	simple_core_assert("(quote)",nil_str);

	simple_core_assert_reason("(apply)", Core::result_type::fail);
	simple_core_assert_reason("(apply kok)", Core::result_type::fail);

	simple_core_assert("(apply '+ '(1 2 3))", "6");
	simple_core_assert("(apply 'append '((1 2) (3 4)))", "(1 2 3 4)");
	simple_core_assert("(apply '(lambda (X A) (* X A)) '(3 4))","12");
	simple_core_assert("(apply '(lambda (X L) (* X (car L))) '(3 (4 5 6)))","12");
}

//управление вычислениями
void control_calc() {
	simple_core_assert("(defun kek (x) (* x x))","KEK");
	simple_core_assert("(progn (defun kek (x) (* x x)) (kek 3))","9");
	simple_core_assert("(progn (defun kek (x a) (* x x) a) (kek 3))",nil_str);
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
	simple_core_assert("(progn (setq a 2) (setq b 'a) (set b 3) a)","3");
	simple_core_assert("(progn (setq L '(1 2 3 4)) (loop ((null L) 'da) (setq L (cdr L))))","DA");
	simple_core_assert("(progn (setq N 0) (setq L '(1 2 3 4)) (loop ((null L) N) (setq N (+ 1 N)) (setq L (cdr L))))", "4");
	simple_core_assert("(progn (setq L '(1 a 2 q kek)) (loop ((null(cdr L)) (car L)) (setq L(cdr L))))","KEK");
}

//ввод/вывод
void io() {
	simple_core_assert("(print)",nil_str);  //формат вывода под вопросом (каким?)
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
	test_io_result(std::string("(progn (setq ") + read_up_case_str +  " nil) (print (read)))", "aBc1", "aBc1\n", "aBc1");

	//тест | и "
	simple_core_assert("|AbC1|", "AbC1");
	simple_core_assert("\"AbC1\"", "AbC1");

	//тест на анализ ввода через read
	//test_io_result("(eval '((lambda x (print (read))) nil))", "1b", std::string(nil_str) + "\n", nil_str);
	//тест на анализ ввода
	simple_core_assert_reason("1c2", Core::result_type::fail);
	
}

void test_outs(NATests::Tester& tester){
	bool fail = false;
	for (const auto& result : tester.execute()) {
		std::cout << result << std::endl;
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
			NATests::Tester tester(false);
	#endif

	NATests_RUN_TEST(tester, test_eval_base);
	NATests_RUN_TEST(tester, arifm);
	NATests_RUN_TEST(tester, logic);
	NATests_RUN_TEST(tester, list);
	NATests_RUN_TEST(tester, pred);
	NATests_RUN_TEST(tester, usl);
	NATests_RUN_TEST(tester, calcfun);
	NATests_RUN_TEST(tester, control_calc);
	NATests_RUN_TEST(tester, io);

	test_outs(tester);
}