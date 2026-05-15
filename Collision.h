#pragma once

#include "OBB.h"
#include "Bezier.h"

bool CheckCollision(const OBB& obb, const Vector2 p[3], float t1, float t2, int depth, float& hitT);

bool CheckCollisionDetailed(const OBB& obb, const Vector2 p[3], float& t1, float& t2, int depth);

bool ResolveObbBezierResponse(OBB& obb, Vector2& velocity, const Vector2 p[3]);

bool ResolveRigidBodyObbBezierResponse(RigidBodyOBB& body, const Vector2 pBezier[3]);

bool ResolveRigidBodyObbBezierResponseDetailed(RigidBodyOBB& body, const Vector2 pBezier[3]);

void ProcessPointToSegment(RigidBodyOBB& body, const Vector2& point, const Vector2& segmentStart, const Vector2& segmentEnd);

void ProcessPointToOBBEdges(RigidBodyOBB& body, const Vector2& pWorld);

void ApplyImpulse(RigidBodyOBB& obb, const Vector2& worldContact, const Vector2& normal, const float overlap, const float restitution, const float reboundPercent, const float allowRange);