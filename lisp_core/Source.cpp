#include <iostream>

#include "test_runner.h"
#include "profile.h"
using namespace std;
/*
	Задание


*/



void test_all();
void default_test();


int main() {
	

}

void default_test() {

}

void test_all() {
	TestRunner runner;
	RUN_TEST(runner, default_test);
}