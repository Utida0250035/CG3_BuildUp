#include "Audio/Audio.h"
#include "Camera/Camera.h"
#include "Cast/StaticCast.h"
#include "Engine/Alias/CoreAlias.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Command/CommandContext.h"
#include "Engine/Device/RenderDevice.h"
#include "Engine/Renderer/Draw.h"
#include "Engine/Renderer/DrawSprite.h"
#include "Engine/Resource/AssetModel.h"
#include "Engine/Resource/CreateBufferResource.h"
#include "Engine/Resource/ModelStorage.h"
#include "Engine/Resource/TextureStorage.h"
#include "ForDebug/DebugConsole.h"
#include "ForDebug/DebugLayer.h"
#include "ForDebug/ErrorSupression.h"
#include "ForDebug/Log.h"
#include "ForDebug/SetBreakOnSeverity.h"
#include "Geometry/Plane.h"
#include "Input/DirectInput.h"
#include "Input/PlayInput.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector4.h"
#include "String/ConvertString.h"
#include "System/DeltaTime.h"
#include "System/Hash64.h"
#include <array>
#include <cassert>
#include <chrono>
#include <cfloat>
#include <cstdint>
#include <filesystem>
#ifdef USE_IMGUI
#include "ForDebug/ImGuiJapaneseFont.h"
#endif

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
#include "System/Editor/EditorImportedAsset.h"
#endif

#ifdef DEVELOPMENT
#include <format>
#include <fstream>
#endif
#include <memory>
#include <numbers>
#include <SDL.h>
#include <SDL_syswm.h>
#include <sstream>
#include <string>
#include <strsafe.h>
#include <thread>
#include <vector>
#include <Windows.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <DirectXTex/d3dx12.h>
#pragma comment(lib, "DirectXTex.lib")

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#include <d3d12sdklayers.h>

#endif

namespace {

#ifdef DEVELOPMENT

// #region agent log
void AgentDebugLog(const char *location, const char *message,
                   const char *hypothesisId, const std::string &dataJson) {
  const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now().time_since_epoch())
                             .count();
  std::ofstream log(
      R"(./.cursor/debug-db6cac.log)",
      std::ios::app);
  if (!log) {
    return;
  }
  log << std::format(
             R"({{"sessionId":"db6cac","runId":"pre-fix","hypothesisId":"{}","location":"{}","message":"{}","data":{},"timestamp":{}}})",
             hypothesisId, location, message, dataJson, timestamp)
      << "\n";
}
// #endregion

#endif

} // namespace

namespace Atrum {

std::unique_ptr<AtrumEngine> AtrumEngine::instance_ = nullptr;

AtrumEngine *EngineInstance() { return AtrumEngine::Instance(); }

FrameDeltaTime *EngineFrameDeltaTime() {
	return AtrumEngine::Instance()->GetFrameDeltaTime();
}

Input::PlayInput *EnginePlayInput() {
	return AtrumEngine::Instance()->GetPlayInput();
}

Audio::AudioManager *EngineAudioManager() {
	return AtrumEngine::Instance()->GetAudioManager();
}

TextureStorage *EngineTextureStorage() {
	return AtrumEngine::Instance()->GetTextureStorage();
}

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

static LONG WINAPI ExportDump(EXCEPTION_POINTERS *exception) {

  // Dumpsフォルダを作成
  CreateDirectory(L"./Dumps", nullptr);

  // 現在時刻を名前に入れたファイルをDumpsフォルダ以下に作成
  SYSTEMTIME time;
  GetLocalTime(&time);
  wchar_t filePath[MAX_PATH] = {0};
  StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d_%02d-%02d_%02d-%02d.dmp",
                   time.wYear, time.wMonth, time.wDay, time.wHour,
                   time.wMinute);
  HANDLE dumpFileHandle =
      CreateFile(filePath, GENERIC_READ | GENERIC_WRITE,
                 FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

  // processId(このexeのId)とクラッシュ(例外)の発生した、threadIdを取得
  DWORD processId = GetCurrentProcessId();
  DWORD threadId = GetCurrentThreadId();

  // 設定情報を入力
  MINIDUMP_EXCEPTION_INFORMATION miniDumpInformation{0};
  miniDumpInformation.ThreadId = threadId;
  miniDumpInformation.ExceptionPointers = exception;
  miniDumpInformation.ClientPointers = TRUE;

  // Dumpを出力(MiniDumpNormalフラグで最低限の情報を出力させるようにする)
  MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle,
                    MiniDumpNormal, &miniDumpInformation, nullptr, nullptr);

  /* 他に関連付けられているSEH例外ハンドラがあれば記述を追加する */

  return EXCEPTION_EXECUTE_HANDLER;
}

void AtrumEngine::CreateDirectionalLightBuffer() {

  assert(!isInitialized_ &&
         "CreateDirectionalLightResource() is initializeHelper");

  directionalLightBuffer_ =
      std::make_unique<SingleConstantBuffer<DirectionalLightData>>();

  directionalLightBuffer_->CreateBuffer(renderDevice_->GetDevice());

  // デフォルト値
  DirectionalLightData directionalLightData{};

  directionalLightData.color = {1.0f, 1.0f, 1.0f};
  directionalLightData.direction = {0.0f, -1.0f, 0.0f};
  directionalLightData.intensity = 1.0f;

  directionalLightData_ = directionalLightData;
  directionalLightBuffer_->SetData(directionalLightData_);
}

void AtrumEngine::SetFps(const int32_t &fps) {

  secondsPerFrame_ = 1.0f / static_cast<float>(fps);
}

void AtrumEngine::InitImGui(
    [[maybe_unused]] D3D12_RENDER_TARGET_VIEW_DESC rtvDesc) {

#ifdef USE_IMGUI

  // ImGuiの初期化

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForD3D(window_->GetWindow());

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  Atrum::Debug::SetupImGuiJapaneseFont(io);
  io.Fonts->Build();

  DescriptorAllocator::DescriptorHandle imguiSrvHandle{};
  imguiSrvHandle = srvAllocator_->Allocate();

  ImGui_ImplDX12_Init(renderDevice_->GetDevice(), SwapChain::kBackBufferCount,
                      rtvDesc.Format, srvAllocator_->GetDescriptorHeap(),
                      imguiSrvHandle.cpu, imguiSrvHandle.gpu);

#endif
}

void AtrumEngine::EnableAdditionalException() {

#ifdef DEVELOPMENT

  // 浮動小数点例外を有効にする
  unsigned int currentControl;

  // 0除算 (_EM_ZERODIVIDE) と 無効な操作（NaN発生など）(_EM_INVALID) を有効化

  _controlfp_s(&currentControl, 0u, _MCW_EM);

  _controlfp_s(&currentControl,
               static_cast<unsigned int>(~(_EM_ZERODIVIDE | _EM_INVALID)),
               _MCW_EM);

#endif
}

void AtrumEngine::InitAssetStorages() {

  /* アセットストレージの初期化 */

  textureStorage_ = std::make_unique<TextureStorage>(TextureStorage::Token{});
  textureStorage_->Initialize(
      commandContextDirect_.get(), fenceManager_.get(), swapChainManager_.get(),
      srvAllocator_.get(), renderDevice_->GetDevice(), &temporaryResources_);

  const uint32_t modelDefaultTexture =
      textureStorage_->GetTexture("./Assets/Images/white4x4.png");

  modelStorage_ = std::make_unique<ModelStorage>(ModelStorage::Token{});
  modelStorage_->Initialize(commandContextDirect_.get(),
                            renderDevice_->GetDevice(), &temporaryResources_,
                            modelDefaultTexture, textureStorage_.get());
}

void AtrumEngine::InitDepthStencil() {

  // 深度ステンシルリソースの生成
  depthStencilResource_ = this->CreateDepthStencilResource(
      window_->GetClientWidth(), window_->GetClientHeight());

  // 深度ステンシルディスクリプタの生成
  dsvAllocator_ = std::make_unique<DescriptorAllocator>();
  dsvAllocator_->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false,
                            L"dsvDescriptor", renderDevice_->GetDevice());

  // DSVの設定
  D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
  // Format 基本Resourceに合わせる
  dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  // 2dTexture
  dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
  // DSVHeapの先頭にDSVを作る
  renderDevice_->GetDevice()->CreateDepthStencilView(
      depthStencilResource_.Get(), &dsvDesc, dsvAllocator_->GetCpuStart());
}

void AtrumEngine::InitRenderer() {

  /* 描画クラスの初期化 */

  draw_ = std::make_unique<Draw>(Draw::Token{});
  draw_->Initialize(renderDevice_->GetDevice(), commandContextDirect_.get(),
                    srvAllocator_.get());

  {
    std::array<ID3D12PipelineState *,
                static_cast<size_t>(BlendMode::MODE_COUNT)>
        object3dPso{};
    std::array<ID3D12PipelineState *,
                static_cast<size_t>(BlendMode::MODE_COUNT)>
        particlePso{};
    for (uint32_t i = 0; i < static_cast<uint32_t>(BlendMode::MODE_COUNT);
         ++i) {
      object3dPso[i] = graphicsPSObjects[i]->GetPSO();
      particlePso[i] = graphicsParticlePSObjects[i]->GetPSO();
    }
    draw_->RegisterObject3dAndParticlePipelines(object3dPso, particlePso);
    draw_->SetActiveBlendMode(blendMode_);
  }

  drawSprite_ = std::make_unique<DrawSprite>(DrawSprite::Token{});
  drawSprite_->Initialize(renderDevice_->GetDevice(),
                          commandContextDirect_.get(), srvAllocator_.get(),
                          textureStorage_.get());
}

void AtrumEngine::InitPlayInput() {

  /* 入力デバイスの初期化 */

  SDL_SysWMinfo wmInfo{};
  SDL_VERSION(&wmInfo.version);

  if (!SDL_GetWindowWMInfo(window_->GetWindow(), &wmInfo)) {
    assert(false);
  }

  // DirectInput
  directInput_ = std::make_unique<I::DirectInput>();
  directInput_->Initialize(window_.get());

  playInput_ = std::make_unique<I::PlayInput>();
}

void AtrumEngine::Initialize(const std::string &windowLabel,
                             const int32_t &clientWidth,
                             const int32_t &clientHeight) {

  assert(!isInitialized_ && "AtrumEngine is already initialized");

  // SEH例外が補足されなかった場合(Unhandled)に補足する関数を登録
  SetUnhandledExceptionFilter(ExportDump);

  D::OpenDebugConsole();

  D::EnableDebugLayer();

  // COMの初期化
  [[maybe_unused]] HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

  // COMの初期化が失敗したら起動不可
  assert(SUCCEEDED(hr));

  // ログ出力ファイルの初期化
  D::LogFile::Instance()->Initialize();

  window_ = std::make_unique<Window>();
  window_->Initialize(windowLabel, clientWidth, clientHeight);

  // レンダリングデバイスを生成
  renderDevice_ = std::make_unique<RenderDevice>();
  // レンダリングデバイスを初期化
  renderDevice_->Initialize();

  D::ErrorSuppressionDebug(renderDevice_->GetDevice());

  D::SetBreakOnSeverity(renderDevice_->GetDevice());

  // 描画コマンド経路を生成
  commandContextDirect_ = std::make_unique<CommandContext>();
  // 描画コマンド経路を初期化
  commandContextDirect_->Initialize(renderDevice_->GetDevice(),
                                    SwapChain::kBackBufferCount,
                                    D3D12_COMMAND_LIST_TYPE_DIRECT);

  rtvAllocator_ = std::make_unique<DescriptorAllocator>();

  swapChainManager_ = std::make_unique<SwapChain>();

  // RTVの設定
  D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
  rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
  rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

  swapChainManager_->Initialize(
      clientWidth, clientHeight, renderDevice_->GetDevice(),
      renderDevice_->GetDxgiFactory(), commandContextDirect_->GetCommandQueue(),
      window_->GetHWnd(), rtvAllocator_.get(), rtvDesc);

  // SRVディスクリプタヒープの生成
  srvAllocator_ = std::make_unique<DescriptorAllocator>();
  srvAllocator_->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true,
                            L"srvDescriptors", renderDevice_->GetDevice());

  // fenceの管理インスタンスを生成
  fenceManager_ = std::make_unique<Fence>();
  fenceManager_->Initialize(renderDevice_->GetDevice(),
                            SwapChain::kBackBufferCount);

  // ルートシグネチャの作成
  rootSignature_ = std::make_unique<RootSignature>();
  rootSignature_->Initialize(renderDevice_->GetDevice());

  // シェーダーコンパイラの初期化
  shaderCompiler_ = std::make_unique<ShaderCompiler>();
  shaderCompiler_->Initialize();

  // 必要なシェーダーのコンパイル
  shaderCompiler_->CompileShaders();

  // PSOの生成

  std::vector<IDxcBlob *> psBlobs{};

  psBlobs.resize(static_cast<uint32_t>(BlendMode::MODE_COUNT));

  for (auto &psBlob : psBlobs) {

    psBlob = shaderCompiler_->GetPixelShaderBlob();
  }

  psBlobs[static_cast<uint32_t>(BlendMode::BINARY_ALPHA)] =
      shaderCompiler_->GetPsBinaryAlphaBlob();
  psBlobs[static_cast<uint32_t>(BlendMode::DITHERED_ALPHA)] =
      shaderCompiler_->GetPsDitheredAlphaBlob();

  for (uint32_t i = 0; i < static_cast<uint32_t>(BlendMode::MODE_COUNT); ++i) {
    graphicsPSObjects[i] = std::make_unique<PipelineState>();

    graphicsPSObjects[i]->Initialize(rootSignature_->GetRootSignature(),
                                     renderDevice_->GetDevice(),
                                     shaderCompiler_->GetVertexShaderBlob(),
                                     psBlobs[i], static_cast<BlendMode>(i));

    graphicsParticlePSObjects[i] = std::make_unique<PipelineState>();

    graphicsParticlePSObjects[i]->Initialize(
        rootSignature_->GetRootSignature(), renderDevice_->GetDevice(),
        shaderCompiler_->GetParticleVertexShaderBlob(), psBlobs[i],
        static_cast<BlendMode>(i));
  }

  this->InitAssetStorages();

  this->InitRenderer();

  /**/

  // 平行光源Bufferの生成
  this->CreateDirectionalLightBuffer();

  this->InitDepthStencil();

  this->InitImGui(rtvDesc);

  this->EnableAdditionalException();

  this->InitPlayInput();

  /* 時間差分マネージャーの生成 */

  frameDeltaTime_ = std::make_unique<FrameDeltaTime>();
  frameDeltaTime_->Initialize();
  deltaTime_ = std::make_unique<DeltaTime>();

  audioManager_ = std::make_unique<Audio::AudioManager>(Audio::AudioManager::Token{});
  audioManager_->Initialize();

  /* 初期化完了のログ出力 */

  D::LogFile::Instance()->Log("Hello World!");

  isInitialized_ = true;
}

bool AtrumEngine::Process() {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  playInput_->EndOfFrame();

  SDL_Event event;

  while (SDL_PollEvent(&event)) {

#ifdef USE_IMGUI
    ImGui_ImplSDL2_ProcessEvent(&event);
#endif

    switch (event.type) {

    case SDL_WINDOWEVENT:

      if (event.window.event == SDL_WINDOWEVENT_CLOSE) {

        return false;
      }

      break;

    case SDL_QUIT:

      return false;

    case SDL_KEYDOWN:

      assert(event.key.keysym.scancode < 256);

      playInput_->SetKey(static_cast<uint8_t>(event.key.keysym.scancode), true);

      break;

    case SDL_KEYUP:

      assert(event.key.keysym.scancode < 256);

      playInput_->SetKey(static_cast<uint8_t>(event.key.keysym.scancode),
                         false);

      break;

    case SDL_MOUSEBUTTONDOWN:

      playInput_->SetMouseButton(event.button.button - 1, true);

      break;

    case SDL_MOUSEBUTTONUP:

      playInput_->SetMouseButton(event.button.button - 1, false);

      break;

    case SDL_MOUSEWHEEL:

      playInput_->AddMouseWheel(event.wheel.y);

      break;

    case SDL_MOUSEMOTION:

      playInput_->AddCursorDelta(event.motion.xrel, event.motion.yrel);

      playInput_->SetCursorPos(event.motion.x, event.motion.y);

      break;

#if defined(DEVELOPMENT) && defined(USE_IMGUI)
    case SDL_DROPFILE:
      if (event.drop.file != nullptr) {
        Atrum::Editor::Import::FileDropQueue::Instance().Push(event.drop.file);
      }
      SDL_free(event.drop.file);
      break;
#endif
    }
  }

  deltaTime_->CalcDeltaTime();

  if (deltaTime_->GetDeltaTime() < secondsPerFrame_) {

    float sleepSec = secondsPerFrame_ - deltaTime_->GetDeltaTime();

    auto sleepMicroSec = static_cast<long long>(sleepSec * 1000000.0f);

    if (sleepMicroSec > 1000) {

      std::this_thread::sleep_for(std::chrono::microseconds(sleepMicroSec));
    }
  }

  directInput_->Update();

  audioManager_->Update();

  frameDeltaTime_->CalcDeltaTime();

  return true;
}

#ifdef USE_IMGUI

void AtrumEngine::ImGuiNewFrame() const {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  ImGui_ImplDX12_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

void AtrumEngine::ImGuiRender() const {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  ImGui::Render();
}

#endif

void AtrumEngine::PreDraw() {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  swapChainManager_->UpdateBackBufferIndex();

#ifdef DEVELOPMENT

  // #region agent log
  {
    static uint32_t preDrawLogCount = 0;
    if (preDrawLogCount < 30) {
      AgentDebugLog(
          "AtrumEngine.cpp:PreDraw",
          "frame start back buffer",
          "C",
          std::format(R"({{"backBufferIndex":{}}})",
                      swapChainManager_->GetBackBufferIndex()));
      ++preDrawLogCount;
    }
  }
  // #endregion

#endif

  D3D12_RESOURCE_BARRIER barrier{};

  // バリアの種類
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

  // バリアフラグ
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

  // サブリソース
  barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

  // バリアを張る対象のリソース(現在のバックバッファに対して行なう)
  barrier.Transition.pResource =
      swapChainManager_->GetSwapChainResourceCurrent();

  // 遷移前(現在)のResourceState
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

  // 遷移後のResourceState
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

  // TransitionBarrierを張る
  commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

  // 描画先のRTVとDSVを設定
  D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvAllocator_->GetCpuStart();
  commandContextDirect_->GetCommandList()->OMSetRenderTargets(
      1, swapChainManager_->PGetRtvHandleCurrent(), false, &dsvHandle);

  // 指定した深度(1.0f)で画面全体をクリアする
  commandContextDirect_->GetCommandList()->ClearDepthStencilView(
      dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

  // 指定色で画面全体をクリアする
  float clearColor[] = {0.0625f, 0.1875f, 0.125f, 1.0f};
  commandContextDirect_->GetCommandList()->ClearRenderTargetView(
      swapChainManager_->GetRtvHandleCurrent(), clearColor, 0, nullptr);

  // Viewportを設定
  if (subViewportOverride_.has_value() && subScissorOverride_.has_value()) {
    commandContextDirect_->GetCommandList()->RSSetViewports(
        1, &subViewportOverride_.value());
    commandContextDirect_->GetCommandList()->RSSetScissorRects(
        1, &subScissorOverride_.value());
  } else {
    commandContextDirect_->GetCommandList()->RSSetViewports(
        1, &window_->GetViewport());
    commandContextDirect_->GetCommandList()->RSSetScissorRects(
        1, &window_->GetScissorRect());
  }

  RebindObject3dDrawBindings();
}

void AtrumEngine::RebindObject3dDrawBindings() {
  if (!isInitialized_) {
    return;
  }

  ID3D12DescriptorHeap *descriptorHeaps[] = {
      srvAllocator_->GetDescriptorHeap()};
  commandContextDirect_->GetCommandList()->SetDescriptorHeaps(1,
                                                              descriptorHeaps);

  commandContextDirect_->GetCommandList()->SetGraphicsRootSignature(
      rootSignature_->GetRootSignature());

  commandContextDirect_->GetCommandList()->SetPipelineState(
      graphicsPSObjects[static_cast<uint32_t>(blendMode_)]->GetPSO());
  draw_->SetActiveBlendMode(blendMode_);

  commandContextDirect_->GetCommandList()->IASetVertexBuffers(
      0, 1, draw_->vertexBuffer_->PGetVertexBufferView());
  commandContextDirect_->GetCommandList()->IASetIndexBuffer(
      draw_->indexBuffer_->PGetBufferView());
  commandContextDirect_->GetCommandList()->IASetPrimitiveTopology(
      D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
      0, draw_->materialBuffer_->GetGpuVirtualAddress());
  commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
      1, draw_->transformationBuffer_->GetGpuVirtualAddress());

  FlushDirectionalLightConstantBuffer();
  commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
      3, directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::SetSubViewport(const int x, const int y, const int width,
                                 const int height) {
  if (width <= 0 || height <= 0) {
    ClearSubViewport();
    return;
  }
  D3D12_VIEWPORT viewport{};
  viewport.TopLeftX = static_cast<float>(x);
  viewport.TopLeftY = static_cast<float>(y);
  viewport.Width = static_cast<float>(width);
  viewport.Height = static_cast<float>(height);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  subViewportOverride_ = viewport;

  D3D12_RECT scissor{};
  scissor.left = x;
  scissor.top = y;
  scissor.right = x + width;
  scissor.bottom = y + height;
  subScissorOverride_ = scissor;
}

void AtrumEngine::ClearSubViewport() {
  subViewportOverride_.reset();
  subScissorOverride_.reset();
}

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

ID3D12Device *AtrumEngine::GetD3DDevice() const { return renderDevice_->GetDevice(); }

ID3D12GraphicsCommandList *AtrumEngine::GetD3DCommandList() const {
  return commandContextDirect_->GetCommandList();
}

DescriptorAllocator *AtrumEngine::GetSrvAllocator() { return srvAllocator_.get(); }

D3D12_CPU_DESCRIPTOR_HANDLE AtrumEngine::GetSwapChainRtvCpu() const {
  return swapChainManager_->GetRtvHandleCurrent();
}

D3D12_CPU_DESCRIPTOR_HANDLE AtrumEngine::GetMainDepthStencilViewCpu() const {
  return dsvAllocator_->GetCpuStart();
}

void AtrumEngine::EditorRenderSceneViewTarget(
    const int width, const int height, ID3D12Resource *colorResource,
    const D3D12_CPU_DESCRIPTOR_HANDLE colorRtv, ID3D12Resource *depthResource,
    const D3D12_CPU_DESCRIPTOR_HANDLE depthDsv,
    const bool colorReadableByShader,
    const std::function<void()> &drawSceneContent) {
  if (width <= 0 || height <= 0 || colorResource == nullptr ||
      depthResource == nullptr || !drawSceneContent) {
    return;
  }

  ID3D12GraphicsCommandList *commandList =
      commandContextDirect_->GetCommandList();

  if (colorReadableByShader) {
    D3D12_RESOURCE_BARRIER toRender{};
    toRender.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toRender.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    toRender.Transition.pResource = colorResource;
    toRender.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    toRender.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    toRender.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    commandList->ResourceBarrier(1, &toRender);
  }

  commandList->OMSetRenderTargets(1, &colorRtv, false, &depthDsv);

  const D3D12_VIEWPORT viewport{
      0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height),
      0.0f, 1.0f};
  const D3D12_RECT scissor{0, 0, width, height};
  commandList->RSSetViewports(1, &viewport);
  commandList->RSSetScissorRects(1, &scissor);

  const float clearColor[] = {0.0625f, 0.1875f, 0.125f, 1.0f};
  commandList->ClearRenderTargetView(colorRtv, clearColor, 0, nullptr);
  commandList->ClearDepthStencilView(
      depthDsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

  ClearSubViewport();
  RebindObject3dDrawBindings();
  drawSceneContent();

  D3D12_RESOURCE_BARRIER toSrv{};
  toSrv.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  toSrv.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  toSrv.Transition.pResource = colorResource;
  toSrv.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
  toSrv.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
  toSrv.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
  commandList->ResourceBarrier(1, &toSrv);

  const D3D12_CPU_DESCRIPTOR_HANDLE swapRtv = GetSwapChainRtvCpu();
  const D3D12_CPU_DESCRIPTOR_HANDLE mainDsv = GetMainDepthStencilViewCpu();
  commandList->OMSetRenderTargets(1, &swapRtv, false, &mainDsv);

  ID3D12DescriptorHeap *descriptorHeaps[] = {srvAllocator_->GetDescriptorHeap()};
  commandList->SetDescriptorHeaps(1, descriptorHeaps);

  commandList->RSSetViewports(1, &window_->GetViewport());
  commandList->RSSetScissorRects(1, &window_->GetScissorRect());
}

#endif

void AtrumEngine::ApplySubViewportToCommandList() const {
  if (!isInitialized_) {
    return;
  }
  if (!subViewportOverride_.has_value() || !subScissorOverride_.has_value()) {
    return;
  }
  commandContextDirect_->GetCommandList()->RSSetViewports(
      1, &subViewportOverride_.value());
  commandContextDirect_->GetCommandList()->RSSetScissorRects(
      1, &subScissorOverride_.value());
}

void AtrumEngine::PostDraw() {

  assert(isInitialized_ && "AtrumEngine is not initialized");

#ifdef USE_IMGUI

  // ImGui::Render() 後のみ有効（未呼び出し時は GetDrawData() が null になり得る）
  if (ImDrawData *drawData = ImGui::GetDrawData();
      drawData != nullptr && drawData->Valid) {
    ImGui_ImplDX12_RenderDrawData(drawData,
                                  commandContextDirect_->GetCommandList());
  }

#endif

  D3D12_RESOURCE_BARRIER barrier{};

  // バリアの種類
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

  // バリアフラグ
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

  // サブリソース
  barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

  // バリアを張る対象のリソース(現在のバックバッファに対して行なう)
  barrier.Transition.pResource =
      swapChainManager_->GetSwapChainResourceCurrent();

  // 画面に描く処理が終了し画面に映すため状態を遷移
  // RenderTargetからPresentにする
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

  // TransitionBarrierを張る
  commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

  // コマンドリストの内容を確定させる
  [[maybe_unused]] HRESULT hr =
      commandContextDirect_->GetCommandList()->Close();
#if defined(DEVELOPMENT)
  assert(SUCCEEDED(hr) &&
         "コマンドリスト Close 失敗。D3D12 デバッグレイヤー出力を確認してください。");
#else
  assert(SUCCEEDED(hr));
#endif

  // GPUにコマンドリストを実行させる
  ID3D12CommandList *commandLists[] = {commandContextDirect_->GetCommandList()};
  commandContextDirect_->GetCommandQueue()->ExecuteCommandLists(1,
                                                                commandLists);

  fenceManager_->Signal(commandContextDirect_->GetCommandQueue(),
                        swapChainManager_->GetBackBufferIndex());

  // 次のフレーム番号を取得する
  swapChainManager_->UpdateBackBufferIndex();

  fenceManager_->WaitForNextBuffer(swapChainManager_->GetBackBufferIndex());

  if (!temporaryResources_.empty()) {

    // フレーム内で作成した中間リソースを全て解放
    temporaryResources_.clear();
  }

  // GPUとOSに画面の交換を行なうよう通知する
  swapChainManager_->GetSwapChain()->Present(1, 0);

  // 次のフレーム用のコマンドリストを準備

  hr = commandContextDirect_
           ->GetCommandAllocator(swapChainManager_->GetBackBufferIndex())
           ->Reset();
  assert(SUCCEEDED(hr));
  hr = commandContextDirect_->GetCommandList()->Reset(
      commandContextDirect_->GetCommandAllocator(
          swapChainManager_->GetBackBufferIndex()),
      nullptr);
  assert(SUCCEEDED(hr));

#ifdef DEVELOPMENT

  // #region agent log
  {
    static uint32_t postDrawLogCount = 0;
    if (postDrawLogCount < 60) {
      HRESULT deviceRemovedReason = S_OK;
      if (renderDevice_ && renderDevice_->GetDevice()) {
        deviceRemovedReason =
            renderDevice_->GetDevice()->GetDeviceRemovedReason();
      }
      const uint32_t drawVertexCount = draw_->vertexBuffer_->GetDrewCount();
      const uint32_t drawIndexCount = draw_->indexBuffer_->GetDrewCount();
      const bool drawVertexOverflow =
          drawVertexCount > Draw::kVertexMaxDrawCount;
      const bool drawCbOverflow =
          draw_->constantBufferCount_ >= Draw::kMaxDrawCount;
      const bool spriteCbOverflow =
          drawSprite_->constantBufferCount_ >= DrawSprite::kMaxDrawCount;
      AgentDebugLog(
          "AtrumEngine.cpp:PostDraw",
          "frame draw usage",
          "A,B,D",
          std::format(
              R"({{"backBufferIndex":{},"drawConstantBufferCount":{},"drawVertexCount":{},"drawIndexCount":{},"drawVertexMax":{},"drawCbMax":{},"spriteConstantBufferCount":{},"drawVertexOverflow":{},"drawCbOverflow":{},"spriteCbOverflow":{},"deviceRemovedHr":{}}})",
              swapChainManager_->GetBackBufferIndex(),
              draw_->constantBufferCount_, drawVertexCount, drawIndexCount,
              Draw::kVertexMaxDrawCount, Draw::kMaxDrawCount,
              drawSprite_->constantBufferCount_, drawVertexOverflow,
              drawCbOverflow, spriteCbOverflow,
              static_cast<long>(deviceRemovedReason)));
      ++postDrawLogCount;
    }
  }
  // #endregion

#endif

  // 描画頂点数のカウントをリセット
  drawSprite_->vertexBuffer_->ResetDrewCount();
  draw_->vertexBuffer_->ResetDrewCount();

  // 頂点インデックス数のカウントをリセット
  drawSprite_->indexBuffer_->ResetDrewCount();
  draw_->indexBuffer_->ResetDrewCount();

  // 定数バッファのカウントをリセット
  drawSprite_->constantBufferCount_ = 0;
  draw_->constantBufferCount_ = 0;
}

void AtrumEngine::Finalize() {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  // GPUの完了を待つ
  fenceManager_->ForceSyncGPU(commandContextDirect_->GetCommandQueue());

  /* 解放処理 */

  textureStorage_.reset();
  modelStorage_.reset();
  draw_.reset();
  drawSprite_.reset();
  audioManager_.reset();
  frameDeltaTime_.reset();
  playInput_.reset();
  directInput_.reset();

  temporaryResources_.clear();

#ifdef USE_IMGUI

  ImGui_ImplDX12_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

#endif

#ifdef DEVELOPMENT

  /* NVIDIAグラフィクスドライバの浮動小数点例外(修正難)への応急処置 */

  // 浮動小数点例外を無効にする
  unsigned int currentControl;

  // 0除算 (_EM_ZERODIVIDE) と 無効な操作（NaN発生など）(_EM_INVALID)
  // の例外を無効化

  _controlfp_s(&currentControl, _MCW_EM, _MCW_EM);

#endif
}

ComPtr<ID3D12Resource> AtrumEngine::CreateDepthStencilResource(int32_t width,
                                                               int32_t height) {

  assert(!isInitialized_ && "CreateDepthStencilResource() is initializeHelper");

  D3D12_RESOURCE_DESC resourceDesc{};
  // Textureの幅
  resourceDesc.Width = width;
  // Textureの高さ
  resourceDesc.Height = height;
  // MipMapの数
  resourceDesc.MipLevels = 1;
  // 奥行き or 配列Textureの要素数
  resourceDesc.DepthOrArraySize = 1;
  // DepthStencilとして利用可能なフォーマット
  resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  // サンプリングカウント 1固定
  resourceDesc.SampleDesc.Count = 1;
  // 2次元
  resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
  // DepthStencilとして使う通知
  resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

  // 利用するHeapの設定
  D3D12_HEAP_PROPERTIES heapProperties{};
  // VRAM上に作る
  heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

  // 深度値のクリア設定
  D3D12_CLEAR_VALUE depthClearValue{};
  // 1.0f(最大値)でクリア
  depthClearValue.DepthStencil.Depth = 1.0f;
  // フォーマット リソースと合わせる
  depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

  // Resourceの生成
  ComPtr<ID3D12Resource> resource = nullptr;
  [[maybe_unused]] HRESULT hr =
      renderDevice_->GetDevice()->CreateCommittedResource(
          // heapの設定
          &heapProperties,
          // Heapの特殊な設定 無し
          D3D12_HEAP_FLAG_NONE,
          // Resourceの設定
          &resourceDesc,
          // 深度値を書き込む状態にしておく
          D3D12_RESOURCE_STATE_DEPTH_WRITE,
          // Clear最適値
          &depthClearValue,
          // 作成するResourceポインタへのポインタ
          IID_PPV_ARGS(&resource));

  assert(SUCCEEDED(hr));

  return resource;
}

void AtrumEngine::DrawTriangle(const uint32_t &textureIndex,
                               const M::Vector4 &textureColor,
                               const M::Transform &uvTransform,
                               const M::Matrix4x4 &worldMatrix,
                               const std::array<VertexData, 3> &vertexData,
                               const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawTriangle(textureIndex, textureColor, uvTransform, worldMatrix,
                       vertexData, isLighting,
                       directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawSphere(const uint32_t& textureIndex,
    const Math::Vector4& textureColor,
    const Math::Transform& uvTransform,
    const Math::Matrix4x4& worldMatrix, const float radius,
    const uint32_t subdivision, const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawSphere(textureIndex, textureColor, uvTransform, worldMatrix,
                     radius, subdivision, isLighting,
                     directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawBillboard(const uint32_t &textureIndex,
                                const M::Vector4 &textureColor,
                                const M::Transform &uvTransform,
                                const M::Matrix4x4 &worldMatrix,
                                const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawBillboard(textureIndex, textureColor, uvTransform, worldMatrix,
                        isLighting,
                        directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawSphereParticlesInstanced(
    const uint32_t &textureIndex, const M::Transform &uvTransform,
    const std::vector<ParticleInstanceGpuData> &instances,
    uint32_t subdivision, const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawSphereParticlesInstanced(
      textureIndex, uvTransform, instances, subdivision, isLighting,
      directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawBillboardParticlesInstanced(
    const uint32_t &textureIndex, const M::Transform &uvTransform,
    const std::vector<ParticleInstanceGpuData> &instances,
    const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawBillboardParticlesInstanced(
      textureIndex, uvTransform, instances, isLighting,
      directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawModelParticlesInstanced(
    AssetModel *model, const std::vector<ParticleInstanceGpuData> &instances,
    const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawModelParticlesInstanced(
      model, instances, isLighting, directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawLine3D(const uint32_t &textureIndex,
                             const M::Vector4 &textureColor,
                             const M::Transform &uvTransform,
                             const M::Vector3 &worldStart,
                             const M::Vector3 &worldEnd, const float width,
                             const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawLine3D(textureIndex, textureColor, uvTransform, worldStart,
                     worldEnd, width, isLighting,
                     directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawLine3DOnFloor(
    const uint32_t &textureIndex, const M::Vector4 &textureColor,
    const M::Transform &uvTransform, const M::Vector3 &worldStart,
    const M::Vector3 &worldEnd, const float width, const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawLine3DOnFloor(textureIndex, textureColor, uvTransform, worldStart,
                            worldEnd, width, isLighting,
                            directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawLine3DOnFloorPlane(
    const uint32_t &textureIndex, const M::Vector4 &textureColor,
    const M::Transform &uvTransform, const M::Vector3 &worldStart,
    const M::Vector3 &worldEnd, const float lineWidthPixels,
    const int viewportWidth, const int viewportHeight, const float floorY,
    const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawLine3DOnFloorPlane(
      textureIndex, textureColor, uvTransform, worldStart, worldEnd,
      lineWidthPixels, viewportWidth, viewportHeight, floorY, isLighting,
      directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawDashedLine3D(const uint32_t &textureIndex,
                                   const M::Vector4 &textureColor,
                                   const M::Transform &uvTransform,
                                   const M::Vector3 &worldStart,
                                   const M::Vector3 &worldEnd, const float width,
                                   const float dashLength, const float gapLength,
                                   const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawDashedLine3D(textureIndex, textureColor, uvTransform, worldStart,
                           worldEnd, width, dashLength, gapLength, isLighting,
                           directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawAsymmetricPyramid(
    const uint32_t &textureIndex, const M::Vector4 &textureColor,
    const M::Transform &uvTransform, const M::Matrix4x4 &worldMatrix,
    const G::PyramidMesh &mesh, const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  draw_->DrawAsymmetricPyramid(textureIndex, textureColor, uvTransform,
                                worldMatrix, mesh, isLighting,
                                directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::PrepareSprite() {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  drawSprite_->PrepareSprite();
}

void AtrumEngine::DrawSpriteRect(const uint32_t &textureIndex,
                                 const M::Vector4 &textureColor,
                                 const M::Transform &uvTransform,
                                 const M::Transform &rectTransform,
                                 const M::Vector2 &spriteSize,
                                 const M::Vector2 &spriteLeftTop) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  drawSprite_->DrawSpriteRect(textureIndex, textureColor, uvTransform,
                               rectTransform, spriteSize, spriteLeftTop);
}

void AtrumEngine::DrawSpriteLine(const uint32_t &textureIndex,
                                 const M::Vector4 &textureColor,
                                 const M::Transform &uvTransform,
                                 const M::Vector2 &start, const M::Vector2 &end,
                                 const float &width, const float &posZ) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  drawSprite_->DrawSpriteLine(textureIndex, textureColor, uvTransform, start,
                               end, width, posZ);
}

void AtrumEngine::DrawSpriteCall(const uint32_t &textureIndex) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  drawSprite_->DrawSpriteCall(textureIndex);
}

void AtrumEngine::DrawModel(AssetModel *model,
                            const Math::Matrix4x4 &worldMatrix,
                            const bool isLighting, const Math::Vector4 &color) {
  draw_->DrawModel(model, worldMatrix, color, isLighting,
                    directionalLightBuffer_->GetGpuVirtualAddress());
}

uint32_t AtrumEngine::GetTexture(const std::string &filePath) {

  return textureStorage_->GetTexture(filePath);
}

std::shared_ptr<AssetModel> AtrumEngine::GetModel(
    const std::string &directoryPathObj, const std::string &objFileName,
    const std::string &directoryPathMtl, const std::string &mtlFileName) {

  return modelStorage_->GetModel(directoryPathObj, objFileName,
                                  directoryPathMtl, mtlFileName);
}

void AtrumEngine::SetDirectionalLightData(const DirectionalLightData &data) {
  directionalLightData_ = data;
  FlushDirectionalLightConstantBuffer();
}

void AtrumEngine::FlushDirectionalLightConstantBuffer() {
  if (draw_ != nullptr && draw_->camera_ != nullptr) {
    directionalLightData_.cameraWorldPosition = draw_->camera_->GetTranslate();
  }
  volumetricFog_.ApplyTo(directionalLightData_);
  godRays_.ApplyTo(directionalLightData_);
  directionalShadow_.ApplyTo(directionalLightData_);
  if (directionalLightBuffer_ != nullptr) {
    directionalLightBuffer_->SetData(directionalLightData_);
  }
}

void AtrumEngine::SetCamera(Camera *camera) {
  draw_->SetCamera(camera);
  drawSprite_->SetCamera(camera);
}

void AtrumEngine::SetBlendMode(const BlendMode &blendMode) {
  blendMode_ = blendMode;

  commandContextDirect_->GetCommandList()->SetPipelineState(
      graphicsPSObjects[static_cast<uint32_t>(blendMode_)]->GetPSO());
  draw_->SetActiveBlendMode(blendMode_);
}

void AtrumEngine::SetBlendModeForFrame(const BlendMode &blendMode) {
  // PSOを設定
  commandContextDirect_->GetCommandList()->SetPipelineState(
      graphicsPSObjects[static_cast<uint32_t>(blendMode)]->GetPSO());
  draw_->SetActiveBlendMode(blendMode);
}

CoUnInitializer::~CoUnInitializer() {

  OutputDebugStringA("\nleakCheck\n\n");

  Microsoft::WRL::ComPtr<IDXGIDebug1> debug;

  if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {

    debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
    debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
    debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
  }

  // COMの終了処理
  CoUninitialize();
}

} // namespace Atrum