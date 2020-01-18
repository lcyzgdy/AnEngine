#pragma once
#ifndef __SAMPLEMESHRENDER_H__
#define __SAMPLEMESHRENDER_H__

#include"Renderer.h"
#include"PipelineState.h"
#include"VertexBuffer.h"
#include"MCamera.h"
#include"SampleAssets.h"
#include"Fence.h"

namespace AnEngine::Game
{
	class SampleMeshRenderer : public Renderer
	{
		const static uint32_t NumLights = 3u;

		struct LightState
		{
			XMFLOAT4 position;
			XMFLOAT4 direction;
			XMFLOAT4 color;
			XMFLOAT4 falloff;

			XMFLOAT4X4 view;
			XMFLOAT4X4 projection;
		};

		struct MyConstantBuffer
		{
			XMFLOAT4X4 model;
			XMFLOAT4X4 view;
			XMFLOAT4X4 projection;
			XMFLOAT4 ambientColor;
			BOOL sampleShadowMap;
			BOOL padding[3];
			LightState lights[NumLights];
		};

		std::wstring m_fileName;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		RenderCore::GraphicPSO* m_psoShadowMap;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencil;
		ID3D12DescriptorHeap* m_dsvHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_dsvGpuHandle;

		RenderCore::Resource::VertexBuffer* m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_textures[_countof(SampleAssets::Textures)];
		Microsoft::WRL::ComPtr<ID3D12Resource> m_textureUploads[_countof(SampleAssets::Textures)];
		ID3D12DescriptorHeap* m_cbvSrvHeap;
		ID3D12DescriptorHeap* m_samplerHeap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_cbvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE m_samplerHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_cbvGpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_samplerGpuHandle;

		D3D12_GPU_DESCRIPTOR_HANDLE m_nullSrvHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_shadowDepthHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE m_shadowCbvHandle;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_shadowTexture;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_shadowConstantBuffer;
		MyConstantBuffer* mp_shadowConstantBufferWO;
		MyConstantBuffer* mp_sceneConstantBufferWO;

		LightState m_lights[NumLights];
		LightCamera m_lightCameras[NumLights];

		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;

		RenderCore::Fence* m_fence;

	public:
		SampleMeshRenderer(std::wstring&& fileName);

		// 通过 Renderer 继承
		virtual void LoadAsset() override;
		virtual void OnRender(ID3D12GraphicsCommandList* iList, ID3D12CommandAllocator* iAllocator) override;

		virtual void Destory() override;
	};
}
#endif // !__SAMPLEMESHRENDER_H__
