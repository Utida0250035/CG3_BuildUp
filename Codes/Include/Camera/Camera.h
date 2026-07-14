#pragma once

#include "Cast/StaticCast.h"
#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"

class Camera {

protected:

	// 透視投影
	Matrix4x4 perspectiveFovMatrix_ = MakePerspectiveFovMatrix(0.5f, 1.77777f, 0.125f, 128.0f);

	// 正射影
	Matrix4x4 orthographicMatrix_ = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0001f, 100.0f);

	// 平行移動
	Vector3 translate_{};

	// 回転
	Quaternion quaternion_{};

	// ビュー行列
	Matrix4x4 viewMatrix_{};


public:

	virtual void Initialize() {}
	virtual void Update() {}

	void CreateOrthographicMatrix(const int32_t clientWidth, const int32_t clientHeight) {
		orthographicMatrix_ = MakeOrthographicMatrix(0.0f, 0.0f, cast::Float(clientWidth), cast::Float(clientHeight), 0.0f, 100.0f);
	}

	void UpdateMatrix() {
		viewMatrix_ = RTMatrixInverse(quaternion_.create_rotate_matrix(), MakeTranslateMatrix(translate_));
	}

	/* ゲッター */

	Matrix4x4 GetViewMatrix() const { return viewMatrix_; }
	Matrix4x4 GetPerspectiveFovMatrix() const { return perspectiveFovMatrix_; }
	Matrix4x4 GetOrthographicMatrix() const { return orthographicMatrix_; }

	/* セッター */

	void SetPerspectiveFovMatrix(const Matrix4x4& matrix) { perspectiveFovMatrix_ = matrix; }
	void SetOrthographicMatrix(const Matrix4x4& matrix) { orthographicMatrix_ = matrix; }

	/* 加算 */

	void AddRotate(const Vector3& add) {
		
		Vector3 up = quaternion_.rotate_vector({ 0, 1, 0 });
		Vector3 right = quaternion_.rotate_vector({ 1,0,0 });
		Vector3 forward = quaternion_.rotate_vector({ 0, 0, 1 });

		Quaternion yawQ = Quaternion::FromAxisAngle(up, add.x);

		Quaternion pitchQ = Quaternion::FromAxisAngle(right, add.y);

		Quaternion rollQ = Quaternion::FromAxisAngle(forward, add.z);

		quaternion_ = (quaternion_ * yawQ * pitchQ * rollQ).normalized();

	}

	void AddTranslate(const Vector3& add) { translate_ += add; }

};