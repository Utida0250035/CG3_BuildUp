#pragma once

#include "Camera/Camera.h"
#include "Math/Vector2.h"
#include <memory>

#ifdef _DEBUG

namespace Atrum::Input {

	class PlayInput;

}

#endif

namespace Atrum::Debug {

#ifdef _DEBUG

	enum class DebugCameraMode {
		FREE, ORBIT
	};

#endif

	class DebugCamera : public Camera {

#ifdef _DEBUG

	private:

		DebugCameraMode mode_ = DebugCameraMode::FREE;

		Input::PlayInput* input_ = nullptr;

		Math::Vector3 pivot_{ 0.0f, 0.0f, 0.0f };
		float distance_ = 50.0f;

#endif

	public:

		void Initialize() override;

		void Update() override;

#ifdef _DEBUG

		float& RefDistance() { return distance_; }
		Math::Vector3& RefTranslate() { return translate_; }
		Math::Quaternion& RefQuaternion() { return quaternion_; }
		Math::Vector3& RefPivot() { return pivot_; };
		DebugCameraMode GetMode() const { return mode_; }

#endif

	};

}