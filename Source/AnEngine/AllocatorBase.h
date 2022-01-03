#pragma once
#ifndef __ALLOCATORBASE__H__
#define __ALLOCATORBASE__H__

namespace AnEngine::Memory
{
	class AllocatorBase
	{
	public:
		virtual ~AllocatorBase() = 0 {}
	};
}

#endif // !__ALLOCATORBASE__H__
