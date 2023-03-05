#pragma once
#ifndef __ANENGINE__PARAMETER_HPP
#define __ANENGINE__PARAMETER_HPP

namespace AnEngine::ECS
{
	template<typename T>
	class ParameterConstraints
	{
	protected:
		using _ConstType = const T;
		using _ConstRefType = const T&;
		using _Type = T;
		using _RefType = T&
	};

	template<typename T>
	class ReadOnlyParameter
	{
	public:
		using _RealType = ParameterConstraints<T>::_ConstRefType;
	};
}

#endif // !__ANENGINE__PARAMETER_HPP
