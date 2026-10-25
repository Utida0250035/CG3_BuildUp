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
  Math::Matrix4x4 perspectiveFovMatrix_ =
      Math::Matrix4x4::PerspectiveFov(0.5f, 1.77777f, 0.125f, 128.0f);

  // 正射影
  Math::Matrix4x4 orthographicMatrix_ = Math::Matrix4x4::Orthographic(
      0.0f, 0.0f, 1280.0f, 720.0f, 0.0001f, 100.0f);

  Math::Matrix4x4 viewProjectionMatrix_{};

  // 平行移動
  Math::Vector3 translate_{0.0f, 0.0f, -10.0f};

  Math::Vector3 deltaRotate_{};

  // クオータニオン
  Math::Quaternion quaternion_{};

  // ビュー行列
  Math::Matrix4x4 viewMatrix_{};

public:
  virtual void Initialize() {}
  virtual void Update() { UpdateMatrix(); }

  void CreateOrthographicMatrix(const int32_t clientWidth,
                                const int32_t clientHeight) {
    orthographicMatrix_ =
        Math::Matrix4x4::Orthographic(0.0f, 0.0f, Cast::Float(clientWidth),
                                      Cast::Float(clientHeight), 0.0f, 100.0f);
  }

  void UpdateMatrix() {
    viewMatrix_ =
        Math::Matrix4x4::InverseRT(quaternion_.MakeRotateMatrixLh(),
                                   Math::Matrix4x4::Translate(translate_));

    viewProjectionMatrix_ = viewMatrix_ * perspectiveFovMatrix_;

  }

  /* ゲッター */

  Math::Matrix4x4 GetViewMatrix() const { return viewMatrix_; }
  Math::Matrix4x4 GetPerspectiveFovMatrix() const {
    return perspectiveFovMatrix_;
  }

  Math::Matrix4x4 GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

  Math::Matrix4x4 GetOrthographicMatrix() const { return orthographicMatrix_; }

  Math::Vector3 GetTranslate() const { return translate_; }

  /* セッター */

  void SetPerspectiveFovMatrix(const Math::Matrix4x4 &matrix) {
    perspectiveFovMatrix_ = matrix;
  }
  void SetOrthographicMatrix(const Math::Matrix4x4 &matrix) {
    orthographicMatrix_ = matrix;
  }

  void SetTranslate(const Math::Vector3 &translate) { translate_ = translate; }

  /// <summary>
  /// 指定位置から注視点を向く（正射影スプライトと透視3Dの両方で共通の視点）
  /// </summary>
  void LookAt(const Math::Vector3 &eye, const Math::Vector3 &target,
              const Math::Vector3 &up = Math::Vector3::UpLh()) {
    translate_ = eye;
    quaternion_ = Math::Quaternion::FromLhLookAt(target, eye, up).Normalized();
  }

  /// <summary>
  /// 現在の視線を基準としたローカル回転をクオータニオンに乗算する
  /// </summary>
  void AddLocalRotation(const Math::Quaternion &localDelta) {
    quaternion_ = (quaternion_ * localDelta).Normalized();
  }

  /* 加算 */

  void AddRotateFirstPerson(const Math::Vector3 &add) {

    deltaRotate_ = add;

    deltaRotate_.x =
        std::clamp(deltaRotate_.x, -std::numbers::pi_v<float> * -0.5f,
                   std::numbers::pi_v<float> * 0.5f);

    Math::Quaternion yawQ =
        Math::Quaternion::FromAxisAngle(Math::Vector3::UpLh(), deltaRotate_.y);

    Math::Quaternion pitchQ = Math::Quaternion::FromAxisAngle(
        Math::Vector3::RightLh(), deltaRotate_.x);

    Math::Quaternion rollQ = Math::Quaternion::FromAxisAngle(
        Math::Vector3::ForwardLh(), deltaRotate_.z);

    quaternion_ = (pitchQ * yawQ * rollQ * quaternion_).Normalized();
  }

  void AddTranslate(const Math::Vector3 &add) { translate_ += add; }
};

} // namespace Atrum