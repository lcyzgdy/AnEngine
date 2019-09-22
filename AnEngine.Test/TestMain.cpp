#pragma once

#include "FbxImporter.h"
#include "Delegate.hpp"
#include <functional>
using namespace std;
using namespace AnEngine::AssetsWrapper;
using namespace AnEngine;

void F()
{
	cout << __FUNCDNAME__ << endl;
}

void G()
{
	cout << __FUNCDNAME__ << endl;
}

class A
{
public:
	int F(int p)
	{
		cout << __FUNCDNAME__ << endl;
		cout << p << endl;
		return 3 + p;
	}
};

int main()
{
	/*
	Delegate<void> onTest;
	onTest += F;
	onTest += G;
	onTest();
	onTest -= F;
	onTest();
	*/
	// auto lambda2 = []() {cout << "BBB" << endl; };
/*
	auto lambda1 = []() {cout << "AAA" << endl; };
	function<void()> f1(lambda1);
	cout << typeid(decltype(lambda1)).name() << endl;
	cout << f1.target<decltype(lambda1)>() << endl;*/
	A a;
	auto f = bind(&(A::F), a, std::_Ph<1>());
	f(3);
	//// cout << typeid(decltype(lambda1)).name() << endl;
	//function<void()> f = f;// []() {cout << "test lambda" << endl; };
	//cout << f.target_type().name() << endl;
	//var temp = f.target<void(*)()>();
	//cout << temp << endl;
	//cout << *temp << endl;
	return 0;
}