#pragma once
#ifndef __PROMISE_H__
#define __PROMISE_H__

#include "IPromise.h"

namespace AnEngine
{


	class DLL_API Promise : public IPromise
	{
	public:
		Promise(std::function<void()>&& func);



		// 通过 IPromise 继承
		virtual IPromise& Then(std::function<void()>&& func) override;

		virtual IPromise& Catch(std::function<void()>&& func) override;

	};
}

#endif // !__PROMISE_H__
