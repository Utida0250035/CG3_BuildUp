#pragma once

#include "Camera.h"
#include <memory>

class PlayInput;

class DebugCamera : public Camera {

private:

	PlayInput* input_ = nullptr;

	Matrix4x4 culmrativeRotateMatrix_{};

public:

	void Initialize();

	void Update();

};