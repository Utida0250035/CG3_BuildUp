#include "Engine/Alias/CoreAlias.h"

#include "Audio/Audio.h"
#include "Cast/StaticCast.h"
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
#include "Hash/Hash64.h"
#include "Input/DirectInput.h"
#include "Input/PlayInput.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector4.h"
#include "String/ConvertString.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <Windows.h>
#include <cassert>
#include <cfloat>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <numbers>
#include <sstream>
#include <string>
#include <strsafe.h>
#include <vector>

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

namespace Atrum {

AtrumEngine *AtrumEngine::instance_ = nullptr;

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

  directionalLightBuffer_->SetData(directionalLightData);
}

void AtrumEngine::SetFps(const int32_t &fps) {

  secondsPerFrame_ = 1.0f / static_cast<float>(fps);
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
  D::LogFile::GetInstance()->Initialize();

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

  for (uint32_t i = 0; i < static_cast<uint32_t>(BlendMode::MODE_COUNT); ++i) {
    graphicsPSObjects[i] = std::make_unique<PipelineState>();

    graphicsPSObjects[i]->Initialize(
        rootSignature_->GetRootSignature(), renderDevice_->GetDevice(),
        shaderCompiler_->GetVertexShaderBlob(),
        shaderCompiler_->GetPixelShaderBlob(), static_cast<BlendMode>(i));
  }

  /* 描画クラスの初期化 */

  pDraw_ = Draw::GetInstance();
  pDraw_->Initialize(renderDevice_->GetDevice(), commandContextDirect_.get(),
                     srvAllocator_.get());

  pDrawSprite_ = DrawSprite::GetInstance();
  pDrawSprite_->Initialize(renderDevice_->GetDevice(),
                           commandContextDirect_.get(), srvAllocator_.get(),
                           clientWidth, clientHeight);

  /**/

  // 平行光源Bufferの生成
  this->CreateDirectionalLightBuffer();

  // 深度ステンシルリソースの生成
  depthStencilResource_ =
      this->CreateDepthStencilResource(clientWidth, clientHeight);

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

#ifdef USE_IMGUI

  // ImGuiの初期化

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForD3D(window_->GetWindow());

  DescriptorAllocator::DescriptorHandle imguiSrvHandle{};

  imguiSrvHandle = srvAllocator_->Allocate();

  ImGui_ImplDX12_Init(renderDevice_->GetDevice(), SwapChain::kBackBufferCount,
                      rtvDesc.Format, srvAllocator_->GetDescriptorHeap(),
                      imguiSrvHandle.cpu, imguiSrvHandle.gpu);

  ImGuiIO &io = ImGui::GetIO();

  io.Fonts->Build();

#endif

#ifdef DEVELOPMENT

  // 浮動小数点例外を有効にする
  unsigned int currentControl;

  // 0除算 (_EM_ZERODIVIDE) と 無効な操作（NaN発生など）(_EM_INVALID) を有効化

  _controlfp_s(&currentControl, 0u, _MCW_EM);

  _controlfp_s(&currentControl,
               static_cast<unsigned int>(~(_EM_ZERODIVIDE | _EM_INVALID)),
               _MCW_EM);

#endif

  /* アセットストレージの初期化 */

  pTextureStorage_ = TextureStorage::GetInstance();
  pTextureStorage_->Initialize(
      commandContextDirect_.get(), fenceManager_.get(), swapChainManager_.get(),
      srvAllocator_.get(), renderDevice_->GetDevice(), &temporaryResources_);

  uint32_t modelDefaultTexture =
      pTextureStorage_->GetTexture("./Assets/Images/white4x4.png");

  pModelStorage_ = ModelStorage::GetInstance();
  pModelStorage_->Initialize(commandContextDirect_.get(),
                             renderDevice_->GetDevice(), &temporaryResources_,
                             modelDefaultTexture);

  /* 時間差分マネージャーの生成 */

  deltaTimeManager_.reset(new DeltaTime());

  /* 入力デバイスの初期化 */

  SDL_SysWMinfo wmInfo{};
  SDL_VERSION(&wmInfo.version);

  if (!SDL_GetWindowWMInfo(window_->GetWindow(), &wmInfo)) {

    assert(false);
  }

  // DirectInput
  directInput_ = I::DirectInput::GetInstance();
  directInput_->Initialize(window_->GetHInstance(), window_->GetHWnd());

  // SDL2入力
  playInput_ = I::PlayInput::GetInstance();

  /* 音源マネージャーの初期化 */

  audio_ = Audio::Manager::GetInstance();
  audio_->Initialize();

  /* 初期化完了のログ出力 */

  D::LogFile::GetInstance()->Log("Hello World!");

  isInitialized_ = true;
}

bool AtrumEngine::Process() const {

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
    }
  }

  directInput_->Update();

  audio_->Update();

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

  D3D12_RESOURCE_BARRIER barrier{};

  // バリアの種類
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

  // バリアフラグ
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

  // サブリソース
  barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

  // バリアを張る対象のリソース(現在のバックバッファに対して行なう)
  barrier.Transition.pResource =
      swapChainManager_->GetSwapChainResourceCurrent().Get();

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

  // 描画用のDescriptorHeapの設定
  ID3D12DescriptorHeap *descriptorHeaps[] = {
      srvAllocator_->GetDescriptorHeap()};
  commandContextDirect_->GetCommandList()->SetDescriptorHeaps(1,
                                                              descriptorHeaps);

  // Viewportを設定
  commandContextDirect_->GetCommandList()->RSSetViewports(
      1, &window_->GetViewport());

  // ScissorRectを設定
  commandContextDirect_->GetCommandList()->RSSetScissorRects(
      1, &window_->GetScissorRect());

  // RootSignatureを設定 PSOに設定しているが別途の設定が必要
  commandContextDirect_->GetCommandList()->SetGraphicsRootSignature(
      rootSignature_->GetRootSignature());

  // PSOを設定
  commandContextDirect_->GetCommandList()->SetPipelineState(
      graphicsPSObjects[static_cast<uint32_t>(blendMode_)]->GetPSO());

  // VBVを設定
  commandContextDirect_->GetCommandList()->IASetVertexBuffers(
      0, 1, pDraw_->vertexBuffer_->PGetVertexBufferView());

  // IBVを設定
  commandContextDirect_->GetCommandList()->IASetIndexBuffer(
      pDraw_->indexBuffer_->PGetBufferView());

  // 形状を設定 PSOに設定しているものとは別で同じものを設定すると考える
  commandContextDirect_->GetCommandList()->IASetPrimitiveTopology(
      D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // マテリアルCBufferの場所を設定
  commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
      0, pDraw_->materialBuffer_->GetGpuVirtualAddress());

  // Transformation用のCBufferの場所を設定
  commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
      1, pDraw_->transformationBuffer_->GetGpuVirtualAddress());

  // directionalLight用のCBufferの場所を設定
  commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
      3, directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::PostDraw() {

  assert(isInitialized_ && "AtrumEngine is not initialized");

#ifdef USE_IMGUI

  // ImGuiの描画コマンドを積む
  ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(),
                                commandContextDirect_->GetCommandList());

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
      swapChainManager_->GetSwapChainResourceCurrent().Get();

  // 画面に描く処理が終了し画面に映すため状態を遷移
  // RenderTargetからPresentにする
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

  // TransitionBarrierを張る
  commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

  // コマンドリストの内容を確定させる
  [[maybe_unused]] HRESULT hr =
      commandContextDirect_->GetCommandList()->Close();
  assert(SUCCEEDED(hr));

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

  // 描画頂点数のカウントをリセット
  pDrawSprite_->vertexBuffer_->ResetDrewCount();
  pDraw_->vertexBuffer_->ResetDrewCount();

  // 頂点インデックス数のカウントをリセット
  pDrawSprite_->indexBuffer_->ResetDrewCount();
  pDraw_->indexBuffer_->ResetDrewCount();

  // 定数バッファのカウントをリセット
  pDrawSprite_->constantBufferCount_ = 0;
  pDraw_->constantBufferCount_ = 0;
}

void AtrumEngine::Finalize() {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  // GPUの完了を待つ
  fenceManager_->ForceSyncGPU(commandContextDirect_->GetCommandQueue());

  /* 解放処理 */

  TextureStorage::Destroy();

  ModelStorage::Destroy();

  Draw::Destroy();

  DrawSprite::Destroy();

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

  // 0除算 (_EM_ZERODIVIDE) と 無効な操作（NaN発生など）(_EM_INVALID) の例外を無効化

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
                               const M::Transform &triangleTransform,
                               const std::array<VertexData, 3> &vertexData,
                               const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  pDraw_->DrawTriangle(textureIndex, textureColor, uvTransform,
                       triangleTransform, vertexData, isLighting,
                       directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawSphere(const uint32_t &textureIndex,
                             const M::Vector4 &textureColor,
                             const M::Transform &uvTransform,
                             const M::Transform &sphereTransform,
                             const float radius, const uint32_t subdivision,
                             const bool isLighting) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  pDraw_->DrawSphere(textureIndex, textureColor, uvTransform, sphereTransform,
                     radius, subdivision, isLighting,
                     directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::DrawAsymmetricPyramid(
    const uint32_t &textureIndex, const M::Vector4 &textureColor,
    const M::Transform &uvTransform, const M::Vector3 &scale,
    const M::Quaternion &rotate, const M::Vector3 &translate,
    const G::PyramidMesh &mesh, const bool isLighting) {

  pDraw_->DrawAsymmetricPyramid(
      textureIndex, textureColor, uvTransform, scale, rotate, translate, mesh,
      isLighting, directionalLightBuffer_->GetGpuVirtualAddress());
}

void AtrumEngine::PrepareSprite() {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  pDrawSprite_->PrepareSprite();
}

void AtrumEngine::DrawSpriteRect(const uint32_t &textureIndex,
                                 const M::Vector4 &textureColor,
                                 const M::Transform &uvTransform,
                                 const M::Transform &rectTransform,
                                 const M::Vector2 &rectSize) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  pDrawSprite_->DrawSpriteRect(textureIndex, textureColor, uvTransform,
                               rectTransform, rectSize);
}

void AtrumEngine::DrawSpriteLine(const uint32_t &textureIndex,
                                 const M::Vector4 &textureColor,
                                 const M::Transform &uvTransform,
                                 const M::Vector2 &start, const M::Vector2 &end,
                                 const float &width, const float &posZ) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  pDrawSprite_->DrawSpriteLine(textureIndex, textureColor, uvTransform, start,
                               end, width, posZ);
}

void AtrumEngine::DrawSpriteCall(const uint32_t &textureIndex) {

  assert(isInitialized_ && "AtrumEngine is not initialized");

  pDrawSprite_->DrawSpriteCall(textureIndex);
}

void AtrumEngine::DrawModel(AssetModel *model, const M::Transform &transform,
                            const bool isLighting) {

  pDraw_->DrawModel(model, transform, isLighting,
                    directionalLightBuffer_->GetGpuVirtualAddress());
}

uint32_t AtrumEngine::GetTexture(const std::string &filePath) {

  return pTextureStorage_->GetTexture(filePath);
}

std::shared_ptr<AssetModel> AtrumEngine::GetModel(
    const std::string &directoryPathObj, const std::string &objFileName,
    const std::string &directoryPathMtl, const std::string &mtlFileName) {

  return pModelStorage_->GetModel(directoryPathObj, objFileName,
                                  directoryPathMtl, mtlFileName);
}

void AtrumEngine::SetViewMatrix(const M::Matrix4x4 &mat) {
  pDraw_->viewMatrix_ = mat;
}

void AtrumEngine::SetBlendMode(const BlendMode &blendMode) {
  blendMode_ = blendMode;

  commandContextDirect_->GetCommandList()->SetPipelineState(
      graphicsPSObjects[static_cast<uint32_t>(blendMode_)]->GetPSO());
}

void AtrumEngine::SetBlendModeForFlame(const BlendMode &blendMode) {
  // PSOを設定
  commandContextDirect_->GetCommandList()->SetPipelineState(
      graphicsPSObjects[static_cast<uint32_t>(blendMode)]->GetPSO());
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