#include "Math/Vector4.h"
#include <cmath>

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum::Math {

	float Vector4::Length() const {

		return sqrt(LengthSquare());

	}


#ifdef USE_IMGUI

	void Vector4::DragGui(const char* label) {

		const float kThirtySecond = 0.03125f;

		ImGui::DragFloat4(label, &this->x, kThirtySecond);

	}

	void Vector4::ColorEdit(const char* label) {

		ImGui::ColorEdit4(label, &this->x);

	}

#endif

}