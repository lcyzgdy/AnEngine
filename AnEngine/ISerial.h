#pragma once
#ifndef __ISERIAL_H__
#define __ISERIAL_H__

namespace AnEngine::Game
{
	template<typename ..._ComponentType>
	class ISerial;

	/*
	 * Job 是纯执行方法，可依赖 Component 中的数据，不可依赖 Behaviour 的数据。
	 * Job 有两种，这里是串行执行的 Job，因此不建议此处依赖其他 Entity 的Component，否则性能会大幅下降，而且可能死锁。
	 */
	template<>
	class ISerial<>
	{
	public:
		virtual void Execute(int index) = 0;
		virtual bool Check(int index) { return true; }
		size_t Length;
	};

	template<typename ..._ComponentType>
	class ISerial : public ISerial<>
	{
	};
}

#endif // !__ISERIAL_H__
