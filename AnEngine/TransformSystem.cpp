#include "TransformSystem.h"
using namespace std;
using namespace AnEngine::DMath;

namespace AnEngine::Game::System
{
	TransformSystem::TransformSystem(ComponentGroup<Component::Position>& poses, ComponentGroup<Component::Rotation>& rots,
		ComponentGroup<Component::Scale>& scas, ComponentGroup<Component::Matrix4x4>& obj2local,
		ComponentGroup<Component::Matrix4x4>& local2world, ComponentGroup<Component::Matrix4x4>&obj2world) :
		m_posG(poses), m_rotG(rots), m_scaG(scas), m_objectToLocal(obj2local), m_localToWorld(local2world), m_objectToWorld(local2world)
	{
		Length = m_posG.Data().size();
		if (rots.Data().size != Length || scas.Data().size() != Length) throw exception("长度不对");
	}

	void TransformSystem::Execute(int index)
	{
		//var[pPosition, pRotation, pScale, pLocalPos, pLocalRot, pLocalScale, pMatrix] = m_data[index];
		/*Matrix4x4 t(0, 0, 0, (*pPosition).x,
			0, 0, 0, (*pPosition).y,
			0, 0, 0, (*pPosition).z,
			0, 0, 0, 1);*/

			/*var t = XMMatrixTranslationFromVector(XMLoadFloat3(pPosition));
			var p = XMMatrixRotationQuaternion(*pRotation);
			var s = XMMatrixScalingFromVector(XMLoadFloat3(pScale));
			*pMatrix = t * p * s;*/
	}

	bool TransformSystem::Check()
	{
		Length = m_posG.Data().size();
		if (m_rotG.Data().size() != Length) return false;
		if (m_scaG.Data().size() != Length) return false;
		return true;
	}
}