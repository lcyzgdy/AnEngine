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
	/*void Camera::OnInit()
	{
		lock_guard<recursive_mutex> lock(m_recursiveMutex);
		ObjectBehaviour::OnInit();
		m_colorBuffer = new ColorBuffer(Color::Blue);
		//m_depthBuffer = new DepthBuffer();

		if (m_enable) OnEnable();
	}*/

	/*void Camera::BeforeUpdate()
	{
		this_thread::sleep_for(std::chrono::milliseconds(100 / 6));
	}*/

	void Camera::Start()
	{
		cameraPool.emplace_back(this);
	}

	void Camera::OnActive()
	{
	}

	void Camera::Update()
	{
		this_thread::sleep_for(1ms);
		lock_guard<mutex> lock(m_rtvMutex);
		//m_colorBuffer->GetFence()->CpuWait(Timer::GetTotalTicks());

		var[commandList, commandAllocator] = GraphicsContext::GetOne();
		//var commandAllocator = GraphicsCommandAllocator::GetInstance()->GetOne();
		var iCommandList = commandList->GetCommandList();
		var iCommandAllocator = commandAllocator->GetAllocator();

		ThrowIfFailed(iCommandAllocator->Reset());
		ThrowIfFailed(iCommandList->Reset(iCommandAllocator, nullptr));

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToCommon;
		commonToRenderTarget.Transition.pResource = m_colorBuffer->GetResource();
		renderTargetToCommon.Transition.pResource = m_colorBuffer->GetResource();

		iCommandList->ResourceBarrier(1, &commonToRenderTarget);
		var clearColorTemp = m_colorBuffer->GetClearColor();
		float clearColor[4] = { 0.0f, 0.2f, sin((float)Timer::GetTotalTicks() / 300000), 1.0f };
		iCommandList->ClearRenderTargetView(m_colorBuffer->GetRTV(), clearColor, 0, nullptr);
		iCommandList->ResourceBarrier(1, &renderTargetToCommon);
		iCommandList->Close();
		ID3D12CommandList* ppcommandList[] = { iCommandList };
		r_graphicsCard[0]->ExecuteSync(_countof(ppcommandList), ppcommandList);
		m_colorBuffer->GetFence()->GpuSignal(Timer::GetTotalTicks());

		GraphicsContext::Push(commandList, commandAllocator);
		//GraphicsCommandContext::GetInstance()->Push(commandList);
		//GraphicsCommandAllocator::GetInstance()->Push(commandAllocator);
	}

	void Camera::AfterUpdate()
	{
		//RenderColorBuffer(m_colorBuffer);
		if (m_postProcessShader != nullptr) PostProcess();
		//this_thread::sleep_for(std::chrono::microseconds(1));
		BlendBuffer(m_colorBuffer);
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
		m_colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_colorBuffer->SetAsRenderTargetView();
		m_depthBuffer = new DepthBuffer(0, 0);
	}

	Camera::Camera(wstring&& name) : ObjectBehaviour(name), m_clearFlag(ClearFlags::SkyBox)
	{
		//m_colorBuffer = new ColorBuffer(Color::Blue);
		//m_colorBuffer->Create(this->name, Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT);
		m_colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_colorBuffer->SetAsRenderTargetView();
		m_depthBuffer = new DepthBuffer(0, 0);
	}

	Camera::Camera(wstring&& name, ClearFlags clearFlag) : ObjectBehaviour(name), m_clearFlag(clearFlag)
	{
		m_colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_colorBuffer->SetClearColor(Color::Blue);
		m_colorBuffer->SetAsRenderTargetView();
		m_depthBuffer = new DepthBuffer(0, 0);
	}

	Camera::Camera(const std::wstring& name, ClearFlags clearFlag) : ObjectBehaviour(name), m_clearFlag(clearFlag)
	{
		m_colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_colorBuffer->SetClearColor(Color::Blue);
		m_colorBuffer->SetAsRenderTargetView();
		m_depthBuffer = new DepthBuffer(0, 0);
	}

	Camera::~Camera()
	{
		if (m_colorBuffer)
		{
			m_colorBuffer->Release();
			delete m_colorBuffer;
			m_colorBuffer = nullptr;
		}
		if (m_depthBuffer)
		{
			m_depthBuffer->Release();
			delete m_depthBuffer;
			m_depthBuffer = nullptr;
		}
	}

	RenderCore::Resource::ColorBuffer * Camera::GetColorBuffer()
	{
		//lock_guard<mutex> lock(m_rtvMutex);
		return m_colorBuffer;
	}

	RenderCore::Resource::DepthBuffer * Camera::GetDepthBuffer()
	{
		return m_depthBuffer;
	}

	void Camera::ClearFlag(ClearFlags flag)
	{
		m_clearFlag = flag;
		if (flag == ClearFlags::SolidColor)
		{
			m_clearColor = Color::Blue;
			m_colorBuffer->SetClearColor(m_clearColor);
		}
	}

	Camera::ClearFlags Camera::ClearFlag()
	{
		return m_clearFlag;
	}

	void Camera::ClearColor(Color color)
	{
		m_clearColor = color;
		m_colorBuffer->SetClearColor(m_clearColor);
	}

	Color Camera::ClearColor()
	{
		return m_clearColor;
	}

	ColorBuffer* Camera::FindForwordTarget(Vector3&& pos)
	{
		for (var i : cameraPool)
		{
			if (i != nullptr) return i->GetColorBuffer();
		}
		return nullptr;
	}
}