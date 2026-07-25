#pragma once

#include "Cast/StaticCast.h"
#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"
#include <numbers>

namespace Atrum {

	class Camera {

	protected:

		// 透視投影
		Math::Matrix4x4 perspectiveFovMatrix_ = Math::Matrix4x4::PerspectiveFov(0.5f, 1.77777f, 0.125f, 128.0f);

		// 正射影
		Math::Matrix4x4 orthographicMatrix_ = Math::Matrix4x4::Orthographic(0.0f, 0.0f, 1280.0f, 720.0f, 0.0001f, 100.0f);

		// 平行移動
		Math::Vector3 translate_{ 0.0f,0.0f, -10.0f };

		Math::Vector3 rotate_{};

		// クオータニオン
		Math::Quaternion quaternion_{};

		// ビュー行列
		Math::Matrix4x4 viewMatrix_{};


	public:

		virtual void Initialize() {}
		virtual void Update() {
			UpdateMatrix();
		}

		void CreateOrthographicMatrix(const int32_t clientWidth, const int32_t clientHeight) {
			orthographicMatrix_ = Math::Matrix4x4::Orthographic(0.0f, 0.0f, Cast::Float(clientWidth), Cast::Float(clientHeight), 0.0f, 100.0f);
		}

		void UpdateMatrix() {
			viewMatrix_ = Math::Matrix4x4::InverseRT(Math::Matrix4x4::ToLeftHanded(quaternion_.MakeRotateMatrixRh()), Math::Matrix4x4::Translate(translate_));
		}

		/* ゲッター */

		Math::Matrix4x4 GetViewMatrix() const { return viewMatrix_; }
		Math::Matrix4x4 GetPerspectiveFovMatrix() const { return perspectiveFovMatrix_; }
		Math::Matrix4x4 GetOrthographicMatrix() const { return orthographicMatrix_; }

		/* セッター */

		void SetPerspectiveFovMatrix(const Math::Matrix4x4& matrix) { perspectiveFovMatrix_ = matrix; }
		void SetOrthographicMatrix(const Math::Matrix4x4& matrix) { orthographicMatrix_ = matrix; }

		/* 加算 */

		void AddRotateFirstPerson(const Math::Vector3& add) {

			rotate_ += add;

			rotate_.x = std::clamp(rotate_.x, -std::numbers::pi_v<float> *-0.5f, std::numbers::pi_v<float> *0.5f);

			Math::Quaternion yawQ = Math::Quaternion::FromAxisAngle(Math::Vector3::Up(), rotate_.y);

			Math::Quaternion pitchQ = Math::Quaternion::FromAxisAngle(Math::Vector3::Right(), rotate_.x);

			Math::Quaternion rollQ = Math::Quaternion::FromAxisAngle(Math::Vector3::Forward(), rotate_.z);

			quaternion_ = (yawQ * pitchQ).Normalized();

		}

		void AddTranslate(const Math::Vector3& add) { translate_ += add; }

	};

}