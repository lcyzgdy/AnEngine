#pragma once
#ifndef __COMMONSTATE_H__
#define __COMMONSTATE_H__

#include <d3d12.h>

#include "onwind.h"

namespace AnEngine::RenderCore::CommonState
{
	extern D3D12_RESOURCE_BARRIER commonToRenderTarget;
	extern D3D12_RESOURCE_BARRIER renderTargetToCommon;
	extern D3D12_RESOURCE_BARRIER commonToResolveDest;
	extern D3D12_RESOURCE_BARRIER resolveDestToCommon;
	extern D3D12_RESOURCE_BARRIER renderTargetToResolveDest;
	extern D3D12_RESOURCE_BARRIER resolveSourceToRenderTarget;
	extern D3D12_RESOURCE_BARRIER presentToRenderTarget;
	extern D3D12_RESOURCE_BARRIER renderTargetToPresent;
	extern D3D12_RESOURCE_BARRIER renderTarget2ResolveSource;
	extern D3D12_RESOURCE_BARRIER commonToResolveSource;
	extern D3D12_RESOURCE_BARRIER resolveSourceToCommon;
	extern D3D12_RESOURCE_BARRIER depthWrite2PsResource;
	extern D3D12_RESOURCE_BARRIER psResource2DepthWrite;
}

#endif // !__COMMONSTATE_H__
