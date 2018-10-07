#include "Camera.h"
#include "RenderCore.h"
#include "ThreadPool.hpp"
#include "Screen.h"
#include "CommandContext.h"
#include "DTimer.h"

using namespace AnEngine::RenderCore;
using namespace AnEngine::RenderCore::Resource;
using namespace AnEngine::DMath;

namespace AnEngine::Game
{
	static vector<Camera*> cameraPool;
	static mutex poolMutex;

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
		var depthBuffer = m_depthBuffers.GetBack();

		ThrowIfFailed(iAllocator->Reset());
		ThrowIfFailed(iList->Reset(iAllocator, nullptr));

		var commonToRenderTarget = CommonState::commonToRenderTarget;
		var renderTargetToCommon = CommonState::renderTargetToCommon;
		commonToRenderTarget.Transition.pResource = frameBuffer->GetResource();
		renderTargetToCommon.Transition.pResource = frameBuffer->GetResource();

		iList->ResourceBarrier(1, &commonToRenderTarget);
		iList->ClearRenderTargetView(frameBuffer->GetRtv(), m_clearColor.GetPtr(), 0, nullptr);
		iList->ClearDepthStencilView(depthBuffer->GetDsv(), D3D12_CLEAR_FLAG_DEPTH,
			depthBuffer->GetClearDepth(), 0, 0, nullptr);
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
			var colorBuffer = new ColorBuffer(ColorBuffer::DescribeAsGBuffer(Screen::GetInstance()->Width(), Screen::GetInstance()->Height()));
			colorBuffer->SetAsRtv();

			m_colorBuffers.ManageBuffer(colorBuffer);
		}
		{
			var colorBuffer = new ColorBuffer(ColorBuffer::DescribeAsGBuffer(Screen::GetInstance()->Width(), Screen::GetInstance()->Height()));
			colorBuffer->SetAsRtv();

			m_colorBuffers.ManageBuffer(colorBuffer);
		}
		{
			var depthBuffer = new DepthBuffer(Screen::GetInstance()->Width(), Screen::GetInstance()->Height());
			depthBuffer->SetAsDsv();

			m_depthBuffers.ManageBuffer(depthBuffer);
		}
		{
			var depthBuffer = new DepthBuffer(Screen::GetInstance()->Width(), Screen::GetInstance()->Height());
			depthBuffer->SetAsDsv();

			m_depthBuffers.ManageBuffer(depthBuffer);
		}
	}

	Camera::Camera(ClearFlags clearFlag) : ObjectBehaviour(), m_clearFlag(clearFlag)
	{
		{
			var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
				Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
			colorBuffer->SetAsRtv();
			m_colorBuffers.ManageBuffer(colorBuffer);
		}
		{
			var colorBuffer = new ColorBuffer(L"", Screen::GetInstance()->Width(),
				Screen::GetInstance()->Height(), 1, 4, DXGI_FORMAT_R8G8B8A8_UNORM);
			colorBuffer->SetAsRtv();
			m_colorBuffers.ManageBuffer(colorBuffer);
		}
	}

	Camera::~Camera()
	{
	}

	void Camera::ClearFlag(ClearFlags flag)
	{
		m_clearFlag = flag;
		if (flag == ClearFlags::SolidColor)
		{
			m_clearColor = Color::Blue;
		}
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

	void Camera::ResetProjectionMatrix()
	{

	}
}