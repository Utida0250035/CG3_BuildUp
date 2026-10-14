#include "../System/HitJudge.h"
#include "../System/CmpHitSphere.h"
#include "../System/Entity.h"
#include <cassert>

#ifdef _DEBUG

#include <iostream>

#endif

namespace Atrum {

std::vector<Entity*> HitJudge::IsHitOthers(CmpHitSphere* cmpHitSphere) {

	assert(cmpHitSphere);

	if (!cmpHitSphere->GetIsActive()) {
		return {};
	}

	Entity& myOwner = cmpHitSphere->RefOwner();

	std::vector<Entity*> hitEntitys{};

	std::vector<HitSphere> myHitSpheres = cmpHitSphere->GetSpheres();

	Math::Vector3 myPosition{};
	Math::Vector3 otherPosition{};

	bool isHit = false;

	for (CmpHitSphere* cmp : cmpHitSpheres_) {

		if (cmp == cmpHitSphere)
			continue;

		if (!cmp->GetIsActive())
			continue;

		Entity& otherOwner = cmp->RefOwner();

		std::vector<HitSphere> otherHitSpheres = cmp->GetSpheres();

		for (const HitSphere& myHitSphere : myHitSpheres) {

			for (const HitSphere& otherHitSphere : otherHitSpheres) {

				myPosition = myOwner.GetWorldPosition() + myHitSphere.localOffset;

				otherPosition = otherOwner.GetWorldPosition() + otherHitSphere.localOffset;

				if ((otherPosition - myPosition).LengthSquare() <= (myHitSphere.radius + otherHitSphere.radius) * (myHitSphere.radius + otherHitSphere.radius)) {

					hitEntitys.push_back(&otherOwner);

					isHit = true;

					break;
				}
			}

			if (isHit)
				break;
		}
	}

	return hitEntitys;
}

void HitJudge::Register(CmpHitSphere* cmpHitSphere) { cmpHitSpheres_.push_back(cmpHitSphere); }

void HitJudge::Remove(CmpHitSphere* cmpHitSphere) { std::erase(cmpHitSpheres_, cmpHitSphere); }

} // namespace Atrum