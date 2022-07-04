#pragma once

#include <deque>
#include <list>
#include <variant>
#include <unordered_map>
#include "Cell.h"

class CoreEnvironment;
//Класс, реализующий функции, вызываемые парсером, и хранящий объекты c++ для грамматики
class ParserHelper {
public:
	using id_type = unsigned long;

	using sp_id = id_type;
	using data_id = id_type;
private:
	using sp = std::deque<Cell>;
	using data_type = std::variant<Cell, sp>;
	using container = std::unordered_map<id_type, data_type>;
public:
	// для пула
	ParserHelper() = default;

	ParserHelper(CoreEnvironment& env);

	// Sp -> S
	sp_id make_empty_sp();

	// Sp -> Sp S
	sp_id make_sp(sp_id base, data_id elem);

	// создание списка
	data_id make_list(sp_id sp);

	// создание точечной пары
	data_id make_dp(sp_id f, data_id s);

	// создание символа
	data_id make_symbol(const char* str);


	// сохраняет если макросимвол, иначе нет
	bool save_if_macro(char ch);
	// сохраняет макросимволом, если макросимвол, иначе - символом
	void save_symbol_or_macro(char ch);
	// сохраняет
	void save(data_id cell);

	// сделать значение "конечным" (для грамматики)
	void saved_to_final();

	// для дебага
	// вывод sp
	void output_sp(const char* str, sp_id sp);
	// вывод остальных данных
	void output_cell(const char* str, data_id cell);


	// доп функции

	// создать значение, созданной извне
	data_id push_cell(const Cell& cell);

	// получить сохранённое значение
	Cell& get_saved();
	// явлется ли макросимволом
	bool is_saved_macro();
	// является ли "конечным"
	bool is_saved_final();
private:
	// работа с хранилищем
	id_type t_push_resourse(std::variant<Cell, sp>&& res);
	void t_remove_resourse(id_type id);

	Cell& t_get_cell(id_type id);
	sp& t_get_sp(id_type id);
private:
	std::deque<int> t_ids;
	int t_max_id = 0;
	container t_data;
	Cell* t_saved = nullptr;
	bool t_macro = false;
	bool t_final = false;
	CoreEnvironment* t_env = nullptr;
};


//функции, вызываемые парсером
extern "C" unsigned long make_empty_sp(void* ph);
extern "C" unsigned long make_sp(void* ph, unsigned long base, unsigned long elem);
extern "C" unsigned long make_list(void* ph, unsigned long sp);
extern "C" unsigned long make_dp(void* ph, unsigned long f, unsigned long s);
extern "C" void save(void* ph, unsigned long cell);
extern "C" unsigned long make_symbol(void* ph, const char* str);
extern "C" void output_sp(void* ph, const char*, unsigned long sp);
extern "C" void output_cell(void* ph, const char*, unsigned long cell);
extern "C" void saved_to_final(void* ph);

extern "C" void save_symbol_or_macro(void* ph, char ch);
extern "C" void save_if_or_macro(void* ph, char ch, int* result);


