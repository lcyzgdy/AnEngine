#pragma once
#ifndef __UNIQUEID_HPP__
#define __UNIQUEID_HPP__

#include "onwind.h"
#include <mutex>

namespace AnEngine
{
	class UniqueId : public Singleton<UniqueId>
	{
		std::uint64_t m_uid;
		std::mutex m_mutex;

	public:
		UniqueId()
		{
			m_uid = 65535LL;
		}

		uint64_t GetUniqueId()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_uid++;
			return m_uid;
		}
	};
}

#endif // !__UNIQUEID_H__
