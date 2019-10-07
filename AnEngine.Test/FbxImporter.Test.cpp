#include "FbxImporter.h"

using namespace std;
using namespace AnEngine;

int main()
{
	AnEngine::AssetsWrapper::LoadFbxFromFile("./Assets/unitychan.fbx");
	return 0;
}