#pragma once

#include "OBB.h"
#include "Bezier.h"

bool CheckCollision(const OBB& obb, const Vector2 p[3], float t1, float t2, int depth, float& hitT);

bool ResolveObbBezierResponse(OBB& obb, Vector2& velocity, const Vector2 p[3]);

bool ResolveRigidBodyObbBezierResponse(RigidBodyOBB& body, const Vector2 pBezier[3]);