#pragma once
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <cstdint>

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum {

// 光源モデル
enum class LightModel : uint32_t { Lambert, HalfLambert };

// ライトフラグ
enum class LightFlags : uint32_t {
  // 無し
  NONE = 0,
  // 影を落とす
  CAST_SHADOW = 1 << 0,
  // 立体的な霧や空気感を表現する
  VOLUMETRIC = 1 << 1
};

// 平行光源(新)
struct LightDataDirectional {
  // 平行光源の色
  Math::Vector3 color = Math::Vector3::One();
  // 平行光源の輝度
  float intensity = 1.0f;

  // 平行光源の向き
  Math::Vector3 direction = Math::Vector3::DownLh();
  // アライメント用
  float padding0 = 0.0f;

  // ライトフラグ
  uint32_t flags = static_cast<uint32_t>(LightFlags::NONE);
  // 使用するシャドウマップのインデックス
  int32_t shadowMapIndex = -1;
  // ライトモデル
  LightModel lightModel = LightModel::HalfLambert;
  // アライメント用
  float padding1 = 0.0f;

  // アライメント用 256バイトまで 208バイト分
  float padding2[52] = {0.0f};
};

struct LightDataPoint {
  // 点光源の色
  Math::Vector3 color = Math::Vector3::One();
  // 点光源の輝度
  float intensity = 1.0f;

  // 点光源の位置
  Math::Vector3 position{};
  // 点光源の影響半径
  float range = 10.0f;

  // 点光源の減衰度合い
  float decay = 1.0f;
  // LightFlagsビット演算用
  uint32_t flags = 0;
  // シャドウマップ用インデックス (-1なら影無し)
  int32_t shadowMapIndex = -1;
  // アライメント用
  float padding0 = 0.0f;

  // アライメント用 256バイトまで 208バイト分
  float padding1[52] = {0.0f};
};

struct LightDataSpot {
  // スポットライトの色
  Math::Vector3 color = Math::Vector3::One();
  // スポットライトの輝度
  float intensity = 1.0f;

  // スポットライトの位置
  Math::Vector3 position{};
  // スポットライトの影響半径
  float range = 10.0f;

  // スポットライトの向き
  Math::Vector3 direction = Math::Vector3::One();
  // 減衰度合い
  float decay = 1.0f;

  // 照射角(コサイン値) : 外側(光が完全に消える角度)
  float cosAngle = 0.5f;
  // 照射角(コサイン値) : 内側(減衰せずに明るさを保つ角度)
  float cosInnerAngle = 0.7f;
  // LightFlagsビット演算用
  uint32_t flags = 0;
  // シャドウマップ用インデックス(-1なら影無し)
  int32_t shadowMapIndex = -1;

  // アライメント用 256バイトまで 192バイト分
  float padding0[48] = {0.0f};
};

constexpr uint32_t kMaxPointLights = 32;
constexpr uint32_t kMaxSpotLights = 16;

struct SceneLightData {
  // 平行光源
  LightDataDirectional directionalLight{};

  // 各ライトの有効数
  uint32_t pointLightCount = 0;
  uint32_t spotLightCount = 0;
  float padding[2] = {0.0f};

  // 各ライトの配列
  LightDataPoint pointLights[kMaxPointLights]{};
  LightDataSpot spotLights[kMaxSpotLights]{};
};

#pragma region old 現行の古い実装

// 平行光源(現)
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

  void ImGui();

#endif
};

#pragma endregion

} // namespace Atrum