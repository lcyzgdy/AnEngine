#include "FbxImporter.h"
#include "AssetsDatabase.h"

using namespace std;
using namespace AnEngine;

int main()
{
	AnEngine::AssetsWrapper::LoadFbxFromFile("C:/Users/PC/Documents/Code/VSProject/AnEngine/Assets/unitychan.fbx");
	cout << AssetsWrapper::AssetsDatabase::Instance()->StatisticsMessageStream << endl;
	return 0;
}