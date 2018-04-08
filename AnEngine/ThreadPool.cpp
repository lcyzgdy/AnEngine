#include "ThreadPool.hpp"

namespace Utility::ThreadPool
{
	namespace Private
	{
		ThreadPool u_s_threadPool(std::thread::hardware_concurrency());
	}
}