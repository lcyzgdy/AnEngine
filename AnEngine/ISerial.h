#pragma once
#ifndef __ISERIAL_H__
#define __ISERIAL_H__

namespace AnEngine::Game
{
	class ISerial
	{
	public:
		virtual void Execute(int index) = 0;
		virtual bool Check(int index) { return true; }
		size_t Length;
	};
}

#endif // !__ISERIAL_H__
