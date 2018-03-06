#include "MCamera.h"
using namespace std;

namespace AnEngine
{
	MiCamera::MiCamera() :
		initialPosition(0, 0, 0), position(initialPosition),
		yawRotate(XM_PI), pitchRotate(0), lookDirection(0, 0, -1), upDirection(0, 1, 0),
		moveSpeed(20.0), turnSpeed(XM_PIDIV2), keyPressed{}
	{
	}

	MiCamera::~MiCamera()
	{
	}

	void MiCamera::OnInit(XMFLOAT3 _position)
	{
		initialPosition = _position;
		Reset();
	}

	void MiCamera::OnUpdate(float _elapsedSeconds)
	{
		XMFLOAT3 move(0, 0, 0);

		if (BaseInput::GetInstance()->GetKey(DIK_A))
		{
			move.x -= 1.0f;
		}
		if (BaseInput::GetInstance()->GetKey(DIK_D))
		{
			move.x += 1.0f;
		}
		if (BaseInput::GetInstance()->GetKey(DIK_W))
		{
			move.z -= 1.0f;
		}
		if (BaseInput::GetInstance()->GetKey(DIK_S))
		{
			move.z += 1.0f;
		}

		if (fabs(move.x) > 0.1f && fabs(move.z) > 0.1f)
		{
			XMVECTOR vector = XMVector3Normalize(XMLoadFloat3(&move));
			move.x = XMVectorGetX(vector);
			move.z = XMVectorGetZ(vector);
		}	// 向量转四元数

		float moveDelta = moveSpeed * _elapsedSeconds;
		float rotateDelta = turnSpeed * _elapsedSeconds;

		if (BaseInput::GetInstance()->GetKey(DIK_UP))
		{
			pitchRotate += rotateDelta;
		}
		if (BaseInput::GetInstance()->GetKey(DIK_DOWN))
		{
			pitchRotate -= rotateDelta;
		}
		if (BaseInput::GetInstance()->GetKey(DIK_LEFT))
		{
			yawRotate += rotateDelta;
		}
		if (BaseInput::GetInstance()->GetKey(DIK_RIGHT))
		{
			yawRotate -= rotateDelta;
		}

		pitchRotate = min(pitchRotate, XM_PIDIV4);
		pitchRotate = max(-XM_PIDIV4, pitchRotate);
		// 限制范围

		float x = move.x * -cosf(yawRotate) - move.z * sinf(yawRotate);
		float z = move.x * sinf(yawRotate) - move.z * cosf(yawRotate);
		position.x += x * moveDelta;
		position.z += z * moveDelta;
		// 在模型空间中移动摄像机

		float r = cosf(pitchRotate);
		lookDirection.x = r * sinf(yawRotate);
		lookDirection.y = sinf(pitchRotate);
		lookDirection.z = r * cosf(yawRotate);
		// 确定观察方向
	}

	void MiCamera::OnRelease()
	{
	}

	void MiCamera::OnKeyUp(UINT8 _key)
	{
#define true false
		switch (_key)
		{
		case 'W':
		{
			keyPressed.w = true;
			break;
		}
		case 'A':
		{
			keyPressed.a = true;
			break;
		}
		case 'S':
		{
			keyPressed.s = true;
		}
		case 'D':
		{
			keyPressed.d = true;
			break;
		}
		case VK_UP:
		{
			keyPressed.up = true;
			break;
		}
		case VK_DOWN:
		{
			keyPressed.down = true;
			break;
		}
		case VK_LEFT:
		{
			keyPressed.left = true;
			break;
		}
		case VK_RIGHT:
		{
			keyPressed.right = true;
			break;
		}
		}
#undef true
	}

	void MiCamera::OnKeyDown(UINT8 _key)
	{
		switch (_key)
		{
		case 'W':
		{
			keyPressed.w = true;
			break;
		}
		case 'A':
		{
			keyPressed.a = true;
			break;
		}
		case 'S':
		{
			keyPressed.s = true;
		}
		case 'D':
		{
			keyPressed.d = true;
			break;
		}
		case VK_UP:
		{
			keyPressed.up = true;
			break;
		}
		case VK_DOWN:
		{
			keyPressed.down = true;
			break;
		}
		case VK_LEFT:
		{
			keyPressed.left = true;
			break;
		}
		case VK_RIGHT:
		{
			keyPressed.right = true;
			break;
		}
		}
	}

	void MiCamera::Reset()
	{
		position = initialPosition;
		yawRotate = XM_PI;
		pitchRotate = 0.0f;
		lookDirection = { 0,0,-1 };
	}

	XMMATRIX MiCamera::GetViewMatrix()
	{
		return XMMatrixLookToRH(XMLoadFloat3(&position), XMLoadFloat3(&lookDirection), XMLoadFloat3(&upDirection));
	}

	XMMATRIX MiCamera::GetProjectionMatrix(float _fov, float _aspectRatio, float _nearPlane, float _farPlane)
	{
		return XMMatrixPerspectiveFovRH(_fov, _aspectRatio, _nearPlane, _farPlane);
	}

	void MiCamera::SetMoveSpeed(float v)
	{
		moveSpeed = v;
	}

	void MiCamera::SetTurnSpeed(float v)
	{
		turnSpeed = v;
	}


	LightCamera* LightCamera::m_camera = nullptr;

	LightCamera::LightCamera()
	{
		Reset();
		m_camera = this;
	}

	LightCamera::~LightCamera()
	{
		m_camera = nullptr;
	}

	LightCamera* LightCamera::get()
	{
		return m_camera;
	}

	void LightCamera::Get3DViewProjMatrices(XMFLOAT4X4 *view, XMFLOAT4X4 *proj, float fovInDegrees, float screenWidth, float screenHeight)
	{

		float aspectRatio = (float)screenWidth / (float)screenHeight;
		float fovAngleY = fovInDegrees * XM_PI / 180.0f;

		if (aspectRatio < 1.0f)
		{
			fovAngleY /= aspectRatio;
		}

		XMStoreFloat4x4(view, XMMatrixTranspose(XMMatrixLookAtRH(m_eye, m_at, m_up)));
		XMStoreFloat4x4(proj, XMMatrixTranspose(XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, 0.01f, 125.0f)));
	}

	void LightCamera::GetOrthoProjMatrices(XMFLOAT4X4 *view, XMFLOAT4X4 *proj, float width, float height)
	{
		XMStoreFloat4x4(view, XMMatrixTranspose(XMMatrixLookAtRH(m_eye, m_at, m_up)));
		XMStoreFloat4x4(proj, XMMatrixTranspose(XMMatrixOrthographicRH(width, height, 0.01f, 125.0f)));
	}
	void LightCamera::RotateYaw(float deg)
	{
		XMMATRIX rotation = XMMatrixRotationAxis(m_up, deg);

		m_eye = XMVector3TransformCoord(m_eye, rotation);
	}

	void LightCamera::RotatePitch(float deg)
	{
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(m_eye, m_up));
		XMMATRIX rotation = XMMatrixRotationAxis(right, deg);

		m_eye = XMVector3TransformCoord(m_eye, rotation);
	}

	void LightCamera::Reset()
	{
		m_eye = XMVectorSet(0.0f, 15.0f, -30.0f, 0.0f);
		m_at = XMVectorSet(0.0f, 8.0f, 0.0f, 0.0f);
		m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	}

	void LightCamera::Set(XMVECTOR eye, XMVECTOR at, XMVECTOR up)
	{
		m_eye = eye;
		m_at = at;
		m_up = up;
	}
}