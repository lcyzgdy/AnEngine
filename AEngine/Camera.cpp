#include "Camera.h"
#include "RenderCore.h"
#include "ThreadPool.hpp"

namespace AEngine::Game
{
	void Camera::OnInit()
	{
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
}