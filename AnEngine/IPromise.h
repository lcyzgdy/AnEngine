#pragma once
#ifndef __IPROMISE_H__
#define __IPROMISE_H__

#include "onwind.h"

namespace AnEngine
{
	class DLL_API IPromise
	{
	public:
		virtual IPromise& Then(std::function<void()>&& func) = 0;
		virtual IPromise& Catch(std::function<void()>&& func) = 0;
	};
}

#endif // !__IPROMISE_H__
