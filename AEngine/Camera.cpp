#include "Camera.h"
#include "RenderCore.h"
#include "ThreadPool.hpp"
using namespace AEngine::RenderCore;
using namespace AEngine::RenderCore::Resource;

namespace AEngine::Game
{
	void Camera::OnInit()
	{
		lock_guard<recursive_mutex> lock(m_recursiveMutex);
		ComponentBehaviour::OnInit();
		m_colorBuffer = new ColorBuffer(Color::Blue);
		//m_depthBuffer = new DepthBuffer();

		if (m_enable) OnEnable();
	}

	void Camera::BeforeUpdate()
	{
		this_thread::sleep_for(std::chrono::milliseconds(100 / 6));
	}

	void Camera::Update()
	{

	}

	void Camera::AfterUpdate()
	{
	}

	void Camera::OnRelease()
	{
		lock_guard<recursive_mutex> lock(m_recursiveMutex);
		if (m_colorBuffer)
		{
			m_colorBuffer->Release();
			m_colorBuffer = nullptr;
		}
		if (m_depthBuffer)
		{
			m_depthBuffer->Release();
			m_depthBuffer = nullptr;
		}
		ComponentBehaviour::OnRelease();
	}

	Camera::Camera() :ComponentBehaviour(true)
	{
	}

	RenderCore::Resource::ColorBuffer * Camera::GetColorBuffer()
	{
		return m_colorBuffer;
	}

	RenderCore::Resource::DepthBuffer * Camera::GetDepthBuffer()
	{
		return m_depthBuffer;
	}
}