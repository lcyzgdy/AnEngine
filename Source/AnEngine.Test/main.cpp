#pragma once

#include "FbxImporter.h"
#include "Delegate.hpp"
using namespace std;
using namespace AnEngine::AssetsWrapper;
using namespace AnEngine;

int main()
{
	Delegate<void> onTest;

	onTest += []() {cout << "Delegate Test!" << endl; };
	onTest();
}