#pragma once
#ifndef __ANENGINE__GRAPHVERTEX_HPP__
#define __ANENGINE__GRAPHVERTEX_HPP__

#include "../../onwind.h"

namespace AnEngine::Library
{
	template<typename T>
	struct DLL_API GraphVertex
	{
		T data;
	};

	template<typename T>
	using GraphNode = GraphVertex<T>;

	template<typename _NodeDataType, typename _WeightType>
	struct DLL_API GraphEdge
	{
		GraphVertex<_NodeDataType>& v1;
		GraphVertex<_NodeDataType>& v2;
		_WeightType weight;

		GraphEdge() = delete;

		GraphEdge(GraphVertex<_NodeDataType>& _v1, GraphVertex<_NodeDataType>& _v2, const _WeightType& _weight) : v1(_v1), v2(_v2), weight(_weight)
		{
		}

		GraphEdge(GraphVertex<_NodeDataType>&& _v1, GraphVertex<_NodeDataType>&& _v2, const _WeightType& _weight) : GraphEdge(std::forward(_v1), std::forward(_v2), weight(_weight))
		{
		}
	};
}

#endif // !__ANENGINE__GRAPHVERTEX_HPP__
