#include "TransformSystem.h"
using namespace std;
using namespace AnEngine::DMath;

namespace AnEngine::Game::System
{
	TransformSystem::TransformSystem(deque<GameObject*>& objInScene, ComponentGroup<Component::Position>& poses, ComponentGroup<Component::Rotation>& rots,
		ComponentGroup<Component::Scale>& scas, ComponentGroup<Component::Matrix4x4>& obj2local,
		ComponentGroup<Component::Matrix4x4>& local2world, ComponentGroup<Component::Matrix4x4>&obj2world) :
		SystemBase(objInScene),
		m_posG(poses), m_rotG(rots), m_scaG(scas), m_objectToLocal(obj2local), m_localToWorld(local2world), m_objectToWorld(local2world)
	{
		Length = m_posG.Data().size();
		if (rots.Data().size() != Length || scas.Data().size() != Length) throw exception("长度不对");
	}

	void TransformSystem::Execute(int index)
	{
		if (!Check(index)) return;

		var t = XMMatrixTranslationFromVector(XMLoadFloat3(&m_posG[index]));
		var r = XMMatrixRotationQuaternion(m_rotG[index]);
		var s = XMMatrixScalingFromVector(XMLoadFloat3(&m_scaG[index]));
		m_objectToLocal[index] = t * r * s;

		m_localToWorld[index] = XMMatrixIdentity();

		var parent = m_objectsInScene[index]->Parent();
		while (parent != nullptr)
		{
			uint32_t id = parent->Id();
			var S = XMMatrixScalingFromVector(XMLoadFloat3(&m_posG[id]));
			var T = XMMatrixTranslationFromVector(XMLoadFloat3(&m_scaG[id]));
			var R = XMMatrixRotationQuaternion(m_rotG[id]);
			m_localToWorld[index] *= T * R * S;
			parent = parent->Parent();
		}
	}

	bool TransformSystem::Check(int index)
	{
		//if (!m_activeObj[index]) return false;
		if (m_objectsInScene[index]->Destoryed()) return false;
		return true;
	}
}