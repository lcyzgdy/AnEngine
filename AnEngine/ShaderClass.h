#pragma once
#ifndef __SHADER_H__
#define __SHADER_H__

#include"DX.h"

namespace AnEngine::RenderCore
{
	class Shader
	{
	protected:
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
		uint32_t m_compileFlag;
	public:
		Shader(const std::wstring& fileName, const std::string& invokeFunction, std::string&& shaderVersion);
		Shader(std::wstring&& fileName, std::string&& invokeFunction, std::string&& shaderVersion);
		Shader(const std::wstring& fileName);
		Shader(std::wstring&& fileName);

		D3D12_SHADER_BYTECODE GetByteCode();
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader(const std::wstring& fileName);
		VertexShader(std::wstring&& fileName);
		VertexShader(const std::wstring& fileName, const std::string& invokeFunction);
		VertexShader(std::wstring&& fileName, std::string&& invokeFunction);
	};

	class PixelShader : public Shader
	{
	public:
		PixelShader(const std::wstring& fileName);
		PixelShader(std::wstring&& fileName);
		PixelShader(const std::wstring& fileName, const std::string& invokeFunction);
		PixelShader(std::wstring&& fileName, std::string&& invokeFunction);
	};

	class GeometryShader : public Shader
	{
	public:
		GeometryShader() = default;
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader(const std::wstring& fileName);
		ComputeShader(std::wstring&& fileName);
		ComputeShader(const std::wstring& fileName, const std::string& invokeFunction);
		ComputeShader(std::wstring&& fileName, std::string&& invokeFunction);
	};
}

#endif // !__SHADER_H__
