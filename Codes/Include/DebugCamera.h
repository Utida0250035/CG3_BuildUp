#pragma once

#include "Camera.h"
#include "Vector2.h"
#include <memory>

class PlayInput;

class DebugCamera : public Camera {

private:

	PlayInput* input_ = nullptr;

	Vector3 pivot_{ 0.0f, 0.0f, 0.0f };
	float distance_ = 50.0f;

public:

	void Initialize();

	void Update();

#ifdef _DEBUG

	float* PGetDistance() { return &distance_; }

#endif

};