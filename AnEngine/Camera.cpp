#include "Camera.h"
#include "RenderCore.h"
#include "ThreadPool.hpp"
#include "Screen.h"
#include"CommandContext.h"
#include"DTimer.h"

using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Resource;

namespace AnEngine::Game
{
	static vector<Camera*> cameraPool;
	static mutex poolMutex;

	/*void Camera::BeforeUpdate()
	{
		if (m_postProcessShader != nullptr) PostProcess();
		m_colorBuffers.Swap();
		BlendBuffer(m_colorBuffers.GetFront());
	}*/

	void Camera::Start()
	{
		ObjectBehaviour::Start();
		cameraPool.emplace_back(this);
	}

	void Camera::OnActive()
	{
	}

	void Camera::Update()
	{
		if (m_postProcessShader != nullptr) PostProcess();
		m_colorBuffers.Swap();
		BlendBuffer(m_colorBuffers.GetFront());

		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		var iList = commandList->GetCommandList();
		var iAllocator = commandAllocator->GetAllocator();
		var frameBuffer = m_colorBuffers.GetBack();

		ThrowIfFailed(iAllocator->Reset());
		ThrowIfFailed(iList->Reset(iAllocator, nullptr));

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToCommon;
		commonToRenderTarget.Transition.pResource = frameBuffer->GetResource();
		renderTargetToCommon.Transition.pResource = frameBuffer->GetResource();

		iList->ResourceBarrier(1, &commonToRenderTarget);
		//float clearColor[4] = { 0.0f, 0.2f, sin((float)Timer::GetTotalTicks() / 30000), 1.0f };
		iList->ClearRenderTargetView(frameBuffer->GetRTV(), m_clearColor.GetPtr(), 0, nullptr);
		iList->ResourceBarrier(1, &renderTargetToCommon);
		ThrowIfFailed(iList->Close());

		GraphicsContext::Push(commandList, commandAllocator);
	}

	void Camera::LateUpdate()
	{
	}

	void Camera::OnInvalid()
	{
	}

	void Camera::Destory()
	{
		var it = find(cameraPool.begin(), cameraPool.end(), this);
		if (it != cameraPool.end() && cameraPool.size() > 0)
		{
			cameraPool.erase(it);
		}
	}

	void Camera::PostProcess()
	{
	}

	Camera::Camera() : ObjectBehaviour(), m_clearFlag(ClearFlags::SkyBox)
	{
		{
			var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
				Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
			colorBuffer->SetAsRenderTargetView();
			//m_depthBuffer = new DepthBuffer(0, 0);

			m_colorBuffers.ManageBuffer(colorBuffer);
		}
		{
			var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
				Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
			colorBuffer->SetAsRenderTargetView();

			m_colorBuffers.ManageBuffer(colorBuffer);
		}
	}

	/*Camera::Camera(wstring&& name) : ObjectBehaviour(name), m_clearFlag(ClearFlags::SkyBox)
	{
		//m_colorBuffer = new ColorBuffer(Color::Blue);
		//m_colorBuffer->Create(this->name, Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT);
		var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		colorBuffer->SetAsRenderTargetView();
		m_colorBuffers.ManageBuffer(colorBuffer);

		colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		colorBuffer->SetAsRenderTargetView();
		m_colorBuffers.ManageBuffer(colorBuffer);

		m_depthBuffer = new DepthBuffer(0, 0);
	}*/

	Camera::Camera(ClearFlags clearFlag) : ObjectBehaviour(), m_clearFlag(clearFlag)
	{
		{
			var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
				Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
			colorBuffer->SetClearColor(Color::Blue);
			colorBuffer->SetAsRenderTargetView();
			m_colorBuffers.ManageBuffer(colorBuffer);
		}
		//m_depthBuffer = new DepthBuffer(0, 0);
		{
			var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
				Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
			colorBuffer->SetAsRenderTargetView();
			m_colorBuffers.ManageBuffer(colorBuffer);
		}
	}
	/*
	Camera::Camera(const std::wstring& name, ClearFlags clearFlag) : ObjectBehaviour(name), m_clearFlag(clearFlag)
	{
		var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		colorBuffer->SetClearColor(Color::Blue);
		colorBuffer->SetAsRenderTargetView();
		m_depthBuffer = new DepthBuffer(0, 0);

		m_colorBuffers.ManageBuffer(colorBuffer);
		colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		colorBuffer->SetAsRenderTargetView();
		m_colorBuffers.ManageBuffer(colorBuffer);
	}*/

	Camera::~Camera()
	{
		/*if (m_colorBuffer)
		{
			//m_colorBuffer->Release();
			delete m_colorBuffer;
			m_colorBuffer = nullptr;
		}*/
		/*if (m_depthBuffer)
		{
			//m_depthBuffer->Release();
			delete m_depthBuffer;
			m_depthBuffer = nullptr;
		}*/
	}

	ColorBuffer* Camera::GetColorBuffer()
	{
		//lock_guard<mutex> lock(m_rtvMutex);
		//return m_colorBuffer;
		return m_colorBuffers.GetBack();
	}

	DepthBuffer* Camera::GetDepthBuffer()
	{
		//return m_depthBuffer;
		return nullptr;
	}

	void Camera::ClearFlag(ClearFlags flag)
	{
		m_clearFlag = flag;
		if (flag == ClearFlags::SolidColor)
		{
			m_clearColor = Color::Blue;
			//m_colorBuffers.GetBack()->SetClearColor(m_clearColor);
			//m_colorBuffers.GetFront()->SetClearColor(m_clearColor);
		}
	}

	Camera::ClearFlags Camera::ClearFlag()
	{
		return m_clearFlag;
	}

	void Camera::ClearColor(Color color)
	{
		m_clearColor = color;
		//m_colorBuffer->SetClearColor(m_clearColor);
	}

	Color Camera::ClearColor()
	{
		return m_clearColor;
	}

	ColorBuffer* Camera::FindForwordTarget(Vector3&& pos)
	{
		lock_guard<mutex> lock(poolMutex);
		for (var i : cameraPool)
		{
			if (i != nullptr) return i->GetColorBuffer();
		}
		return nullptr;
	}
}