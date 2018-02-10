#pragma once
#ifndef __SHADER_H__
#define __SHADER_H__

#include"DX.h"

namespace AnEngine::RenderCore
{
	class Shader
	{
	protected:
		ComPtr<ID3DBlob> m_blob;
		uint32_t m_compileFlag;
	public:
		Shader(wstring fileName);
		Shader(wstring fileName, wstring invokeFunction);

		D3D12_SHADER_BYTECODE GetByteCode();
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader() = default;
	};

	class PixelShader : public Shader
	{
	public:
		PixelShader() = default;
	};

	class GeometryShader : public Shader
	{
	public:
		GeometryShader() = default;
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader() = default;
	};
}

#endif // !__SHADER_H__
