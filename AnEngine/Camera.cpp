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

	void Camera::BeforeUpdate()
	{
		if (m_postProcessShader != nullptr) PostProcess();
		BlendBuffer(m_colorBuffers.GetFront());
		m_colorBuffers.Swap();
	}

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
		ObjectBehaviour::Update();
		//this_thread::sleep_for(1ms);
		lock_guard<mutex> lock(m_rtvMutex);
		//m_colorBuffer->GetFence()->CpuWait(Timer::GetTotalTicks());

		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		//var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetOne();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();
		var frameBuffer = m_colorBuffers.GetBack();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, nullptr));

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToCommon;
		commonToRenderTarget.Transition.pResource = frameBuffer->GetResource();// m_colorBuffers.GetBack()->GetResource();
		renderTargetToCommon.Transition.pResource = frameBuffer->GetResource();// m_colorBuffers.GetBack()->GetResource();

		iCommandList->ResourceBarrier(1, &commonToRenderTarget);
		//var clearColorTemp = m_colorBuffer->GetClearColor();
		float clearColor[4] = { 0.0f, 0.2f, sin((float)Timer::GetTotalTicks() / 300000), 1.0f };
		//iCommandList->ClearRenderTargetView(m_colorBuffer->GetRTV(), clearColor, 0, nullptr);
		iCommandList->ClearRenderTargetView(frameBuffer->GetRTV(), clearColor, 0, nullptr);
		iCommandList->ResourceBarrier(1, &renderTargetToCommon);
		ThrowIfFailed(iCommandList->Close());
		ID3D12CommandList* ppcommandList[] = { iCommandList };
		r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);

		GraphicsContext::Push(commandList, commandAllocator);
	}

	void Camera::AfterUpdate()
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

	Camera::Camera(const std::wstring& name) : ObjectBehaviour(name), m_clearFlag(ClearFlags::SkyBox)
	{
		var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		colorBuffer->SetAsRenderTargetView();
		m_depthBuffer = new DepthBuffer(0, 0);

		m_colorBuffers.ManageBuffer(colorBuffer);
		colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		colorBuffer->SetAsRenderTargetView();
		m_colorBuffers.ManageBuffer(colorBuffer);
	}

	Camera::Camera(wstring&& name) : ObjectBehaviour(name), m_clearFlag(ClearFlags::SkyBox)
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
	}

	Camera::Camera(wstring&& name, ClearFlags clearFlag) : ObjectBehaviour(name), m_clearFlag(clearFlag)
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
	}

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
	}

	Camera::~Camera()
	{
		/*if (m_colorBuffer)
		{
			//m_colorBuffer->Release();
			delete m_colorBuffer;
			m_colorBuffer = nullptr;
		}*/
		if (m_depthBuffer)
		{
			//m_depthBuffer->Release();
			delete m_depthBuffer;
			m_depthBuffer = nullptr;
		}
	}

	ColorBuffer* Camera::GetColorBuffer()
	{
		//lock_guard<mutex> lock(m_rtvMutex);
		//return m_colorBuffer;
		return m_colorBuffers.GetBack();
	}

	DepthBuffer * Camera::GetDepthBuffer()
	{
		return m_depthBuffer;
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