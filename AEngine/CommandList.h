#pragma once
#ifndef __COMMANDLIST_H__
#define __COMMANDLIST_H__

#include"DX.h"

namespace RenderCore
{
	class CommandList
	{
		ComPtr<ID3D12CommandList> m_commandList;
	public:
		CommandList() = default;
	};
}

#endif // !__COMMANDLIST_H__
