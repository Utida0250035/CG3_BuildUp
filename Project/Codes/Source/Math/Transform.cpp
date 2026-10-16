#include "Math/Transform.h"

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum::Math {

	void Transform::ImGui(const char* label) {

		constexpr float kThirtySecond = 0.03125f;

		if (ImGui::BeginChild(label, ImGui::kChildSize, ImGui::kChildFlags)) {

			ImGui::Text(label);

			ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
			ImGui::DragFloat3("rotate", &rotate.x, kThirtySecond);
			ImGui::DragFloat3("translate", &translate.x, kThirtySecond);

		}

		ImGui::EndChild();

	}

	void TransformLH::ImGui(const char* label) {

		constexpr float kThirtySecond = 0.03125f;

		if (ImGui::BeginChild(label, ImGui::kChildSize, ImGui::kChildFlags)) {

			ImGui::Text(label);

			ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
			ImGui::DragFloat4("quaternion", &quaternion.x, kThirtySecond);

			if (ImGui::IsItemActive()) {

				quaternion.Normalize();

			}

			ImGui::DragFloat3("translate", &translate.x, kThirtySecond);

		}

		ImGui::EndChild();

	}

	void TransformRH::ImGui(const char* label) {

		constexpr float kThirtySecond = 0.03125f;

		if (ImGui::BeginChild(label, ImGui::kChildSize, ImGui::kChildFlags)) {

			ImGui::Text(label);

			ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
			ImGui::DragFloat4("quaternion", &quaternion.x, kThirtySecond);

			if (ImGui::IsItemActive()) {

				quaternion.Normalize();

			}

			ImGui::DragFloat3("translate", &translate.x, kThirtySecond);

		}

		ImGui::EndChild();

	}

}