#pragma once

#include "FbxImporter.h"
#include "Delegate.hpp"
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

int main()
{
	/*
	Delegate<void> onTest;
	onTest += F;
	onTest += G;
	onTest();
	onTest -= F;
	onTest();
	auto lambda1 = []() {cout << "AAA" << endl; };
	*/
	// auto lambda2 = []() {cout << "BBB" << endl; };

	// function<void()> f1(lambda1);
	// cout << f1.target<lambda>() << endl;
	//// cout << typeid(decltype(lambda1)).name() << endl;
	//function<void()> f = f;// []() {cout << "test lambda" << endl; };
	//cout << f.target_type().name() << endl;
	//var temp = f.target<void(*)()>();
	//cout << temp << endl;
	//cout << *temp << endl;
	return 0;
}