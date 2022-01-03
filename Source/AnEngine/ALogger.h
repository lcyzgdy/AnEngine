#pragma once
#ifndef __ALOGGER_H__
#define __ALOGGER_H__

#include <string>

#include "onwind.h"

// TODO:
/*
 * 这里希望设计一个多线程可用可写入文件或stdout的日志，并尽可能轻量
 */

namespace AnEngine
{
	/// <summary>
	/// Logger实例的输出应该一致
	/// </summary>
	class ALogger
	{


	public:
		enum class LogLevel
		{
			Debug,
			Info,
			Warning,
			Error,
			Critical
		};

		ALogger() = default;
		~ALogger() = default;

		void Write(LogLevel level, const std::string& text);

		void Debug(const std::string& text);
		void Info(const std::string& text);
		void Warning(const std::string& text);
		void Error(const std::string& text);
		void Critical(const std::string& text);
	};
}

#endif 