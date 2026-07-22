#pragma once

#include "Cast/StaticCast.h"
#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"

namespace Atrum {

	class Camera {

	protected:

		// 透視投影
		Math::Matrix4x4 perspectiveFovMatrix_ = Math::Matrix4x4::PerspectiveFov(0.5f, 1.77777f, 0.125f, 128.0f);

		// 正射影
		Math::Matrix4x4 orthographicMatrix_ = Math::Matrix4x4::Orthographic(0.0f, 0.0f, 1280.0f, 720.0f, 0.0001f, 100.0f);

		// 平行移動
		Math::Vector3 translate_{ 0.0f,0.0f, -10.0f };

		// 回転
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
			viewMatrix_ = Math::Matrix4x4::InverseRT(quaternion_.MakeRotateMatrix(), Math::Matrix4x4::Translate(translate_));
		}

		/* ゲッター */

		Math::Matrix4x4 GetViewMatrix() const { return viewMatrix_; }
		Math::Matrix4x4 GetPerspectiveFovMatrix() const { return perspectiveFovMatrix_; }
		Math::Matrix4x4 GetOrthographicMatrix() const { return orthographicMatrix_; }

		/* セッター */

		void SetPerspectiveFovMatrix(const Math::Matrix4x4& matrix) { perspectiveFovMatrix_ = matrix; }
		void SetOrthographicMatrix(const Math::Matrix4x4& matrix) { orthographicMatrix_ = matrix; }

		/* 加算 */

		void AddRotate(const Math::Vector3& add) {

			Math::Vector3 up = quaternion_.RotateVector({ 0, 1, 0 });
			Math::Vector3 right = quaternion_.RotateVector({ 1,0,0 });
			Math::Vector3 forward = quaternion_.RotateVector({ 0, 0, 1 });

			Math::Quaternion yawQ = Math::Quaternion::FromAxisAngle(up, add.x);

			Math::Quaternion pitchQ = Math::Quaternion::FromAxisAngle(right, add.y);

			Math::Quaternion rollQ = Math::Quaternion::FromAxisAngle(forward, add.z);

			quaternion_ = (quaternion_ * yawQ * pitchQ * rollQ).Normalized();

		}

		void AddTranslate(const Math::Vector3& add) { translate_ += add; }

	};

}