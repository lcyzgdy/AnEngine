#pragma once
#ifndef __SHADER_H__
#define __SHADER_H__

#include"DX.h"

namespace RenderCore
{
	class Shader
	{
	protected:
		ComPtr<ID3DBlob> m_blob;
		uint32_t m_compileFlag;
	public:
		Shader();

		D3D12_SHADER_BYTECODE GetByteCode();
		virtual void CompileFromFile(string& fileName) = 0;
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader() = default;

		// 通过 Shader 继承
		virtual void CompileFromFile(string& fileName) override;
	};

	class PixelShader : public Shader
	{
	public:
		PixelShader() = default;

		// 通过 Shader 继承
		virtual void CompileFromFile(string& fileName) override;
	};

	class GeometryShader : public Shader
	{
	public:
		GeometryShader() = default;

		// 通过 Shader 继承
		virtual void CompileFromFile(string& fileName) override;
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader() = default;

		// 通过 Shader 继承
		virtual void CompileFromFile(string& fileName) override;
	};
}

#endif // !__SHADER_H__
