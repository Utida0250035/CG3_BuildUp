#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Quaternion.h"
#include "Math/Vector3.h"

// #define DEPRECATE_OLD_TRANSFORM

#ifdef DEPRECATE_OLD_TRANSFORM

#define DEPRECATED_OLD_TRANSFORM                                               \
  [[deprecated("Use old struct: Transform with eulerRotate")]]

#else

#define DEPRECATED_OLD_TRANSFORM

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

  void ImGui(const char *label = "transform");

#endif
};

void to_json(nlohmann::json &json, const Transform &transform);
void from_json(const nlohmann::json &json, Transform &transform);

struct TransformLH {
  Vector3 scale = Vector3::One();
  Quaternion quaternion = Quaternion::Identity();
  Vector3 translate = Vector3::Zero();

  [[nodiscard]] Matrix4x4 MakeWorldMatrix() const {

    return Matrix4x4::World(translate, quaternion, scale);
  }

#ifdef USE_IMGUI

  void ImGui(const char *label = "transform");

#endif
};

void to_json(nlohmann::json &json, const TransformLH &transform);
void from_json(const nlohmann::json &json, TransformLH &transform);

struct TransformRH {
  Vector3 scale = Vector3::One();
  Quaternion quaternion = Quaternion::Identity();
  Vector3 translate = Vector3::Zero();

  [[nodiscard]] Matrix4x4 MakeWorldMatrix() const {

    return Matrix4x4::ToRightHanded(
        Matrix4x4::World(translate, quaternion, scale));
  }

#ifdef USE_IMGUI

  void ImGui(const char *label = "transform");

#endif
};

void to_json(nlohmann::json &json, const TransformRH &transform);
void from_json(const nlohmann::json &json, TransformRH &transform);

} // namespace Atrum::Math