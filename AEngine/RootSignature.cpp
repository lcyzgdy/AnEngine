#include "RootSignature.h"

namespace AEngine::RenderCore
{
	void RootSignature::Reset()
	{
	}

	ID3D12RootSignature * RootSignature::GetSignature() const
	{
		return m_rootSignature.Get();
	}
}