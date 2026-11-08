#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorSceneRenderTarget.h"

#include "Engine/AtrumEngine.h"
#include <memory>

namespace Atrum::Editor {

namespace {

constexpr DXGI_FORMAT kColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT kDepthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

int ToTabIndex(const SceneViewTab tab) {
  switch (tab) {
  case SceneViewTab::Edit:
    return 0;
  case SceneViewTab::Game:
    return 1;
  case SceneViewTab::Runtime:
    return 2;
  }
  return 0;
}

} // namespace

void EditorSceneRenderTarget::ReleaseSrv(AtrumEngine *engine) {
  if (!srvAllocated_ || engine == nullptr) {
    return;
  }
  Atrum::DescriptorAllocator *allocator = engine->GetSrvAllocator();
  if (allocator != nullptr) {
    allocator->Free(srv_.index);
  }
  srv_ = {};
  srvAllocated_ = false;
}

void EditorSceneRenderTarget::CreateSrv(AtrumEngine *engine) {
  if (engine == nullptr || colorResource_ == nullptr) {
    return;
  }
  Atrum::DescriptorAllocator *allocator = engine->GetSrvAllocator();
  ID3D12Device *device = engine->GetD3DDevice();
  if (allocator == nullptr || device == nullptr) {
    return;
  }

  srv_ = allocator->Allocate();
  srvAllocated_ = true;

  D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
  srvDesc.Format = kColorFormat;
  srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
  srvDesc.Texture2D.MipLevels = 1;
  device->CreateShaderResourceView(colorResource_.Get(), &srvDesc, srv_.cpu);
}

bool EditorSceneRenderTarget::Ensure(AtrumEngine *engine, const int width,
                                     const int height) {
  if (engine == nullptr || width <= 0 || height <= 0) {
    return false;
  }
  if (width_ == width && height_ == height && colorResource_ != nullptr) {
    return true;
  }

  ReleaseSrv(engine);
  colorResource_.Reset();
  depthResource_.Reset();
  rtvAllocator_.reset();
  dsvAllocator_.reset();
  rtvReady_ = false;
  colorReadableByShader_ = false;

  width_ = width;
  height_ = height;

  ID3D12Device *device = engine->GetD3DDevice();
  if (device == nullptr) {
    return false;
  }

  D3D12_HEAP_PROPERTIES heapProps{};
  heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

  D3D12_RESOURCE_DESC colorDesc{};
  colorDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
  colorDesc.Width = static_cast<UINT>(width);
  colorDesc.Height = static_cast<UINT>(height);
  colorDesc.DepthOrArraySize = 1;
  colorDesc.MipLevels = 1;
  colorDesc.Format = kColorFormat;
  colorDesc.SampleDesc.Count = 1;
  colorDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

  D3D12_CLEAR_VALUE colorClear{};
  colorClear.Format = kColorFormat;
  colorClear.Color[0] = 0.0625f;
  colorClear.Color[1] = 0.1875f;
  colorClear.Color[2] = 0.125f;
  colorClear.Color[3] = 1.0f;

  HRESULT hr = device->CreateCommittedResource(
      &heapProps, D3D12_HEAP_FLAG_NONE, &colorDesc,
      D3D12_RESOURCE_STATE_RENDER_TARGET, &colorClear,
      IID_PPV_ARGS(colorResource_.GetAddressOf()));
  if (FAILED(hr)) {
    return false;
  }

  D3D12_RESOURCE_DESC depthDesc{};
  depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
  depthDesc.Width = static_cast<UINT>(width);
  depthDesc.Height = static_cast<UINT>(height);
  depthDesc.DepthOrArraySize = 1;
  depthDesc.MipLevels = 1;
  depthDesc.Format = kDepthFormat;
  depthDesc.SampleDesc.Count = 1;
  depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

  D3D12_CLEAR_VALUE depthClear{};
  depthClear.Format = kDepthFormat;
  depthClear.DepthStencil.Depth = 1.0f;

  hr = device->CreateCommittedResource(
      &heapProps, D3D12_HEAP_FLAG_NONE, &depthDesc,
      D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClear,
      IID_PPV_ARGS(depthResource_.GetAddressOf()));
  if (FAILED(hr)) {
    colorResource_.Reset();
    return false;
  }

  rtvAllocator_ = std::make_unique<Atrum::DescriptorAllocator>();
  rtvAllocator_->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false,
                            L"EditorSceneColorRtv", device);
  dsvAllocator_ = std::make_unique<Atrum::DescriptorAllocator>();
  dsvAllocator_->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false,
                            L"EditorSceneDepthDsv", device);

  colorRtvCpu_ = rtvAllocator_->GetCpuStart();
  depthDsvCpu_ = dsvAllocator_->GetCpuStart();

  D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
  rtvDesc.Format = kColorFormat;
  rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
  device->CreateRenderTargetView(colorResource_.Get(), &rtvDesc, colorRtvCpu_);

  D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
  dsvDesc.Format = kDepthFormat;
  dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
  device->CreateDepthStencilView(depthResource_.Get(), &dsvDesc, depthDsvCpu_);

  rtvReady_ = true;
  CreateSrv(engine);
  return srvAllocated_;
}

bool EditorSceneRenderTarget::Render(AtrumEngine *engine,
                                     const std::function<void()> &drawFn) {
  if (!rtvReady_ || engine == nullptr || !drawFn) {
    return false;
  }

  engine->EditorRenderSceneViewTarget(
      width_, height_, colorResource_.Get(), colorRtvCpu_,
      depthResource_.Get(), depthDsvCpu_, colorReadableByShader_, drawFn);
  colorReadableByShader_ = true;
  return true;
}

ImTextureRef EditorSceneRenderTarget::ImGuiTexture() const {
  if (!srvAllocated_) {
    return ImTextureRef{};
  }
  return ImTextureRef(static_cast<ImTextureID>(srv_.gpu.ptr));
}

bool EditorSceneRenderTarget::HasTexture() const { return srvAllocated_; }

EditorSceneRenderTargetHub &EditorSceneRenderTargetHub::Instance() {
  static EditorSceneRenderTargetHub hub;
  return hub;
}

void EditorSceneRenderTargetHub::RenderSceneViews(
    AtrumEngine *engine,
    const std::function<void(SceneViewTab tab, int width, int height)> &drawFn) {
  if (engine == nullptr || !drawFn) {
    return;
  }

  const EditorFrame &frame = EditorFrame::Instance();
  const SceneViewTab tabs[] = {SceneViewTab::Edit, SceneViewTab::Game,
                               SceneViewTab::Runtime};

  for (const SceneViewTab tab : tabs) {
    const SubViewportRect &request = frame.SceneViewRect(tab);
    if (!request.active || request.width <= 0 || request.height <= 0) {
      continue;
    }

    const int index = ToTabIndex(tab);
    EditorSceneRenderTarget &target = targets_[index];
    if (!target.Ensure(engine, request.width, request.height)) {
      continue;
    }

    target.Render(engine, [&drawFn, tab, request]() {
      drawFn(tab, request.width, request.height);
    });
  }
}

ImTextureRef EditorSceneRenderTargetHub::ImGuiTexture(const SceneViewTab tab) const {
  const int index = ToTabIndex(tab);
  return targets_[index].ImGuiTexture();
}

} // namespace Atrum::Editor

#endif
