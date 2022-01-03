#pragma once
#ifndef __DEBUGLOG_H__
#define __DEBUGLOG_H__

#include<string>

namespace AnEngine::Debug
{
	//void Log(std::string l);
	void Log(const std::wstring& l);
	void Log(std::wstring&& l);
	void Log(uint32_t u);

	void LogError(const std::wstring& l);
	void LogError(std::wstring&& l);
}

#endif // !__DEBUGLOG_H__
