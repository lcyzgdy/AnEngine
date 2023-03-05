#pragma once
#ifndef __ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__PLATFORMCOMMANDLIST_H__
#define __ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__PLATFORMCOMMANDLIST_H__

#include <d3d12.h>
#include <wrl.h>

#include "../Rendering/CommandList.h"

namespace AnEngine::PlatformDependence::Rendering
{
	class D3D12CommandList : AnEngine::Rendering::IPlatformCommandListImpl
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

	public:
		~D3D12CommandList() = default;

		virtual void Init() override;
		virtual void Destroy() override;
	};
}

#endif // !__ANENGINE_PLATFORMDEPENDENCE_WINDOWS_DIRECTX__PLATFORMCOMMANDLIST_H__