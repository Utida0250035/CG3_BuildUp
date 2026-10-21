#pragma once

#include "Engine/EngineFwd.h"
#include "Engine/Resource/ConstantBuffer.h"
#include "Engine/Resource/LightData.h"
#include <memory>

namespace Atrum::Visual {

// 光源データ管理クラス
class Lighting final {
private:
  // CPU側の実データ
  SceneLightData sceneLightData_{};

  // GPU転送用の定数バッファ
  std::unique_ptr<SingleConstantBuffer<SceneLightData>> lightingBuffer_ =
      nullptr;

public:
  Lighting() = default;
  ~Lighting() = default;

  // 初期化 定数バッファの生成
  void Initialize(ID3D12Device *device);

  // 毎フレームの更新とGPUへのバインド処理
  void Update(CommandContext *CommandContext, UINT rootParameterIndex);

  // データの参照
  SceneLightData &RefLightData() { return sceneLightData_; }
  // データの取得
  const SceneLightData &GetSceneLightData() const { return sceneLightData_; }

#ifdef USE_IMGUI

  // デバッグ用GUI
  void ImGui();

#endif

  // コピー禁止

  Lighting(const Lighting &) = delete;
  Lighting &operator=(const Lighting &) = delete;
};

} // namespace Atrum::Visual