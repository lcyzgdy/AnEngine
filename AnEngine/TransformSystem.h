#pragma once
#ifndef __TRANSFORMSYSTEM_H__
#define __TRANSFORMSYSTEM_H__

#include "IParallel.h"
#include "Transform.h"
#include "ComponentGroup.hpp"
#include "SystemBase.h"

namespace AnEngine::Game::System
{
	class TransformSystem : public SystemBase, public IParallel
	{
		/*struct DataGroup : public IComponent<Position, Rotation, Scale, LocalPosition, LocalRotation, LocalScale, TransformMatrix>
		{
			std::vector<Position*> position;
			std::vector<Rotation*> rotation;
			std::vector<Scale*> scale;
			std::vector<LocalPosition*> localPosition;
			std::vector<LocalRotation*> localRotation;
			std::vector<LocalScale*> localScale;
			std::vector<TransformMatrix*> transformMatrix;

			std::tuple<Position*, Rotation*, Scale*, LocalPosition*, LocalRotation*, LocalScale*, TransformMatrix*> operator[](int i) override
			{
				return { position[i], rotation[i], scale[i], localPosition[i], localRotation[i], localScale[i], transformMatrix[i] };
			}
		};*/

		ComponentGroup<Component::Position>& m_posG;
		ComponentGroup<Component::Rotation>& m_rotG;
		ComponentGroup<Component::Scale>& m_scaG;
		ComponentGroup<Component::Matrix4x4>& m_objectToLocal;
		ComponentGroup<Component::Matrix4x4>& m_localToWorld;
		ComponentGroup<Component::Matrix4x4>& m_objectToWorld;

	public:
		explicit TransformSystem(std::deque<GameObject*>& objInScene, ComponentGroup<Component::Position>& poses, ComponentGroup<Component::Rotation>& rots,
			ComponentGroup<Component::Scale>& scas, ComponentGroup<Component::Matrix4x4>& obj2local,
			ComponentGroup<Component::Matrix4x4>& local2world, ComponentGroup<Component::Matrix4x4>&obj2world);
		// 通过 IParallel 继承
		virtual void Execute(int index) override;
		virtual bool Check(int index) override;
	};
}

#endif // !__TRANSFORMSYSTEM_H__
