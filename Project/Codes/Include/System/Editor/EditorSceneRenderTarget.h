#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorFrame.h"
#include "Engine/Resource/DescriptorAllocator.h"
#include "ForDebug/ImGui.h"
#include <d3d12.h>
#include <functional>
#include <memory>
#include <wrl/client.h>

namespace Atrum {
class AtrumEngine;
}

namespace Atrum::Editor {

/// 1 つのシーンビュー用カラー＋深度 RT と ImGui 表示用 SRV
class EditorSceneRenderTarget {
public:
  bool Ensure(Atrum::AtrumEngine *engine, int width, int height);
  bool Render(Atrum::AtrumEngine *engine, const std::function<void()> &drawFn);
  ImTextureRef ImGuiTexture() const;
  bool HasTexture() const;

private:
  void ReleaseSrv(Atrum::AtrumEngine *engine);
  void CreateSrv(Atrum::AtrumEngine *engine);

  int width_ = 0;
  int height_ = 0;
  Microsoft::WRL::ComPtr<ID3D12Resource> colorResource_{};
  Microsoft::WRL::ComPtr<ID3D12Resource> depthResource_{};
  Atrum::DescriptorAllocator::DescriptorHandle srv_{};
  bool srvAllocated_ = false;
  std::unique_ptr<Atrum::DescriptorAllocator> rtvAllocator_{};
  std::unique_ptr<Atrum::DescriptorAllocator> dsvAllocator_{};
  D3D12_CPU_DESCRIPTOR_HANDLE colorRtvCpu_{};
  D3D12_CPU_DESCRIPTOR_HANDLE depthDsvCpu_{};
  bool rtvReady_ = false;
  /// 直近のパス後にカラーが SRV 読取可能状態か
  bool colorReadableByShader_ = false;
};

class EditorSceneRenderTargetHub {
public:
  static EditorSceneRenderTargetHub &Instance();

  void RenderSceneViews(
      Atrum::AtrumEngine *engine,
      const std::function<void(SceneViewTab tab, int width, int height)> &drawFn);

  ImTextureRef ImGuiTexture(const SceneViewTab tab) const;

private:
  EditorSceneRenderTarget targets_[3]{};
};

} // namespace Atrum::Editor

#endif
