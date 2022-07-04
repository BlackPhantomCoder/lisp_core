#pragma once

#include <deque>
#include <list>
#include <variant>
#include <unordered_map>
#include "Cell.h"

class CoreEnvironment;
//�����, ����������� �������, ���������� ��������, � �������� ������� c++ ��� ����������
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
	// ��� ����
	ParserHelper() = default;

	ParserHelper(CoreEnvironment& env);

	// Sp -> S
	sp_id make_empty_sp();

	// Sp -> Sp S
	sp_id make_sp(sp_id base, data_id elem);

	// �������� ������
	data_id make_list(sp_id sp);

	// �������� �������� ����
	data_id make_dp(sp_id f, data_id s);

	// �������� �������
	data_id make_symbol(const char* str);


	// ��������� ���� �����������, ����� ���
	bool save_if_macro(char ch);
	// ��������� �������������, ���� �����������, ����� - ��������
	void save_symbol_or_macro(char ch);
	// ���������
	void save(data_id cell);

	// ������� �������� "��������" (��� ����������)
	void saved_to_final();

	// ��� ������
	// ����� sp
	void output_sp(const char* str, sp_id sp);
	// ����� ��������� ������
	void output_cell(const char* str, data_id cell);


	// ��� �������

	// ������� ��������, ��������� �����
	data_id push_cell(const Cell& cell);

	// �������� ���������� ��������
	Cell& get_saved();
	// ������� �� �������������
	bool is_saved_macro();
	// �������� �� "��������"
	bool is_saved_final();
private:
	// ������ � ����������
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


//�������, ���������� ��������
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


