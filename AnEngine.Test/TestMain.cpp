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
	int m_p;
public:
	A(int p)
	{
		m_p = p;
	}
	int F(int p)
	{
		cout << __FUNCDNAME__ << endl;
		cout << p << endl;
		return m_p + p;
	}
};

int main()
{
	Delegate<int, int> onTest;
	/*
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
	A a(6);
	A b(10);
	onTest += Utility::MakeDelegateClassHelper(&(A::F), &a);
	onTest += Utility::MakeDelegateClassHelper(&(A::F), &b);
	// function<int(int)> f = bind(&(A::F), b, std::_Ph<1>());
	// cout << typeid(f).name() << endl;
	// cout << f(3) << endl;
	cout << onTest(3)[0] << ends << onTest(3)[1] << endl;
	//// cout << typeid(decltype(lambda1)).name() << endl;
	//function<void()> f = f;// []() {cout << "test lambda" << endl; };
	//cout << f.target_type().name() << endl;
	//var temp = f.target<void(*)()>();
	//cout << temp << endl;
	//cout << *temp << endl;
	return 0;
}