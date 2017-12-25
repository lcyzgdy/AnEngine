#include "Camera.h"
#include "RenderCore.h"
#include "ThreadPool.hpp"

namespace AEngine::Game
{
	void Camera::OnInit()
	{
		if (m_enable)
		{
			Utility::u_s_threadPool.Commit(std::bind(Camera::BeginUpdate, this));
		}
	}

	void Camera::BeforeUpdate()
	{
	}

	void Camera::OnUpdate()
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