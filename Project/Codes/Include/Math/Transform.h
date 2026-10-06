#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

//#define DEPRECATE_OLD_TRANSFORM

#ifdef DEPRECATE_OLD_TRANSFORM

#define DEPRECATED_OLD_TRANSFORM [[deprecated("Use old struct: Transform with eulerRotate")]]

#else

#define DEPRECATED_OLD_TRANSFORM

#endif

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum::Math {

	struct DEPRECATED_OLD_TRANSFORM Transform {
		Vector3 scale = Vector3::One();
		Vector3 rotate = Vector3::Zero();
		Vector3 translate = Vector3::Zero();

		/// <summary>
		/// ワールド行列の作成
		/// </summary>
		/// <param name="transform"> Transform </param>
		/// <returns> ワールド行列 </returns>
		[[nodiscard]] Matrix4x4 MakeWorldMatrix() const {

			return Matrix4x4::World(translate, scale, rotate);

		}

#ifdef USE_IMGUI

		void ImGui(const std::string& label = "transform") {

			constexpr float kThirtySecond = 0.03125f;

			if (ImGui::BeginChild(label.c_str(), ImGui::kChildSize, ImGui::kChildFlags)) {

				ImGui::Text(label.c_str());

				ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
				ImGui::DragFloat3("rotate", &rotate.x, kThirtySecond);
				ImGui::DragFloat3("translate", &translate.x, kThirtySecond);

			}

			ImGui::EndChild();

		}

#endif

	};

	struct TransformLH {
		Vector3 scale = Vector3::One();
		Quaternion quaternion = Quaternion::Identity();
		Vector3 translate = Vector3::Zero();

		[[nodiscard]] Matrix4x4 MakeWorldMatrix() const {

			return Matrix4x4::World(translate, quaternion, scale);

		}

#ifdef USE_IMGUI

		void ImGui(const std::string& label = "transform") {

			constexpr float kThirtySecond = 0.03125f;

			if (ImGui::BeginChild(label.c_str(), ImGui::kChildSize, ImGui::kChildFlags)) {

				ImGui::Text(label.c_str());

				ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
				ImGui::DragFloat4("quaternion", &quaternion.x, kThirtySecond);

				if (ImGui::IsItemActive()) {

					quaternion.Normalize();

				}

				ImGui::DragFloat3("translate", &translate.x, kThirtySecond);

			}

			ImGui::EndChild();

		}

#endif

	};

	struct TransformRH {
		Vector3 scale = Vector3::One();
		Quaternion quaternion = Quaternion::Identity();
		Vector3 translate = Vector3::Zero();

		[[nodiscard]] Matrix4x4 MakeWorldMatrix() const {

			return Matrix4x4::ToRightHanded(Matrix4x4::World(translate, quaternion, scale));

		}

#ifdef USE_IMGUI

		void ImGui(const std::string& label = "transform") {

			constexpr float kThirtySecond = 0.03125f;

			if (ImGui::BeginChild(label.c_str(), ImGui::kChildSize, ImGui::kChildFlags)) {

				ImGui::Text(label.c_str());

				ImGui::DragFloat3("scale", &scale.x, kThirtySecond);
				ImGui::DragFloat4("quaternion", &quaternion.x, kThirtySecond);

				if (ImGui::IsItemActive()) {

					quaternion.Normalize();

				}

				ImGui::DragFloat3("translate", &translate.x, kThirtySecond);

			}

			ImGui::EndChild();

		}

#endif

	};

}