#pragma once

#include "Geometry/Bezier.h"
#include "Geometry/OBB.h"

namespace Atrum::Physics {

	bool CheckCollision(const Geometry::MyOBB& obb, const Math::Vector2 p[3], float t1, float t2, int depth, float& hitT);

	bool CheckCollisionDetailed(const Geometry::MyOBB& obb, const Math::Vector2& obbVertexPos, const Math::Vector2 p[3], float& t1, float& t2, int depthCount, const int depth);

	bool ResolveObbBezierResponse(Geometry::MyOBB& obb, Math::Vector2& velocity, const Math::Vector2 p[3]);

	bool ResolveRigidBodyObbBezierResponse(RigidBodyOBB& body, const Math::Vector2 pBezier[3]);

	bool ResolveRigidBodyObbBezierResponseDetailed(RigidBodyOBB& body, const Math::Vector2 pBezier[3], float restitution, float reboundPercent, float allowRange, int depth);

	void ProcessPointToSegment(RigidBodyOBB& body, const Math::Vector2& point, const Math::Vector2& segmentStart, const Math::Vector2& segmentEnd);

	void ProcessPointToSegmentInBezier(RigidBodyOBB& body, const Math::Vector2& point, const Math::Vector2 pBezier[3], float t1, float t2, float restitution, float reboundPercent, float allowRange);

	bool ProcessPointToOBBEdges(RigidBodyOBB& body, const Math::Vector2& pWorld, const float restitution, const float boundPercent, const float allowRange);

	void ProcessPointInBezierToOBBEdges(RigidBodyOBB& body, const Math::Vector2 pBezier[3], float t, float restitution, float reboundPercent, float allowRange);

	void ApplyImpulse(RigidBodyOBB& obb, const Math::Vector2& worldContact, const Math::Vector2& normal, const float overlap, const float restitution, const float reboundPercent, const float allowRange);

}