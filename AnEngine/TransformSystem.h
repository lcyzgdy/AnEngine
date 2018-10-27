#pragma once
#ifndef __TRANSFORMSYSTEM_H__
#define __TRANSFORMSYSTEM_H__

#include "IParallel.h"
#include "Transform.h"

namespace AnEngine::Game::System
{
	class TransformSystem : public IParallel
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

		//DataGroup m_data;
	public:
		// 通过 IParallel 继承
		virtual void Execute(int index) override;
	};
}

#endif // !__TRANSFORMSYSTEM_H__
