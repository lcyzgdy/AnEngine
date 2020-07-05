#pragma once
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include "onwind.h"

namespace AnEngine::Utility
{
	template<typename T>
	class DLL_API Singleton : public NonCopyable
	{
		friend typename T;
		template<typename U> friend class Singleton;

	protected:
		static T* s_uniqueObj;

		virtual ~Singleton() = default;

	public:
		__forceinline static T* Instance();
	};


#define DECLEAR_UNIQUE_OBJ(_TYPE) _TYPE* _TYPE::s_uniqueObj = nullptr
#define DECLEAR_INSTANCE(_TYPE) template<> _TYPE*  AnEngine::Utility::Singleton<_TYPE>::Instance() { if (s_uniqueObj == nullptr) { s_uniqueObj = new _TYPE(); } return s_uniqueObj; }
}

#endif // !__SINGLETON_H__
