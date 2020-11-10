#pragma once

#include "FbxImporter.h"
#include "Delegate.hpp"
#include <functional>
using namespace std;
using namespace AnEngine::AssetsWrapper;
using namespace AnEngine;


int F(int p)
{
	cout << __FUNCDNAME__ << endl;
	return p + 4;
}

int G(int p)
{
	cout << __FUNCDNAME__ << endl;
	return p + 5;
}

class A
{
	int m_p;
public:
	A(int p) :m_p(p)
	{
	}
	int F(int p)
	{
		cout << __FUNCDNAME__ << endl;
		cout << p << endl;
		return m_p + p;
	}
};

int main1()
{
	Delegate<int, int> onTest;
	/*
	onTest += F;
	onTest += G;
	onTest();
	onTest -= F;
	onTest();
	*/
	A a(6);
	A b(10);
	onTest += Utility::MakeDelegateClassHelper(&(A::F), &a);
	onTest += Utility::MakeDelegateClassHelper(&(A::F), &b);
	// cout << onTest(3)[0] << ends << onTest(3)[1] << endl;
	var res = onTest(3);
	cout << res[0] << ends << res[1] << endl;
	onTest -= Utility::MakeDelegateClassHelper(&(A::F), &b);
	res = onTest(3);
	onTest += F;
	onTest += G;
	res = onTest(3);
	onTest -= F;
	res = onTest(3);
	onTest += [](int p) {return p + 2; };
	res = onTest(3);
	return 0;
}