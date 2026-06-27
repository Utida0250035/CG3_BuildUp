#pragma once

#include "Camera.h"
#include "Vector2.h"
#include <memory>

class PlayInput;

enum class DebugCameraMode {
	FREE, ORBIT
};

class DebugCamera : public Camera {

private:

	DebugCameraMode mode_ = DebugCameraMode::FREE;

	PlayInput* input_ = nullptr;

	Vector3 pivot_{ 0.0f, 0.0f, 0.0f };
	float distance_ = 50.0f;
	Quaternion pivotQuaternion_{};

public:

	void Initialize();

	void Update();

#ifdef _DEBUG

	float& RefDistance() { return distance_; }
	Vector3& RefTranslate() { return translate_; }
	Quaternion& RefQuaternion() { return quaternion_; }
	Quaternion& RefPivotQuaternion() { return pivotQuaternion_; }
	Vector3& RefPivot() { return pivot_; };
	DebugCameraMode GetMode() const { return mode_; }

#endif

};