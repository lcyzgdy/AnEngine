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
		if (!Check()) return;

		var t = XMMatrixTranslationFromVector(XMLoadFloat3(&m_posG[index]));
		var r = XMMatrixRotationQuaternion(m_rotG[index]);
		var s = XMMatrixScalingFromVector(XMLoadFloat3(&m_scaG[index]));
		m_objectToLocal[index] = t * r * s;
	}

	bool TransformSystem::Check()
	{
		Length = m_posG.Data().size();
		if (m_rotG.Data().size() != Length) return false;
		if (m_scaG.Data().size() != Length) return false;
		return true;
	}
}