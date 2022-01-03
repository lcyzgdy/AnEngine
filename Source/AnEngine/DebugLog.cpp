#include "DebugLog.h"
#include<mutex>
#include<ctime>
#include<fstream>
#include<windows.h>
using namespace std;

namespace AnEngine::Debug
{
	wofstream fout(to_string(time(0)) + "_log.txt");
	mutex m_mutex;

	/*void Log(string l)
	{
		fout << l << std::endl;
	}*/

	void Log(wstring l)
	{
		lock_guard<mutex> lock(m_mutex);
		fout << "[Log] " << l << endl;
	}

	void Log(const wstring& l)
	{
		lock_guard<mutex> lock(m_mutex);
		fout << "[Log] " << l << endl;
	}

	void Log(wstring&& l)
	{
		lock_guard<mutex> lock(m_mutex);
		fout << "[Log] " << l << endl;
	}

	void Log(uint32_t u)
	{
		lock_guard<mutex> lock(m_mutex);
		fout << "[Log] " << u << endl;
	}

	void LogError(const wstring& l)
	{
		lock_guard<mutex> lock(m_mutex);
		fout << "[Error] " << l << endl;
	}

	void LogError(wstring&& l)
	{
		lock_guard<mutex> lock(m_mutex);
		fout << "[Error] " << l << endl;
	}
}