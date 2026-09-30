#pragma once

#include "Camera/Camera.h"
#include "Math/Vector2.h"
#include <memory>

#ifdef DEVELOPMENT

namespace Atrum::Input {

	class PlayInput;

}

#endif

namespace Atrum::Debug {

#ifdef DEVELOPMENT

	enum class DebugCameraMode {
		FREE, ORBIT
	};

#endif

	class DebugCamera : public Camera {

#ifdef DEVELOPMENT

	private:

		DebugCameraMode mode_ = DebugCameraMode::FREE;

		Input::PlayInput* input_ = nullptr;

		Math::Vector3 pivot_{ 0.0f, 0.0f, 0.0f };
		float distance_ = 50.0f;

#endif

	public:

		void Initialize() override;

		void Update() override;

#ifdef DEVELOPMENT

		float& RefDistance() { return distance_; }
		Math::Vector3& RefTranslate() { return translate_; }
		Math::Quaternion& RefQuaternion() { return quaternion_; }
		Math::Vector3& RefPivot() { return pivot_; };
		DebugCameraMode GetMode() const { return mode_; }

#endif

	};

}