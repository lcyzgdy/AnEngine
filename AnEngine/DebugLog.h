#pragma once
#ifndef __DEBUGLOG_H__
#define __DEBUGLOG_H__

#include<string>

namespace AnEngine::Debug
{
	class Debug
	{
	public:
		static void Log(std::string l);
	};
}

#endif // !__DEBUGLOG_H__
