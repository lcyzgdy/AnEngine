#include "Promise.h"

namespace AnEngine
{
	IPromise& Promise::Then(std::function<void()>&& func)
	{
		return *this;
	}

	IPromise& Promise::Catch(std::function<void()>&& func)
	{
		return *this;
	}
}