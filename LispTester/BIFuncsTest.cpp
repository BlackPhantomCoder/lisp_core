#include "BIFuncsTest.h"
#include "test_runner.h"
#include "Core.h"
#include "PredLispFuncs.h"
#include "StdCoreInputMemoryStream.h"
#include "StdCoreInputStream.h"
#include "Tester.h"


#include <vector>
#include <sstream>
#include <fstream>

using namespace CoreData;
using namespace std;


//количество потоков для асинхронных тестов (оптимально - по половине ядер)
constexpr auto treads_count = 6;

//Дебаг
#ifdef _DEBUG
	// true - асинхронно, false - последовательно
using tester_type = NATests::Tester<true>;
#endif

//релиз (в нём лучше без асинхронных)
#ifdef NDEBUG
	// true - асинхронно, false - последовательно
	// Асинхронные тесты на релизе не поддерживаются (только если задефайнить TREAT_SAFE_MEMORY_CONTROL в LispLibrary)
using tester_type = NATests::Tester<false>;
#endif


//ExceptionCaught -- если задефайнено -- ловятся все исключения, выполняются тесты с проверкой на succses/fail
		//(дефайнить в настройках проекта (2х))
#ifdef EX_CATCH
constexpr auto exceptions = false;
#endif

#ifndef EX_CATCH
constexpr auto exceptions = true;
#endif



// тестер
auto tester = tester_type();
// хранит текущий добавляемый тест
auto tests_name = ""s;


// добавляет тест в тестер
#define ADD_TEST(test) {\
	tests_name = #test;\
	test();\
}


vector<string> split_by_lines(string_view str) {
	vector<string> result;
	size_t prev_pos = 0;
	auto pos = str.find('\n');
	while(pos != str.npos){
		result.push_back(string{ str.substr(0, pos) });
		str.remove_prefix(pos + 1);
		pos = str.find('\n');
	}
	if (!empty(str)) {
		result.push_back(string{ str });
	}
	return result;
}

// сохранение состояния с предфанками в переменной
auto default_state = ""s;

// функция для сохранения состояния с предфанками в переменной
auto default_state_func = []() {
	auto core = make_core_w_predfuncs();
	auto saved_state = ostringstream();
	core.save_state(saved_state);

	return saved_state.str();
};





// загрузка ядра из default_state
Core load_default_core() {

	////загрузка файла с пердфанками в оперативу
	//static const auto predfuncs_mem_stream_global = []() {
	//	ifstream f(predfuncs_filename);
	//	return StdCoreInputMemoryStream(f, stream_read_mode::s_expression);
	//}();

	//auto predfuncs_mem_stream = StdCoreInputMemoryStream(predfuncs_mem_stream_global);
	//return make_core_w_custom_predfuncs(predfuncs_mem_stream);
	auto saved_state = istringstream(default_state);
	return make_core_from_state(saved_state);
};





tuple<Core::result_type, string> assert_core_one(const string& input) {
	auto streams = ValStreamCESP(make_unique<istringstream>(input), stream_read_mode::new_string, make_unique<ostringstream>());
	auto core = load_default_core();
	
	auto reason = core.execute_one(streams);
	return { reason,  (reason == Core::result_type::success) ? split_by_lines(streams.os().str()).back() : "" };
}

tuple<Core::result_type, vector<string>> assert_core_all(vector<string>&& input) {
	auto is = StdCoreInputValArrayInput(move(input), stream_read_mode::new_string);
	auto os = ostringstream();
	auto output = StdCoreOutputStream(os);
	auto streams = RefCESP(is, output);

	auto core = load_default_core();
	auto reason = core.execute_all(streams);
	return { reason, (reason == Core::result_type::success) ? split_by_lines(os.str()) : vector<string>{} };
}

tuple<Core::result_type, string> assert_core_raw(const std::string& input) {
	auto streams = ValStreamCESP(make_unique<istringstream>(input), stream_read_mode::new_string, make_unique<ostringstream>());

	auto core = load_default_core();
	auto reason = core.execute_all(streams);
	return { reason,  streams.os().str() };
}





//Тест со сравнением результата
#define one_assert(input, output)\
{\
	auto fnc = []() -> void {\
		auto [result_reason, result] = assert_core_one(input);\
		ASSERT_EQUAL(result_reason, Core::result_type::success);\
		ASSERT_EQUAL(result, output);\
	};\
	tester.add_test<false>(fnc, tests_name);\
}

//Тест нескольких команд со сравнением результата (массив)
#define several_assert(input, output)\
{\
	auto fnc = []() -> void {\
		auto [result_reason, result] = assert_core_all(vector<string> input);\
		ASSERT_EQUAL(result_reason, Core::result_type::success);\
		ASSERT_EQUAL(result, vector<string>output);\
	};\
	tester.add_test<false>(fnc, tests_name);\
}

//Тест потока ввода (полностью) со сравнением результата (полная строка вывода)
#define raw_assert(input, output)\
{\
	auto fnc = []() -> void {\
		auto [result_reason, result] = assert_core_raw(input);\
		ASSERT_EQUAL(result_reason, Core::result_type::success);\
		ASSERT_EQUAL(result, output);\
	};\
	tester.add_test<false>(fnc, tests_name);\
}

//Тест со сравнением результата только последней команды
#define several_assert_last_val(input, output)\
{\
	auto fnc = []() -> void {\
		auto [result_reason, result] = assert_core_all(vector<string> input);\
		ASSERT_EQUAL(result_reason, Core::result_type::success);\
		ASSERT_EQUAL(result.back(), output);\
	};\
	tester.add_test<false>(fnc, tests_name);\
}


//Тест потока ввода (полностью) без сравнения результата (проверка на success/fail) (если ExceptionCaught не задефайнено - не выполняются)
#ifdef EX_CATCH
#define raw_assert_reason(input, output)\
{\
	auto fnc = []() -> void {\
			auto [result_reason, result] = assert_core_raw(input);\
			ASSERT_EQUAL(result_reason, Core::result_type::success);\
	};\
	tester.add_test<false>(fnc, tests_name);\
}
#endif
#ifndef EX_CATCH
	#define raw_assert_reason(input, reason)
#endif


//Тест без сравнения результата (проверка на success/fail) (если ExceptionCaught не задефайнено - не выполняются)
#ifdef EX_CATCH
#define one_assert_reason(input, reason)\
{\
	auto fnc = []() -> void {\
		auto [result_reason, result] = assert_core_one(input);\
		ASSERT_EQUAL(result_reason,reason);\
	};\
	tester.add_test<false>(fnc, tests_name);\
}
#endif
#ifndef EX_CATCH
	#define one_assert_reason(input, reason)
#endif

//Тест без сравнения результата (проверка на success/fail) (если ExceptionCaught не задефайнено - не выполняются)
#ifdef EX_CATCH
#define several_assert_reason(input, reason)\
{\
	auto fnc = []() -> void {\
		auto [result_reason, result] = assert_core_all(vector<string> input);\
		ASSERT_EQUAL(result_reason,reason);\
	};\
	tester.add_test<false>(fnc, tests_name);\
}
#endif
#ifndef EX_CATCH
	#define several_assert_reason(input, reason)
#endif


//пример теста
void test_eval_base() {
	one_assert("A", "A");
	one_assert("1", "1");
	one_assert("-1", "-1");
	one_assert("+1", "+1");
	//one_assert("(symbolp +1)", "T");
	one_assert("long_text_With_some_shit1234567890", "LONG_TEXT_WITH_SOME_SHIT1234567890");
	one_assert("((lambda (x) x) 1)", "1");
	several_assert_last_val(({ "(setq a b)", "(setq b 1)", "((lambda (x) x) a)" }), "B");
	several_assert_last_val(({ "(setq a nil)", "(if a t nil)" }), "NIL");
	several_assert_last_val(({ "(setq a b)", "(setq b nil)", "(if a t nil)" }), "T");
	several_assert_last_val(({ "(setq a b)", "(setq b nil)", "(if b t nil)" }), "NIL");
	one_assert("(if a kok nekok)", "KOK");
	one_assert("(not nil)", "T");
	one_assert("(not T)", "NIL");
	several_assert_last_val(({ "(setq a b)", "(setq b nil)", "(not a)" }), "NIL");
	one_assert("((lambda (x) (cdr x)) '(1 2))", "(2)");
	one_assert_reason("((nlambda (x) x) a)", Core::result_type::fail);
	one_assert("(copy-tree '(1))", "(1)");
	one_assert("(copy-tree '(1 2))", "(1 2)");
	one_assert("(copy-tree '(1 2 . 3))", "(1 2 . 3)");
	one_assert("(copy-tree '((1 2 . 3) (4 . 5)))", "((1 2 . 3) (4 . 5))");
	one_assert("(copy-tree '((1 2 3) (4 (5 (6) 7) 8) . (10 (5))))", "((1 2 3) (4 (5 (6) 7) 8) 10 (5))");
	one_assert("((lambda (x '(n) y) (list x n y)) 1 2 3)", "(1 N 3)");
}

//арифметические функции
void arifm() {
	one_assert("(- 3)", "-3");
	one_assert("(- 10.5 3)", "7.5");
	one_assert("(/ 45.3 3)", "15.1");
	one_assert("(+ 10.5 3)", "13.5");
	one_assert("(* 10.5 3)", "31.5");
	one_assert("(- 10 3)", "7");
	one_assert("(+ 10 3)", "13");
	one_assert("(* 10 3)", "30");
	one_assert("(/ 45 3)", "15");
	one_assert("(+ 10.3 (* -1.0 3) -0.1 (/ 5 2.5))", "9.2");
	one_assert("(* 3 3 3 3)", "81");
	one_assert("(/ 81 3 3 3)", "3");
	one_assert("(+)", "0");
	one_assert("(*)", "1");
}

//логические функции
void logic() {
	one_assert("(< 2 3)", T_str);
	one_assert("(> 2 3)", nil_str);
	one_assert("(< 2 2.0)", nil_str);
	one_assert("(> 2 2.0)", nil_str);
	one_assert("(<= 2 2.0)", T_str);
	one_assert("(>= 2 2.0)", T_str);
	one_assert("(= 2 2.0)", T_str);
	one_assert("(< 2 2.5)", T_str);
	one_assert("(> 2 1.5)", T_str);

	one_assert("(>)", T_str);
	one_assert("(<)", T_str);
	one_assert("(>=)", T_str);
	one_assert("(<=)", T_str);
	one_assert("(=)", T_str);

	one_assert("(> 1)", T_str);
	one_assert("(< 1)", T_str);
	one_assert("(>= 1)", T_str);
	one_assert("(<= 1)", T_str);
	one_assert("(= 1)", T_str);

	one_assert("(getd (defun kok (x) (* x x)))", "(LAMBDA (X) (* X X))");
	one_assert("(getd)", nil_str);

	one_assert("(eq 2 2)", T_str);
	one_assert("(eq 2.0 2)", T_str);
	one_assert("(eq 2 2.5)", nil_str);
	one_assert("(eq aa aa)", T_str);
	one_assert("(progn (setq a 2) (setq b 2) (eq a b))", T_str);
	one_assert("(eq 2 a)", nil_str);
	one_assert("(eq a 2)", nil_str);
	one_assert("(eq '(1 1) '(1 1))", nil_str);
	one_assert("(eq '(1 1) a)", nil_str);
	one_assert("(eq '() nil)", T_str);
	one_assert("(eq)", T_str);
	one_assert("(eq nil nil)", T_str);

	one_assert("(equal 2 2)", T_str);
	one_assert("(equal 2.0 2)", T_str);
	one_assert("(equal 2 2.5)", nil_str);
	one_assert("(equal aa aa)", T_str);
	one_assert("(progn (setq a 2) (setq b 2) (equal a b))", T_str);
	one_assert("(equal 2 a)", nil_str);
	one_assert("(equal a 2)", nil_str);
	one_assert("(equal '(1 1) '(1 1))", T_str);
	one_assert("(equal '(1 1) '(1 2))", nil_str);
	one_assert("(equal '(1 1) a)", nil_str);
	one_assert("(equal '() nil)", T_str);
	one_assert("(equal nil nil)", T_str);
}

//списковые функции
void lists() {
	one_assert("(car (quote (1 2 3)))", "1");
	one_assert("(car (quote ((1 1) 2 3)))", "(1 1)");
	one_assert("(cdr (quote (1 2 3)))", "(2 3)");
	one_assert("(cdr (quote ((1 1) 2 3)))", "(2 3)");

	one_assert("(append (quote (1 1)) (quote (2 2)) (quote (3 3)))", "(1 1 2 2 3 3)");
	one_assert("(append 2 (quote (1 1)) (quote (2 2)) (quote (3 3)))", "(1 1 2 2 3 3)");
	one_assert("(append (quote (1 1)) (quote (2 (4 4) 2)) (quote (3 3)))", "(1 1 2 (4 4) 2 3 3)");
	one_assert("(append (quote ()) (quote (1 2)))", "(1 2)");

	one_assert("(cons a (quote (b)))", "(A B)");
	one_assert("(cons (quote (a)) (quote (b)))", "((A) B)");
	one_assert("(list a b c)", "(A B C)");
	one_assert("(list a (quote b) c)", "(A B C)");
	one_assert("(list a (quote (s d)) c)", "(A (S D) C)");
	one_assert("(list (+ 1 2) (quote (s d)) c)", "(3 (S D) C)");
	one_assert("(list)", nil_str);
	one_assert("(cons)", "(NIL)");
	one_assert("(append)", nil_str);
	one_assert("(car)", nil_str);
	one_assert("(cdr)", nil_str);
	one_assert("(last)", nil_str);
	one_assert("(last '())", nil_str);
	one_assert("(last '(1 2 3))", "(3)");
	one_assert("(last '(1 2 3 (3 4)))", "((3 4))");
	one_assert("(last (append '(1 2 3) '(3 2 1)))", "(1)");
	one_assert("(length '(1 2 3))", "3");
	one_assert("(length (append '(1 2 3) '(1 2 3)))", "6");
	//one_assert("(length ace)","3");
	//one_assert("(length (fib2 100))","5");    //так-то хз
}

//предикаты
void pred() {
	one_assert("(null (list))", T_str);
	one_assert("(null '())", T_str);
	one_assert("(null nil)", T_str);
	one_assert("(null `())",T_str); //косой апостроф,
	one_assert("(null '(0))", nil_str);
	one_assert("(numberp 26)", T_str);
	one_assert("(numberp 26.5)", T_str);
	one_assert("(numberp -26.5)", T_str);
	one_assert("(numberp 'a)", nil_str);
	one_assert("(numberp '(1 1))", nil_str);
	one_assert("(numberp (car '(2 a)))", T_str);
	one_assert("(symbolp 26)", nil_str);
	one_assert("(symbolp 'a)", T_str);
	one_assert("(symbolp '(a a))", nil_str);
	one_assert("(symbolp (car '(a 2)))", T_str);
	one_assert("(listp 26)", nil_str);
	one_assert("(listp 'a)", nil_str);
	one_assert("(listp '(a a))", T_str);
	one_assert("(listp (car '((1 a 2) a 2)))", T_str);
	one_assert("(atom 26)", T_str);
	one_assert("(atom 'a)", T_str);
	one_assert("(atom '(a a))", nil_str);
	one_assert("(atom (car '(a 2)))", T_str);
	one_assert("(null)", T_str);
	one_assert("(numberp)", nil_str);
	one_assert("(listp)", T_str);
	one_assert("(atom)", T_str);
	one_assert("(symbolp)", T_str);
}

//условные функции
void usl() {
	one_assert("(cond ((null '()) 'a) ((numberp 26) 'b) ((listp '(2)) 'c))", "A");
	one_assert("(cond ((null '(1)) 'a) ((numberp 26) 'b) ((listp '(2)) 'c))", "B");
	one_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '(2)) (car '(c 2))))", "C");
	one_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c))", nil_str);
	one_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (T 'da))", "DA");
	one_assert("(cond)", nil_str);
	one_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (a 'da))", "DA");
	one_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (1 'da))", "DA");   //возможно, форис делает \1
	one_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) (nil 'da))", nil_str);
	one_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) ('(1 2) 'da))", "DA");
	one_assert("(cond ((null '(2)) 'a) ((numberp r) 'b) ((listp '2) 'c) ((cons a b) 'da))", "DA");

	one_assert("(if T 'a 'b)", "A");
	one_assert("(if NIL 'a 'b)", "B");
	one_assert("(if (null '()) 'a 'b)", "A");
	one_assert("(if (null '(2)) 'a 'b)", "B");
	one_assert("(if (null '()) 'a)", "A");
	one_assert("(if (null '(2)) 'a)", nil_str);
	one_assert("(if (null '()))", nil_str);
	one_assert("(if (null '()) (+ 1 2) 'b)", "3");
	one_assert("(if (null '(2)) (+ 1 2) (+ 3 4))", "7");
	one_assert("(if)", nil_str);
	one_assert("(if '(1 2) 'a 'b)", "A");
	one_assert("(if '(1 . 2) 'a 'b)", "A");
	one_assert("(if 1 'a 'b)", "A");
}

//функции вычислений
void calcfun() {
	one_assert("(eval)", nil_str);
	one_assert("(eval a)", "A");
	one_assert("(progn (setq a 2) (eval a))", "2");
	one_assert("(progn (setq a 2) (setq b 'a) (eval b))", "2");
	one_assert("(progn (setq a 2) (setq b 'a) (eval 'b))", "A");
	one_assert("(progn (setq a 2) ((null nil) nil) (eval 'b))", nil_str);
	one_assert("(progn (setq a 2) ((null T) nil) (eval 'b))", "B");
	one_assert("(eval '(+ 1 2))", "3");
	one_assert("(eval ((lambda (x) (* x x)) 3))", "9");
	one_assert("(eval ````((lambda (x) (* x x)) 3))","9");
	one_assert("(quote a)", "A");
	one_assert("'(quote a)", "(QUOTE A)");
	one_assert("'''a", "(QUOTE (QUOTE A))");
	one_assert("`a", "A");
	one_assert("(quote)", nil_str);

	one_assert_reason("(apply)", Core::result_type::fail);
	one_assert_reason("(apply kok)", Core::result_type::fail);

	one_assert("(apply '+ '(1 2 3))", "6");
	one_assert("(apply 'append '((1 2) (3 4)))", "(1 2 3 4)");
	one_assert("(apply '(lambda (X A) (* X A)) '(3 4))", "12");
	one_assert("(apply '(lambda (X L) (* X (car L))) '(3 (4 5 6)))", "12");
}

//управление вычислениями
void control_calc() {
	one_assert("(defun kek (x) (* x x))","KEK");
	one_assert("(progn (defun kek (x) (* x x)) (kek 3))","9");
	one_assert("(progn (defun kek (a 's d) d) (kek 1 2 3))", "3");
	one_assert("(progn (defun kek (a '(kok nekok) d) d) (kek 1 2 3))", "3");
	one_assert("(progn (defun kek (x a) (* x x) a) (kek 3))",nil_str);
	one_assert("(progn (defun kok () (setq x 1000) (loop ((< x 0) x) (setq x (- x 7)))) (kok))", "-1"); 
	one_assert("(defun)",nil_str);
	one_assert("(defun kek)",nil_str);
	one_assert("(defun kek (x))","KEK");
	one_assert("(progn)",nil_str);
	one_assert("(progn (+ 1 2) (+ 3 4))","7");
	one_assert("(progn (setq a 2) (+ 2 a) )","4");
	one_assert("(prog1)",nil_str);
	one_assert("(prog1 (+ 1 2) (+ 3 4))","3");
	one_assert("(prog1 (setq a 2) (+ 2 a) (setq a 3))","2");
	one_assert("(setq)",nil_str);
	one_assert("(setq a)",nil_str);
	one_assert("(setq a 2)","2");
	one_assert("(eval (setq a 2))","2");
	one_assert("(progn (setq a 2) a)","2");
	one_assert("(set)",nil_str);
	one_assert("(set a)",nil_str);
	raw_assert("(progn (setq a 1 b 2) (print a) (print b))", "1\n2\n2\n");

	one_assert("(progn (setq a 2) (setq b 'a) (set b 3) a)", "3");
	one_assert("(progn (setq L '(1 2 3 4)) (loop ((null L) 'da) (setq L (cdr L))))", "DA");
	one_assert("(progn (setq N 0) (setq L '(1 2 3 4)) (loop ((null L) N) (setq N (+ 1 N)) (setq L (cdr L))))", "4");
	one_assert("(progn (setq L '(1 a 2 q kek)) (loop ((null(cdr L)) (car L)) (setq L(cdr L))))","KEK");
	one_assert(
		"(progn (setq lst '(1 (2 3) (2 . 3) a nil 4 5)) (loop ((not (car lst)) (cdr lst)) (print lst) (setq lst (cdr lst))))",
		"(4 5)");
	several_assert_last_val(
		({
			"(setq L '(1 a 2 q kek))",
			"(loop ( (null (cdr L)) (car L)) (setq L (cdr L)) )"
		}),
		"KEK"
	);
	several_assert_last_val(
		({
			"(setq lst '(1 (2 3) (2 . 3) a nil 4 5))",
			"(loop ((not (car lst)) (cdr lst)) (print lst) (setq lst (cdr lst)))"
		}),
		"(4 5)"
	);
	raw_assert(
		"(progn (setq lst '(1 (2 3) (2 . 3) a nil 4 5)) (loop ((not (car lst)) (cdr lst)) (print lst) (setq lst (cdr lst))))",
		"(1 (2 3) (2 . 3) A NIL 4 5)\n((2 3) (2 . 3) A NIL 4 5)\n((2 . 3) A NIL 4 5)\n(A NIL 4 5)\n(4 5)\n"
	);
}

//ввод/вывод
void io() {
	one_assert("(QUOTE ( 1 a b]", "(1 A B)");
	one_assert("'( 1 a b]", "(1 A B)");
	one_assert("'( (1 (a (b]", "((1 (A (B))))");
	one_assert_reason("]", Core::result_type::fail);

	one_assert("(print)",nil_str);  
	one_assert("(print a)","A");
	one_assert("(print (+ 1 2 3))", "6");

	raw_assert("(print 'А_ВООБЩЕ_хз_как_его_тестировать)", "А_ВООБЩЕ_ХЗ_КАК_ЕГО_ТЕСТИРОВАТЬ\nА_ВООБЩЕ_ХЗ_КАК_ЕГО_ТЕСТИРОВАТЬ\n");
	raw_assert("(setq a (read)) A", "A\n");
	raw_assert("(print (read)) A", "A\nA\n");


	//тест ввода-вывода без перевода в капс
	raw_assert(string("(progn (setq ") + read_up_case_str + " nil) (print (read))) aBc1", "aBc1\naBc1\n");

	//тест | и "
	one_assert("|AbC1|", "|AbC1|");
	one_assert("\"AbC1\"", "|AbC1|");

	//тест на анализ ввода через read
	raw_assert("(eval '((lambda x (print (read))) nil)) 1b", "1\n1\nB\n");
	//тест на анализ ввода
	one_assert("1c2", "1");

	one_assert("a\\a12", "|Aa12|");
	one_assert("|AAC1|", "AAC1");
	one_assert("|1AAC1|", "|1AAC1|");
	one_assert("\\ \\ \\ ", "|   |");

	//\(\)\ \'\\\;\#
	one_assert("\\(\\)\\ \\'\\\\\\;\\#", "|() '\\\\;#|");
	one_assert("A\\\\B.LSP", "|A\\\\B.LSP|");
	one_assert("\\++++", "++++");
	
}

//идентификаторы
void id_repr() {
	one_assert("\\ ", "| |");
	one_assert("\\1", "\\1");
	one_assert("\\1a", "|1A|");
	one_assert_reason("\\\\", Core::result_type::success);
	one_assert("\\\\", "\\\\");
	one_assert_reason("\\\\\\a", Core::result_type::success);
	one_assert("\\\\\\\\", "|\\\\\\\\|");						//надо согласовать 
	one_assert("\\a\\|", "|a\\||");							//с методичкой стр.44, последний абзац
	one_assert("(pack '(\\| \\|))", "|\\|\\||");				//
	one_assert("(pack '(\\| \\\\ \\|))", "|\\|\\\\\\||");

	one_assert(".", "\\.");
	one_assert(".qwe", ".QWE");
	one_assert(".12", ".12");
	one_assert_reason(",", Core::result_type::fail);    //возможно, стоит обратить внимание на причину фейла
	one_assert("\\,", "\\,");

	//здесь break (у ,) - не fail
	//raw_assert_reason("!@#,!##", Core::result_type::fail);

	one_assert("!@#\\,!@#", "|!@#,!@#|");

	//здесь ` же
	//one_assert("q\\q1\\1`~!@#$%^&*-_=+/\\;\\'\\\".\\,:", "|Qq11`~!@#$%^&*-_=+/;'\".,:|");

	one_assert("q1~!@#$%^&*-_=+/:", "Q1~!@#$%^&*-_=+/:");

	//здесь ` же
	//one_assert("a`", "A`");       //возможно, рофл из коммона (в 85 не загружен)
	//здесь ` же
	//one_assert("q1`~!@#$%^&*-_=+/:", "Q1`~!@#$%^&*-_=+/:");

	one_assert("\\a", "\\a");
	one_assert("\\A", "A");
	one_assert("||", "||");
	one_assert("\"\"", "||");
	one_assert("\\\"\\\"", "|\"\"|");
	one_assert("\"|\"", "\\|");
	one_assert(std::string("(progn (setq ") + read_up_case_str + " nil) \\a) ", "a");
	one_assert(std::string("(progn (setq ") + read_up_case_str + " nil) \\A) ", "A");

}

//функции над идентификаторами
void id_func() {
	one_assert("(numberp 345)", T_str);
	one_assert("(numberp \\345)", nil_str);
	one_assert("(numberp '\\345)", nil_str);
	one_assert("(symbolp '\\345)", T_str);
	one_assert("(atom '\\345)", T_str);
	one_assert("(listp '\\345)", nil_str);
	one_assert("(eq \\345 \\3\\4\\5)", T_str);
	one_assert("(equal \\345 \\3\\4\\5)", T_str);
	one_assert("(eq \\aa aa)", nil_str);
	one_assert("(equal \\aa aa)", nil_str);
	one_assert("(equal |AAC1| aac1)", T_str);
	one_assert("(equal q1 q\\1)", T_str);
	one_assert("(equal A \\A)", T_str);
	one_assert("(equal \\a \\a)", T_str);
	one_assert("(equal \\a\\a |aa|)", T_str);
	one_assert("(equal || \"\")", T_str);
	one_assert("(eq \"a12\" \"a12\")", T_str);     //методичка утверждает обратное (но там коммон)
	one_assert("(equal \"a12\" \"a12\")", T_str);
	one_assert("(progn (setq \\345 1) \\345)", "1");
	one_assert("(progn (setq |kok| 2) \\k\\o\\k)", "2");

	one_assert("(unpack kok)", "(K O K)");
	one_assert("(unpack |kok|)", "(\\k \\o \\k)");
	one_assert("(unpack q\\q1\\1)", "(Q \\q \\1 \\1)");
	one_assert("(unpack)", "(N I L)");
	one_assert("(unpack '(1 2 3))", nil_str);
	one_assert("(unpack 1)", "(\\1)");
	one_assert("(unpack 1.2)", "(\\1 \\. \\2)");
	one_assert("(unpack \"kok11\")", "(\\k \\o \\k \\1 \\1)");
	one_assert("(equal (unpack \\345) (unpack \\3\\4\\5))", T_str);
	one_assert("(numberp (cadr (unpack a1a)))", nil_str);
	one_assert("(pack)", "||");
	one_assert("(pack '(1 2 3))", "|123|");
	one_assert("(pack '(\\1 \\2 \\3))", "|123|");
	one_assert("(equal (pack '(1 2 3)) (pack '(\\1 \\2 \\3)))", T_str);
	one_assert("(pack '(kok nekok))", "KOKNEKOK");
	one_assert("(pack '(kok |kekw| nekok))", "|KOKkekwNEKOK|");
	one_assert("(pack '(kok \"kekw\" nekok))", "|KOKkekwNEKOK|");
	one_assert("(pack '((1 2) (3 4) 5))", "\\5");
	one_assert("(equal (pack '(1 2 3)) (pack '(123)))", T_str);

	one_assert("(cadr (unpack (pack '(a 2 a))))", "\\2");
	one_assert("(pack (unpack \\345))", "|345|");
	one_assert("(pack '(| |))", "| |");
	one_assert("(unpack |\\|\\||)", "(\\| \\|)");
}

//точечные пары
void dotpairs() {
	one_assert("'(a . b)", "(A . B)");
	one_assert("'(a.b)", "(A.B)");
	one_assert("(car '(a . b))", "A");
	one_assert("(cdr '(a . b))", "B");
	one_assert("(cdr '(c a . b))", "(A . B)");
	one_assert("(car '(a.b))", "A.B");
	one_assert("'(1 . (2 . (3 . nil)))", "(1 2 3)");

	one_assert("'(a . nil)", "(A)");
	one_assert("'(nil . b)", "(NIL . B)");
	one_assert("'(nil . nil)", "(NIL)");
	one_assert_reason("'(1 2 3 . 4)", Core::result_type::success);
	one_assert_reason("'(1 2 . 3 4)", Core::result_type::fail);

	one_assert("'(1 2 (3 . 4) 5 . 6)", "(1 2 (3 . 4) 5 . 6)");
	one_assert("(last '(1 2 (3 . 4) 5 . 6))", "(5 . 6)");
	one_assert("(caddr '(1 2 (3 . 4) 5 . 6))", "(3 . 4)");
	one_assert("(length '(1 2 (3 . 4) 5 . 6))", "4");
	one_assert("(length '(1 . 2))", "1");

	one_assert("(cons a b)", "(A . B)");
	one_assert("(cons c (cons a b))", "(C A . B)");
	one_assert("(car (cons a b))", "A");
	one_assert("(cdr (cons a b))", "B");
	one_assert("(cons '(a . b) c)", "((A . B) . C)");
	one_assert("(cons a (list b))", "(A B)");
	one_assert("(cons (list a) b)", "((A) . B)");
	one_assert("(append '(1 . 2) '(3 . 4))", "(1 3 . 4)");

	one_assert("(listp '(1 2 . 3))", T_str);
	one_assert("(listp '(2 . 3))", T_str);
	one_assert("(atom '(1 2 . 3))", nil_str);
	one_assert("(null '(1 2 . 3))", nil_str);
	one_assert("(null '(nil . 3))", nil_str);
	one_assert("(null '(1 2 . nil))", nil_str);
	one_assert("(null '(nil . nil))", nil_str);
	one_assert("(equal '(1 2 . nil) '(1 2))", T_str);
	one_assert("(eq '(1 2 . nil) '(1 2))", nil_str);

	one_assert("(progn (setq b 2) (setq a '(b . c)) (car a))", "B");
	one_assert("(progn (setq b 2) (setq a (cons b c)) (car a))", "2");
	raw_assert("(print '(1 . 2))", "(1 . 2)\n(1 . 2)\n");
	one_assert("(print '(1 . 2))", "(1 . 2)");


	one_assert(
		"(progn (setq foo '(a b c)) (rplaca foo d) foo)",
		"(D B C)"
	);
	one_assert(
		"(progn (setq foo '(a b c)) (rplacd foo d) foo)",
		"(A . D)"
	);
	one_assert(
		"(progn (setq x (list 'a 'b)) (setq z1 (cons x x)) (defun stk (x) (rplaca (car x) 'kok) x) (stk z1))",
		"((KOK B) KOK B)"
	);
	one_assert(
		"(progn (setq x (list 'a 'b)) (setq z2 (cons (list 'a 'b) (list 'a 'b))) (defun stk (x) (rplaca (car x) 'kok) x) (stk z2))",
		"((KOK B) A B)"
	);
	raw_assert(
		"(progn (setq lst '(1 (2 3) (2 . 3) a nil 4 5)) (loop ((not (car lst)) (cdr lst)) (print lst) (setq lst (cdr lst))))",
		"(1 (2 3) (2 . 3) A NIL 4 5)\n((2 3) (2 . 3) A NIL 4 5)\n((2 . 3) A NIL 4 5)\n(A NIL 4 5)\n(4 5)\n"
	);
}

//макросы
void macros() {
	one_assert("(defmacro)", nil_str);
	one_assert("(defmacro kek)", nil_str);
	one_assert("(defmacro kek (x))", "KEK");
	one_assert("(defmacro setqq (x y) (list 'setq x (list 'quote y)))", "SETQQ");
	one_assert("(progn (defmacro kek (a 's d) d) (kek 1 2 3))", "3");
	one_assert("(progn (setq nekok chel) (defmacro setqq (x y) (list 'setq x (list 'quote y))) (setqq kok nekok) kok)", "NEKOK");
	one_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macrocdr (q w e r) 2))",
		"A");       //стоит подумать над примером
	raw_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macrocdr (q w e r t) 3))",
		"(R T)\nA\n"
	);
	one_assert(
		"(progn (defmacro macrocdr (X) X) (getd macrocdr))",
		"(MACRO BODY ((LAMBDA (X) X) (CAR (CDR BODY))))"
	);
	one_assert("(progn (defmacro len lst (length lst)) (len 1 2 3 4 5 6 7 8))", "8");
	one_assert(
		"(progn (defmacro when (e . s) (list 'cond (append (list e) s))) (when T (+ 1 2)))",
		"3");
	raw_assert(
		"(progn (defmacro when (e . s) (list 'cond (append (list e) s))) (when T (print kok) (print (setq b 2))))",
		"KOK\n2\n2\n"
	);
	raw_assert(
		"(progn (defmacro when (e . s) (list 'cond (append (list e) (print s)))) (when T q w e))",
		"(Q W E)\nE\n"
	);


	one_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macroexpand '(macrocdr (q w e r) 2)))",
		"A");
	raw_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macroexpand '(macrocdr (q w e r) 2)))",
		"(E R)\nA\n"
	);
	one_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macroexpand-1 '(macrocdr (q w e r) 2)))",
		"(MACROCDR (W E R) 1)"
	);
	one_assert(
		"(progn (defmacro macrocdr (X N) (cond ((eq N 0) (print x) a) (T (append (list 'macrocdr) (list (cdr X)) (list (- N 1)))))) (macroexpand-1 (macroexpand-1 '(macrocdr (q w e r) 2))))",
		"(MACROCDR (E R) 0)"
	);  //вроде скобки на месте

	one_assert("(macroexpand)", nil_str);
	one_assert("(macroexpand '(q w e))", "(Q W E)");
	one_assert("(macroexpand 1)", "1");
	one_assert("(macroexpand-1)", nil_str);
	one_assert("(macroexpand-1 '(q w e))", "(Q W E)");
	one_assert("(macroexpand-1 1)", "1");
}

//макрочары
void macrochars() {
	several_assert( ({"(set-macro-char \\. '(lambda () (print KEKW)))", "'(1 . 2)", "." }), ({ T_str, "(1 . 2)", "KEKW", "KEKW" }));
	several_assert(({ "(set-macro-char \\] '(lambda () KEKW))", "'( 1 ]", "]" }), ({ T_str, "(1)", "KEKW" }));
	several_assert(({ "(set-macro-char \\, '(lambda () KEKW))", ",",}), ({ T_str, "KEKW" }));
	several_assert(({
		"(and "
			"(progn (setq *READ-UPCASE* nil) T)"
			"(set-macro-char \\   '(lambda () KEKW) T)"
			"(set-macro-char \\\t '(lambda () KEKW) T)"
			"(set-macro-char \\\n '(lambda () KEKW) T)"
			"(set-macro-char \\\r '(lambda () KEKW) T)"
			"(set-macro-char \\;  '(lambda () KEKW) T)"
			"(set-macro-char \\\" '(lambda () KEKW) T)"
			"(set-macro-char \\|  '(lambda () KEKW) T)"
			"(set-macro-char \\(  '(lambda () KEKW) T)"
			"(set-macro-char \\)  '(lambda () KEKW) T)"
			"(set-macro-char \\\' '(lambda () KEKW) T)"
			"(set-macro-char \\]  '(lambda () KEKW) T)"
			"(set-macro-char \\`  '(lambda () KEKW) T)"
		" )"
		"- \t\n\r;\"|()\']AZazАЯая.#`", 
	}), ({
		T_str,
		//макрос сплитит построчно, поэтому \n бьём на 2
		"|- \t",
		"\r;\"\\|()\']AZazАЯая.#`|",
	}));
	
}

void xz_chto_za_categoria() {
	several_assert(
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
	several_assert(
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

void test_outs_group(tester_type& tester) {
	cout << "tests added: " << tester.count() << endl;

	auto m = vector<pair<string, std::vector<std::string>>>();
	bool fail = false;
	for (const auto& result : tester.execute<treads_count>()) {
		auto it = find_if(begin(m), end(m), [&result](const auto& p) {return result.name == p.first; });
		if (it == end(m) ) {
			m.push_back(pair{ result.name, std::vector<std::string>{} });
			it = prev(end(m));
		}

		if (result.fail)
			it->second.push_back(result.output);
		if (result.fail) fail = true;
	}
	for (const auto& result : m) {
		if (!empty(result.second)) {
			cout << result.first << " failed" << "\n";
			for (const auto& out : result.second) {
				cout << out << "\n";
			}
		}
		else {
			cout << result.first << " OK" << "\n";
		}
	}
	cout.flush();
	if (fail) exit(1);
}






//нужны тесты

//* listen
//* read_char
//* peek_char
//* unread_char

void test_bifuncs() {

	default_state = default_state_func();

	LogDuration ld{};

	ADD_TEST(test_eval_base);
	ADD_TEST(arifm);
	ADD_TEST(logic);
	ADD_TEST(lists);
	ADD_TEST(pred);
	ADD_TEST(usl);
	ADD_TEST(calcfun);
	ADD_TEST(control_calc);
	ADD_TEST(io);
	ADD_TEST(id_repr);
	ADD_TEST(id_func);
	ADD_TEST(dotpairs);
	ADD_TEST(macros);
	ADD_TEST(xz_chto_za_categoria);

	ADD_TEST(macrochars);

	test_outs_group(tester);
}