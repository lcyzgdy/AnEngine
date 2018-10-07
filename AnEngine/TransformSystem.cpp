#include "TransformSystem.h"
using namespace AnEngine::DMath;

namespace AnEngine::Game
{
	void TransformSystem::Execute(int index)
	{
		var[pPosition, pRotation, pScale, pLocalPos, pLocalRot, pLocalScale, pMatrix] = m_data[index];
		/*Matrix4x4 t(0, 0, 0, (*pPosition).x,
			0, 0, 0, (*pPosition).y,
			0, 0, 0, (*pPosition).z,
			0, 0, 0, 1);*/

		var t = XMMatrixTranslationFromVector(XMLoadFloat3(pPosition));
		var p = XMMatrixRotationQuaternion(*pRotation);
		var s = XMMatrixScalingFromVector(XMLoadFloat3(pScale));
		*pMatrix = t * p * s;
	}
}