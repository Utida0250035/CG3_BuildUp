#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <cstdint>

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum {

	enum class LightModel : uint32_t {
		Lambert,
		HalfLambert
	};

	struct DirectionalLightData {
		// 平行光源の色
		Math::Vector3 color;
		float padding0;

		// 平行光源の向き
		Math::Vector3 direction;
		// 平行光源の輝度
		float intensity;

		// 光源の種類
		LightModel lightModel;
		float padding1[3];

		// 16 + 16 + 16 = 48
		// 残り208バイト分
		float padding2[52];

#ifdef USE_IMGUI

		void ImGui() {

			constexpr float kThirtySecond = 0.03125f;

			if (ImGui::BeginChild("DirectionalLight")) {

				ImGui::DragFloat3("color", &color.x, kThirtySecond, 0.0f, 1.0f);

				ImGui::DragFloat3("direction", &direction.x, kThirtySecond);

				if (ImGui::IsItemActive()) {

					direction.Normalize();

				}

				ImGui::DragFloat("intensity", &intensity, kThirtySecond);

				int lightMode = static_cast<int>(lightModel);

				ImGui::Selectable("lambert", lightMode == 0);
				if (ImGui::IsItemActivated()) {

					lightMode = 0;

				}

				ImGui::Selectable("halfLambert", lightMode == 1);
				if (ImGui::IsItemActivated()) {

					lightMode = 1;

				}

				lightModel = static_cast<LightModel>(lightMode);

			}

			ImGui::EndChild();

		}

#endif

	};

}