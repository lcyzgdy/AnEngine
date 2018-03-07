#include "DebugLog.h"
#include<mutex>
#include<ctime>
#include<fstream>
using namespace std;

namespace AnEngine::Debug
{
	ofstream fout(to_string(time(0)) + "_log.txt");

	void Debug::Log(std::string l)
	{
		fout << l << std::endl;
	}
}