#include "Camera.h"
#include "RenderCore.h"
#include "ThreadPool.hpp"

namespace AEngine::Game
{
	void Camera::OnInit()
	{
		ComponentBehaviour::OnInit();
		lock_guard<recursive_mutex> lock(m_recursiveMutex);
	}

	void Camera::BeforeUpdate()
	{
	}

	void Camera::AfterUpdate()
	{
	}

	void Camera::OnRelease()
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

	void Camera::BeginUpdate()
	{
	}
}