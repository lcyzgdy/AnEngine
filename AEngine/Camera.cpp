#include "Camera.h"
#include "RenderCore.h"
#include "ThreadPool.hpp"
#include "Screen.h"
using namespace AEngine::RenderCore;
using namespace AEngine::RenderCore::Resource;

namespace AEngine::Game
{
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
	}

	void Camera::OnActive()
	{
	}

	void Camera::Update()
	{
		this_thread::sleep_for(std::chrono::milliseconds(6));
	}

	void Camera::AfterUpdate()
	{
		RenderCore::RenderColorBuffer(m_colorBuffer);
	}

	void Camera::OnInvalid()
	{
	}

	void Camera::Destory()
	{

	}

	Camera::Camera() : ObjectBehaviour(), m_clearFlag(ClearFlags::SkyBox)
	{
		//m_colorBuffer = new ColorBuffer(Color::Blue);
		//m_colorBuffer->Create(this->name, Screen::GetInstance()->Width(), Screen::GetInstance()->Height(), DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UINT);
		m_colorBuffer = new ColorBuffer(L"", RenderCore::r_graphicCard[0], Screen::GetInstance()->Width(),
			Screen::GetInstance()->Height(), 1, DXGI_FORMAT_R8G8B8A8_UNORM);

		m_depthBuffer = new DepthBuffer(0, 0);
	}

	Camera::Camera(ClearFlags clearFlag) : m_clearFlag(clearFlag)
	{
		m_colorBuffer = new ColorBuffer(Color::Blue);
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
		return m_colorBuffer;
	}

	RenderCore::Resource::DepthBuffer * Camera::GetDepthBuffer()
	{
		return m_depthBuffer;
	}

	void Camera::ClearFlag(ClearFlags flag)
	{
		m_clearFlag = flag;
	}

	Camera::ClearFlags Camera::ClearFlag()
	{
		return m_clearFlag;
	}

	void Camera::ClearColor(Color color)
	{
		m_clearColor = color;
	}

	Color Camera::ClearColor()
	{
		return m_clearColor;
	}
}