#pragma once
#ifndef __ROOTSIGNATURE_H__
#define __ROOTSIGNATURE_H__

#include"DX.h"

namespace RenderCore
{
	class RootSignature
	{
	protected:
		ComPtr<ID3D12RootSignature> m_rootSignature;

	public:
		RootSignature();
		~RootSignature();
		void Reset();
	};
}

#endif // !__ROOTSIGNATURE_H__