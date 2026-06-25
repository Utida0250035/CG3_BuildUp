#include "AssetModel.h"
#include "AtrumEngine.h"
#include "CommandContext.h"
#include "ConvertString.h"
#include "CreateBufferResource.h"
#include "DirectInput.h"
#include "Hash64.h"
#include "Log.h"
#include "Matrix3D.h"
#include "Plane.h"
#include "PlayInput.h"
#include "RenderDevice.h"
#include "StaticCast.h"
#include "Vector4.h"
#include <cassert>
#include <cfloat>
#include <cstdint>
#include <DirectXTex/d3dx12.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <numbers>
#include <SDL.h>
#include <SDL_syswm.h>
#include <sstream>
#include <string>
#include <strsafe.h>
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

#ifdef USE_IMGUI

#include "ImGui.h"

#include <d3d12sdklayers.h>

#endif

AtrumEngine* AtrumEngine::instance_ = nullptr;


template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {

	// Dumpsフォルダを作成
	CreateDirectory(L"./Dumps", nullptr);

	// 現在時刻を名前に入れたファイルをDumpsフォルダ以下に作成
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d_%02d-%02d_%02d-%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	// processId(このexeのId)とクラッシュ(例外)の発生した、threadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();

	// 設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION miniDumpInformation{ 0 };
	miniDumpInformation.ThreadId = threadId;
	miniDumpInformation.ExceptionPointers = exception;
	miniDumpInformation.ClientPointers = TRUE;

	// Dumpを出力(MiniDumpNormalフラグで最低限の情報を出力させるようにする)
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &miniDumpInformation, nullptr, nullptr);

	/* 他に関連付けられているSEH例外ハンドラがあれば記述を追加する */

	return EXCEPTION_EXECUTE_HANDLER;

}

void AtrumEngine::PrepareWindow(const std::string& windowLabel, const int32_t& clientWidth, const int32_t& clientHeight) {

	assert(!isInitialized_ && "PrepareWindow() is initializeHelper");

	// SDLの初期化
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		// エラーハンドリング
		assert(false);
		return;

	}

	clientWidth_ = clientWidth;
	clientHeight_ = clientHeight;

	// ウィンドウの生成
	window_.ptr = SDL_CreateWindow(
		windowLabel.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		clientWidth,
		clientHeight,
		SDL_WINDOW_SHOWN
	);

	// DirectX連携のためにHWNDを取得
	SDL_SysWMinfo wmInfo{};
	SDL_VERSION(&wmInfo.version);

	if (SDL_GetWindowWMInfo(window_.ptr, &wmInfo)) {

		hwnd_ = wmInfo.info.win.window;

	}

	//SetWindowLongPtr(hwnd_, GWLP_WNDPROC, (LONG_PTR)WindowProc);

}

void AtrumEngine::ErrorSuppressionDebug() {

#ifdef _DEBUG

	assert(!isInitialized_ && "ErrorSuppressionDebug() is initializeHelper");

	ID3D12InfoQueue* infoQueue = nullptr;

	if (SUCCEEDED(renderDevice_->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		// 深刻なエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[] = {
			// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// エラー情報キューの解放
		infoQueue->Release();

	}

#endif

}

void AtrumEngine::CreateMaterialBuffer() {

	assert(!isInitialized_ && "CreateMaterialResource() is initializeHelper");

	materialBuffer_ = std::make_unique<MultiConstantBuffer<MaterialData>>();

	materialBuffer_->CreateBuffer(renderDevice_->GetDevice(), kMaxDrawCount);

}

void AtrumEngine::CreateTransformationBuffer() {

	assert(!isInitialized_ && "CreateWvpResource() is initializeHelper");

	transformationBuffer_ = std::make_unique<MultiConstantBuffer<TransformationData>>();

	transformationBuffer_->CreateBuffer(renderDevice_->GetDevice(), kMaxDrawCount);

}

void AtrumEngine::CreateVertexBuffer() {

	assert(!isInitialized_ && "CreateVertexResource() is initializeHelper");

	vertexBuffer_ = std::make_unique<VertexBuffer>();

	vertexBuffer_->CreateVertexBuffer(VertexBuffer::kVertexMaxDrawCount, renderDevice_->GetDevice());

}

void AtrumEngine::CreateIndexBuffer() {

	assert(!isInitialized_ && "CreateIndexResource() is initializeHelper");

	indexBuffer_ = std::make_unique<IndexBuffer>();

	indexBuffer_->CreateIndexBuffer(VertexBuffer::kVertexMaxDrawCount, renderDevice_->GetDevice());

}


void AtrumEngine::CreateDirectionalLightBuffer() {

	assert(!isInitialized_ && "CreateDirectionalLightResource() is initializeHelper");

	directionalLightBuffer_ = std::make_unique<SingleConstantBuffer<DirectionalLightData>>();

	directionalLightBuffer_->CreateBuffer(renderDevice_->GetDevice());

	// デフォルト値
	DirectionalLightData directionalLightData{};

	directionalLightData.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData.direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData.intensity = 1.0f;

	directionalLightBuffer_->SetData(directionalLightData);

}

void AtrumEngine::CreateSpriteVertexBuffer() {

	assert(!isInitialized_ && "CreateSpriteVertexResource() is initializeHelper");

	spriteVertexBuffer_ = std::make_unique<VertexBuffer>();

	spriteVertexBuffer_->CreateVertexBuffer(VertexBuffer::kSpriteVertexMaxDrawCount, renderDevice_->GetDevice());

}

void AtrumEngine::CreateSpriteIndexBuffer() {

	assert(!isInitialized_ && "CreateSpriteIndexResource() is initializeHelper");

	spriteIndexBuffer_ = std::make_unique<IndexBuffer>();

	spriteIndexBuffer_->CreateIndexBuffer(VertexBuffer::kSpriteVertexMaxDrawCount, renderDevice_->GetDevice());

}

void AtrumEngine::CreateSpriteMaterialBuffer() {

	assert(!isInitialized_ && "CreateSpriteMaterialResource() is initializeHelper");

	spriteMaterialBuffer_ = std::make_unique<MultiConstantBuffer<MaterialData>>();

	spriteMaterialBuffer_->CreateBuffer(renderDevice_->GetDevice(), kSpriteMaxDrawCount);

	LogFile::GetInstance()->Log("Created MaterialResource");

}

void AtrumEngine::CreateSpriteTransformationBuffer() {

	assert(!isInitialized_ && "CreateSpriteTransformationResource() is initializeHelper");

	spriteTransformationBuffer_ = std::make_unique<MultiConstantBuffer<TransformationData>>();

	spriteTransformationBuffer_->CreateBuffer(renderDevice_->GetDevice(), kSpriteMaxDrawCount);

}

void AtrumEngine::SetUpViewport() {

	assert(!isInitialized_ && "SetUpViewport() is initializeHelper");

	// クライアント領域のサイズと同等にして画面全体を表示領域とする

	viewport_.Width = static_cast<FLOAT>(clientWidth_);
	viewport_.Height = static_cast<float>(clientHeight_);
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

}

void AtrumEngine::SetUpScissorRect() {

	assert(!isInitialized_ && "SetUpScissorRect() is initializeHelper");

	scissorRect_.left = 0;
	scissorRect_.right = clientWidth_;
	scissorRect_.top = 0;
	scissorRect_.bottom = clientHeight_;

}

void AtrumEngine::DrawTriangleCall(const uint32_t& textureIndex) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	assert(constantBufferCount_ + 1 < kMaxDrawCount && "constantBufferCount over maxCount(Sphere)");

	// TransformMatrix (WVP) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformationBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(TransformationData));
	// GPUに設定(rootParameter0)
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// Material (Color) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(MaterialData));
	// GPUに設定(rootParameter1)
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	// GPUに設定(rootParameter3)
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightBuffer_->GetGpuVirtualAddress());

	DescriptorAllocator::DescriptorHandle textureHandle{};

	textureHandle = srvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 rootParameter[2]
	commandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 3頂点で1つのインスタンス
	commandContextDirect_->GetCommandList()->DrawIndexedInstanced(3, 1, indexBuffer_->GetDrewCount(), vertexBuffer_->GetDrewCount(), 0);

	vertexBuffer_->AddDrewCount(3);
	indexBuffer_->AddDrewCount(3);

	constantBufferCount_++;

}

void AtrumEngine::DrawSphereCall(const uint32_t& textureIndex, const uint32_t& indexDataCountInSphere, const uint32_t& vertexCountInSphere) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	assert(constantBufferCount_ + 1 < kMaxDrawCount && "constantBufferCount over maxCount(Sphere)");

	// TransformMatrix (WVP) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformationBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(TransformationData));
	// GPUに設定(rootParameter0)
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// Material (Color) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(MaterialData));

	// GPUに設定(rootParameter1)
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	// GPUに設定(rootParameter3)
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightBuffer_->GetGpuVirtualAddress());

	DescriptorAllocator::DescriptorHandle textureHandle{};

	textureHandle = srvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
	commandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 3頂点で1つのインスタンス
	commandContextDirect_->GetCommandList()->DrawIndexedInstanced(indexDataCountInSphere, 1, indexBuffer_->GetDrewCount(), vertexBuffer_->GetDrewCount(), 0);

	indexBuffer_->AddDrewCount(indexDataCountInSphere);

	vertexBuffer_->AddDrewCount(vertexCountInSphere);

	constantBufferCount_++;

}

void AtrumEngine::SetFps(const int32_t& fps) {

	secondsPerFrame_ = 1.0f / static_cast<float>(fps);

}

void AtrumEngine::Initialize(const std::string& windowLabel, const int32_t& clientWidth, const int32_t& clientHeight) {

	assert(!isInitialized_ && "AtrumEngine is already initialized");

	// SEH例外が補足されなかった場合(Unhandled)に補足する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

#ifdef _DEBUG

	ComPtr<ID3D12Debug1> debugController;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

		// デバッグレイヤーを有効化
		debugController->EnableDebugLayer();

		// GPU側でもチェックを行なうようにする
		debugController->SetEnableGPUBasedValidation(TRUE);

	}

#endif

	// COMの初期化
	hr_ = CoInitializeEx(0, COINIT_MULTITHREADED);

	// COMの初期化が失敗したら起動不可
	assert(SUCCEEDED(hr_));

	// ログ出力ファイルの初期化
	LogFile::GetInstance()->Initialize();

	this->PrepareWindow(windowLabel, clientWidth, clientHeight);

	// レンダリングデバイスを生成
	renderDevice_ = std::make_unique<RenderDevice>();
	// レンダリングデバイスを初期化
	renderDevice_->Initialize();

#ifdef _DEBUG

	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(renderDevice_->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		// 致命的なエラー時にブレーク
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		// 通常のエラー時にブレーク
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		// 警告時にブレーク
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

	}

#endif

	// 描画コマンド経路を生成
	commandContextDirect_ = std::make_unique<CommandContext>();
	// 描画コマンド経路を初期化
	commandContextDirect_->Initialize(renderDevice_->GetDevice(), SwapChain::kBackBufferCount, D3D12_COMMAND_LIST_TYPE_DIRECT);


	swapChainManager_ = std::make_unique<SwapChain>();
	rtvAllocator_ = std::make_unique<DescriptorAllocator>();

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	swapChainManager_->Initialize(clientWidth_, clientHeight_, renderDevice_->GetDevice(), renderDevice_->GetDxgiFactory(), commandContextDirect_->GetCommandQueue(), hwnd_, rtvAllocator_.get(), rtvDesc);


	// SRVディスクリプタヒープの生成
	srvAllocator_ = std::make_unique<DescriptorAllocator>();
	srvAllocator_->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true, L"srvDescriptors", renderDevice_->GetDevice());

	// fenceの管理インスタンスを生成
	fenceManager_ = std::make_unique<Fence>();
	fenceManager_->Initialize(renderDevice_->GetDevice(), SwapChain::kBackBufferCount);

	// ルートシグネチャの作成
	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->Initialize(renderDevice_->GetDevice());

	// シェーダーコンパイラの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	// 必要なシェーダーのコンパイル
	shaderCompiler_->CompileShaders();

	// PSOの生成
	graphicsPipelineState_ = std::make_unique<PipelineState>();
	graphicsPipelineState_->Initialize(rootSignature_->GetRootSignature(), renderDevice_->GetDevice(), shaderCompiler_->GetVertexShaderBlob(), shaderCompiler_->GetPixelShaderBlob());

	// ビューポートの設定
	this->SetUpViewport();

	// シザー矩形の設定
	this->SetUpScissorRect();

	// MaterialBufferの生成
	this->CreateMaterialBuffer();

	// TransformationBufferの生成
	this->CreateTransformationBuffer();

	// VertexBufferの生成
	this->CreateVertexBuffer();

	// IndexBufferの生成
	this->CreateIndexBuffer();

	// 平行光源Bufferの生成
	this->CreateDirectionalLightBuffer();

	// 深度ステンシルリソースの生成
	depthStencilResource_ = this->CreateDepthStencilResource(clientWidth_, clientHeight_);

	// 深度ステンシルディスクリプタの生成
	dsvAllocator_ = std::make_unique<DescriptorAllocator>();
	dsvAllocator_->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false, L"dsvDescriptor", renderDevice_->GetDevice());

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	// Format 基本Resourceに合わせる
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 2dTexture
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	// DSVHeapの先頭にDSVを作る
	renderDevice_->GetDevice()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvAllocator_->GetCpuStart());


	/* Sprite */

	// Sprite用 MaterialBufferの作成
	this->CreateSpriteMaterialBuffer();

	// Sprite用 TransformationMatrixBufferの生成
	this->CreateSpriteTransformationBuffer();

	// Sprite用 VertexBufferの生成
	this->CreateSpriteVertexBuffer();

	// Sprite用 IndexBufferの生成
	this->CreateSpriteIndexBuffer();

	/* ImGui */

#ifdef USE_IMGUI

	// ImGuiの初期化

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForD3D(window_.ptr);

	DescriptorAllocator::DescriptorHandle imguiSrvHandle{};

	imguiSrvHandle = srvAllocator_->Allocate();

	ImGui_ImplDX12_Init(
		renderDevice_->GetDevice(),
		SwapChain::kBackBufferCount,
		rtvDesc.Format,
		srvAllocator_->GetDescriptorHeap(),
		imguiSrvHandle.cpu,
		imguiSrvHandle.gpu
	);

	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->Build();

#endif

#ifdef _DEBUG

	// 浮動小数点例外を有効にする
	unsigned int currentControl;


	// 0除算 (_EM_ZERODIVIDE) と 無効な操作（NaN発生など）(_EM_INVALID) を有効化

	_controlfp_s(&currentControl, 0u, _MCW_EM);

	_controlfp_s(&currentControl, static_cast<unsigned int>(~(_EM_ZERODIVIDE | _EM_INVALID)), _MCW_EM);

#endif

	// 時間差分マネージャーの生成
	deltaTimeManager_.reset(new DeltaTime());

	/* 入力デバイスの初期化 */

	SDL_SysWMinfo wmInfo{};
	SDL_VERSION(&wmInfo.version);

	if (!SDL_GetWindowWMInfo(window_.ptr, &wmInfo)) {

		assert(false);

	}

	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(wmInfo.info.win.window, GWLP_HINSTANCE));

	// DirectInput
	directInput_ = DirectInput::GetInstance();
	directInput_->Initialize(hInstance, hwnd_);

	// SDL2入力
	playInput_ = PlayInput::GetInstance();

	/* 初期化完了のログ出力 */

	LogFile::GetInstance()->Log("Hello World!");

	isInitialized_ = true;

}

bool AtrumEngine::Process() const {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	PlayInput::GetInstance()->EndOfFrame();

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

				playInput_->SetKey(static_cast<uint8_t>(event.key.keysym.scancode), false);

				break;

			case SDL_MOUSEBUTTONDOWN:

				playInput_->SetMouseButton(event.button.button - 1, true);

				LogFile::GetInstance()->Log("Mouse: " + std::to_string(event.button.button - 1));

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
	barrier.Transition.pResource = swapChainManager_->GetSwapChainResourceCurrent().Get();

	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	// TransitionBarrierを張る
	commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

	// 描画先のRTVとDSVを設定
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvAllocator_->GetCpuStart();
	commandContextDirect_->GetCommandList()->OMSetRenderTargets(1, swapChainManager_->PGetRtvHandleCurrent(), false, &dsvHandle);

	// 指定した深度(1.0f)で画面全体をクリアする
	commandContextDirect_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 指定色で画面全体をクリアする
	float clearColor[] = { 0.0625f, 0.1875f, 0.125f, 1.0f };
	commandContextDirect_->GetCommandList()->ClearRenderTargetView(swapChainManager_->GetRtvHandleCurrent(), clearColor, 0, nullptr);

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvAllocator_->GetDescriptorHeap() };
	commandContextDirect_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	// Viewportを設定
	commandContextDirect_->GetCommandList()->RSSetViewports(1, &viewport_);

	// ScissorRectを設定
	commandContextDirect_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);

	// RootSignatureを設定 PSOに設定しているが別途の設定が必要
	commandContextDirect_->GetCommandList()->SetGraphicsRootSignature(rootSignature_->GetRootSignature());

	// PSOを設定
	commandContextDirect_->GetCommandList()->SetPipelineState(graphicsPipelineState_->GetPSO());

	// VBVを設定
	commandContextDirect_->GetCommandList()->IASetVertexBuffers(0, 1, vertexBuffer_->PGetVertexBufferView());

	// IBVを設定
	commandContextDirect_->GetCommandList()->IASetIndexBuffer(indexBuffer_->PGetBufferView());

	// 形状を設定 PSOに設定しているものとは別で同じものを設定すると考える
	commandContextDirect_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルCBufferの場所を設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGpuVirtualAddress());

	// Transformation用のCBufferの場所を設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationBuffer_->GetGpuVirtualAddress());

	// directionalLight用のCBufferの場所を設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightBuffer_->GetGpuVirtualAddress());

}

void AtrumEngine::PostDraw() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

#ifdef USE_IMGUI

	// ImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandContextDirect_->GetCommandList());

#endif

	D3D12_RESOURCE_BARRIER barrier{};

	// バリアの種類
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	// バリアフラグ
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// サブリソース
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	// バリアを張る対象のリソース(現在のバックバッファに対して行なう)
	barrier.Transition.pResource = swapChainManager_->GetSwapChainResourceCurrent().Get();

	// 画面に描く処理が終了し画面に映すため状態を遷移
	// RenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// TransitionBarrierを張る
	commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる
	hr_ = commandContextDirect_->GetCommandList()->Close();
	assert(SUCCEEDED(hr_));

	// GPUにコマンドリストを実行させる
	ID3D12CommandList* commandLists[] = { commandContextDirect_->GetCommandList() };
	commandContextDirect_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	fenceManager_->Signal(commandContextDirect_->GetCommandQueue(), swapChainManager_->GetBackBufferIndex());

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

	hr_ = commandContextDirect_->GetCommandAllocator(swapChainManager_->GetBackBufferIndex())->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandContextDirect_->GetCommandList()->Reset(commandContextDirect_->GetCommandAllocator(swapChainManager_->GetBackBufferIndex()), nullptr);
	assert(SUCCEEDED(hr_));


	// 描画頂点数のカウントをリセット
	spriteVertexBuffer_->ResetDrewCount();
	vertexBuffer_->ResetDrewCount();

	// 頂点インデックス数のカウントをリセット
	spriteIndexBuffer_->ResetDrewCount();
	indexBuffer_->ResetDrewCount();

	// 定数バッファのカウントをリセット
	constantBufferCount_ = 0;
	spriteConstantBufferCount_ = 0;

}

void AtrumEngine::Finalize() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// GPUの完了を待つ
	fenceManager_->ForceSyncGPU(commandContextDirect_->GetCommandQueue());

	/* 解放処理 */

	textures_.clear();
	temporaryResources_.clear();

#ifdef USE_IMGUI

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

#endif

	// fenceManager_.reset();

	/* */

	// COMの終了処理
	CoUninitialize();

	CloseWindow(hwnd_);

}


Matrix4x4 AtrumEngine::CreateWorldMatrix(const Transform& transform) const {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	Matrix4x4 result = MakeWorldMatrix(transform.translate, transform.scale, transform.rotate);

	return result;

}

ComPtr<ID3D12Resource> AtrumEngine::CreateIntermediateResource(const size_t intermediateSize) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	/* 受け取ったサイズでUPLOADヒープのリソースを作成 */
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC bufferDesc{};
	bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	bufferDesc.Alignment = 0;

	// 受け取ったサイズを指定
	bufferDesc.Width = intermediateSize;

	bufferDesc.Height = 1;
	bufferDesc.DepthOrArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ComPtr<ID3D12Resource> intermediateResource = nullptr;
	hr_ = renderDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&intermediateResource)
	);

	assert(SUCCEEDED(hr_));

	return intermediateResource;

}

DirectX::ScratchImage AtrumEngine::LoadTexture(const std::string& filePath) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	std::wstring filePathBuffer = StringToWString(filePath);

	if (!std::filesystem::exists(filePath)) {
		// ファイルが見つからなければ失敗
		std::wstring errorMessage = L"NotFoundFile:\n" + filePathBuffer;
		LogFile::GetInstance()->Log(WStringToString(errorMessage));
		MessageBoxW(nullptr, errorMessage.c_str(), L"Not Found Texture", MB_OK | MB_ICONERROR);

		assert(false && "textureFile not found");

	}

	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	hr_ = DirectX::LoadFromWICFile(filePathBuffer.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr_));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr_ = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr_));

	// ミップマップ付きのデータを返す
	return mipImages;

}

ComPtr<ID3D12Resource> AtrumEngine::CreateTextureResource(const DirectX::TexMetadata& metaData) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	/* metaDataを基にResourceの設定 */
	D3D12_RESOURCE_DESC resourceDesc{};
	// Textureの幅
	resourceDesc.Width = UINT(metaData.width);
	// Textureの高さ
	resourceDesc.Height = UINT(metaData.height);
	// mipMapの数
	resourceDesc.MipLevels = UINT16(metaData.mipLevels);
	// 奥行き or 配列Textureの要素数
	resourceDesc.DepthOrArraySize = UINT16(metaData.arraySize);
	// TextureのFormat
	resourceDesc.Format = metaData.format;
	// サンプリングカウント 1固定
	resourceDesc.SampleDesc.Count = 1;
	// Textureの次元数 普段(画像)は2次元
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);

	/* 利用するHeapの設定 */
	D3D12_HEAP_PROPERTIES heapProperties{};
	// VRAM上に作成
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	/* Resourceの生成 */
	ComPtr<ID3D12Resource> resource = nullptr;
	hr_ = renderDevice_->GetDevice()->CreateCommittedResource(
		// Heapの設定
		&heapProperties,
		// Heapの特殊な設定 特に無し
		D3D12_HEAP_FLAG_NONE,
		// Resourceの設定
		&resourceDesc,
		// 初回のResourceState データ転送される設定
		D3D12_RESOURCE_STATE_COPY_DEST,
		// Clear最適値 使わないのでnullptr
		nullptr,
		// 作成するResourceポインタへのポインタ
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr_));

	return resource;

}

ComPtr<ID3D12Resource> AtrumEngine::CreateDepthStencilResource(int32_t width, int32_t height) {

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
	hr_ = renderDevice_->GetDevice()->CreateCommittedResource(
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
		IID_PPV_ARGS(&resource)
	);

	assert(SUCCEEDED(hr_));

	return resource;

}

ComPtr<ID3D12Resource> AtrumEngine::CreateTextureIntermediateResource(ID3D12Resource* textureResource) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// テクスチャの設定を取得
	D3D12_RESOURCE_DESC texDesc = textureResource->GetDesc();
	uint64_t intermediateSize = 0;

	/*GPU上のレイアウトに基づき、必要な総書き込みサイズを計算*/
	// GetCopyableFootPrints()は、テクスチャの各サブリソースが
	// UPLOADバッファ上でどこに配置されるべきか(オフセットやピッチ)を計算する
	renderDevice_->GetDevice()->GetCopyableFootprints(
		// テクスチャの情報をもとにする
		&texDesc,
		// 最初のサブリソースインデックス
		0,
		// 全ミップレベル分を計算
		texDesc.MipLevels,
		// バッファ上の開始オフセット
		0,
		// レイアウト詳細 不要
		nullptr,
		// 行数 不要
		nullptr,
		// 行サイズ 不要
		nullptr,
		// 合計の必要バイト数の受け取り
		&intermediateSize
	);

	// 受け取ったサイズのUPLOADヒープのリソースを生成して参照元へ
	return this->CreateIntermediateResource(intermediateSize);

}

void AtrumEngine::UploadTextureData(ID3D12Resource* textureResource, const DirectX::ScratchImage& mipImages, ID3D12Resource* intermediateResource) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	UINT subresourceCount = static_cast<UINT>(mipImages.GetImageCount());

	/* レイアウト情報の取得 */
	// GPU上のメモリ配置(アライメント)に合わせたコピー情報を取得する
	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts(subresourceCount);
	std::vector<UINT> numRows(subresourceCount);
	std::vector<UINT64> rowSizeInBytes(subresourceCount);
	UINT64 totalBytes = 0;

	D3D12_RESOURCE_DESC desc = textureResource->GetDesc();
	renderDevice_->GetDevice()->GetCopyableFootprints(
		&desc,
		0,
		subresourceCount,
		0,
		layouts.data(),
		numRows.data(),
		rowSizeInBytes.data(),
		&totalBytes
	);

	/* CPUから中間リソース(UPLOAD)への書き込み(Map > memcpy > UnMap) */
	uint8_t* pDest = nullptr;
	intermediateResource->Map(0, nullptr, reinterpret_cast<void**>(&pDest));

	for (UINT i = 0; i < subresourceCount; ++i) {

		// 各ミップレベルで配列、深さを取得
		const DirectX::Image* image = mipImages.GetImage(i, 0, 0);

		uint8_t* pDestSubresource = pDest + layouts[i].Offset;
		const uint8_t* pSrcSubresource = image->pixels;

		for (UINT y = 0; y < numRows[i]; ++y) {
			// 行ごとにコピー(GPUのアライメント/ピッチに合わせるため)

			std::memcpy(
				// 書き込み先(アライメント考慮済み)
				pDestSubresource + (y * layouts[i].Footprint.RowPitch),
				// 読み込み元
				pSrcSubresource + (y * image->rowPitch),
				// 1行の有効なバイト数
				rowSizeInBytes[i]
			);

		}

	}

	intermediateResource->Unmap(0, nullptr);

	/* 中間リソースからテクスチャへのコピー命令を積む */
	for (UINT i = 0; i < subresourceCount; ++i) {

		D3D12_TEXTURE_COPY_LOCATION destinationLocation{};
		destinationLocation.pResource = textureResource;
		destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destinationLocation.SubresourceIndex = i;

		D3D12_TEXTURE_COPY_LOCATION sourceLocation{};
		sourceLocation.pResource = intermediateResource;
		sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		sourceLocation.PlacedFootprint = layouts[i];

		commandContextDirect_->GetCommandList()->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);

	}

	/* バリアを張って利用可能な状態にする */
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textureResource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

}

void AtrumEngine::MakeShaderResourceView(Texture& texture, const DirectX::TexMetadata& metaData) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// 2Dテクスチャ
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);

	// SRVを作成するDescriptionHeapの場所を決める

	DescriptorAllocator::DescriptorHandle handle{};

	handle = srvAllocator_->Allocate();

	texture.srvHandleCPU = handle.cpu;
	texture.srvHandleGPU = handle.gpu;
	texture.srvIndex = handle.index;

	// SRVの作成
	renderDevice_->GetDevice()->CreateShaderResourceView(texture.resource.Get(), &srvDesc, texture.srvHandleCPU);

}

uint32_t AtrumEngine::GetTexture(const std::string& filePath) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// これから追加するテクスチャがImGui込みのsrvDescriptorHeapに収まらなければエラー
	assert(textures_.size() + 1 < srvAllocator_->GetMaxDescriptorCount() - 1 && "srvDescriptorHeap is over maxCount");

	auto search = textureIndexTable_.find(hash64_str(filePath.c_str()));

	if (search != textureIndexTable_.end()) {

		return search->second;

	}

	Texture texture;

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = this->LoadTexture(filePath);
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();

	texture.resource = this->CreateTextureResource(metaData);

	assert(texture.resource);

	ComPtr<ID3D12Resource> intermediateResource = this->CreateTextureIntermediateResource(texture.resource.Get());

	// 中間リソースを用いた転送
	this->UploadTextureData(texture.resource.Get(), mipImages, intermediateResource.Get());

	// コマンドリストの内容を確定させる
	hr_ = commandContextDirect_->GetCommandList()->Close();
	assert(SUCCEEDED(hr_));

	// GPUにコマンドリストを実行させる
	ID3D12CommandList* commandLists[] = { commandContextDirect_->GetCommandList() };
	commandContextDirect_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	// 次のフレームの準備
	// 現在のフレームが「いつ終わるか」を Signal
	// 「次に使う予定のアロケータ」が解放されているかを確認して Wait する

	// 現在のフレームに完了番号を割り振って Signal
	fenceManager_->Signal(commandContextDirect_->GetCommandQueue(), swapChainManager_->GetBackBufferIndex());

	// 次のフレームのインデックスを取得する
	swapChainManager_->UpdateBackBufferIndex();

	// これから使うアロケータが前回の実行を終えているか確認
	fenceManager_->WaitForNextBuffer(swapChainManager_->GetBackBufferIndex());

	// 次のフレーム用のアロケータとリストをリセット
	hr_ = commandContextDirect_->GetCommandAllocator(swapChainManager_->GetBackBufferIndex())->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandContextDirect_->GetCommandList()->Reset(commandContextDirect_->GetCommandAllocator(swapChainManager_->GetBackBufferIndex()), nullptr);
	assert(SUCCEEDED(hr_));

	// 実際にShaderResourceViewを作る
	MakeShaderResourceView(texture, metaData);

	// 中間リソースを一時保存
	temporaryResources_.emplace_back(intermediateResource);

	// ファイル名からのハッシュとSRV番号を格納
	textureIndexTable_.emplace(hash64_str(filePath.c_str()), texture.srvIndex);

	// 配列に所有権を移動
	textures_.emplace_back(texture);

	LogFile::GetInstance()->Log("Got Texture: " + filePath);

	// 作成したテクスチャの番号を返す
	return textures_.back().srvIndex;

}

void AtrumEngine::DrawTriangle(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& triangleTransform, const std::array<VertexData, 3>& vertexData, const bool isLighting) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// 三角形のTransform
	Matrix4x4 triangleWorldMatrix = this->CreateWorldMatrix(triangleTransform);


	TransformationData transformationData{};

	transformationData.wvp = triangleWorldMatrix * viewMatrix_ * kPerspectiveFovMatrix;
	transformationData.world = triangleWorldMatrix;

	transformationBuffer_->SetData(transformationData, constantBufferCount_);


	MaterialData materialData{};

	materialData.color = textureColor;

	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix *= MakeZRotateMatrix(uvTransform.rotate.z);
	uvTransformMatrix *= MakeTranslateMatrix(uvTransform.translate);
	materialData.uvTransformMatrix = uvTransformMatrix;

	materialData.inLightingEnable = isLighting;

	materialBuffer_->SetData(materialData, constantBufferCount_);


	uint32_t vertexDrewCount = vertexBuffer_->GetDrewCount();

	vertexBuffer_->SetVertexData(vertexData[0], vertexDrewCount);

	vertexBuffer_->SetVertexData(vertexData[1], vertexDrewCount + 1);

	vertexBuffer_->SetVertexData(vertexData[2], vertexDrewCount + 2);


	uint32_t indexDrewCount = indexBuffer_->GetDrewCount();

	indexBuffer_->SetIndexData(0, indexDrewCount);
	indexBuffer_->SetIndexData(1, indexDrewCount + 1);
	indexBuffer_->SetIndexData(2, indexDrewCount + 2);


	// 描画
	this->DrawTriangleCall(textureIndex);

}

void AtrumEngine::DrawSphere(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& sphereTransform, const float radius, const uint32_t subdivision, const bool isLighting) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// 球のTransform
	Matrix4x4 sphereWorldMatrix = this->CreateWorldMatrix(sphereTransform);

	TransformationData transformationData{};

	transformationData.wvp = sphereWorldMatrix * viewMatrix_ * kPerspectiveFovMatrix;
	transformationData.world = sphereWorldMatrix;

	transformationBuffer_->SetData(transformationData, constantBufferCount_);


	MaterialData materialData{};

	materialData.color = textureColor;

	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix *= MakeZRotateMatrix(uvTransform.rotate.z);
	uvTransformMatrix *= MakeTranslateMatrix(uvTransform.translate);
	materialData.uvTransformMatrix = uvTransformMatrix;

	materialData.inLightingEnable = isLighting;

	materialBuffer_->SetData(materialData, constantBufferCount_);


	const float kLonEvery = std::numbers::pi_v<float> *2.0f / cast::Float(subdivision);
	const float kLatEvery = std::numbers::pi_v<float> / cast::Float(subdivision);

	VertexData pointA{}, pointB{}, pointC{}, pointD{};

	float lat = 0.0f;
	float lon = 0.0f;

	uint32_t vertexDataCount = vertexBuffer_->GetDrewCount();
	uint32_t indexDataCount = indexBuffer_->GetDrewCount();
	uint32_t boardCountInSphere = 0;
	uint32_t baseIndex = 0;

	for (uint32_t latIndex = 0; latIndex < subdivision; ++latIndex) {

		lat = -(std::numbers::pi_v<float> *0.5f) + kLatEvery * cast::Float(latIndex);

		for (uint32_t lonIndex = 0; lonIndex < subdivision; ++lonIndex) {

			lon = static_cast<float>(lonIndex) * kLonEvery;

			pointA.position = Vector4{ cos(lat) * cos(lon), sin(lat), cos(lat) * sin(lon), 0.0f } * radius;
			pointA.position.w = 1.0f;
			pointA.texCoord = Vector2{ cast::Float(lonIndex) / cast::Float(subdivision), 1.0f - cast::Float(latIndex) / cast::Float(subdivision) };
			pointA.normal = VectorNormalize(Vector3{ pointA.position.x, pointA.position.y, pointA.position.z });

			pointB.position = Vector4{ cos(lat + kLatEvery) * cos(lon), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon), 0.0f } * radius;
			pointB.position.w = 1.0f;
			pointB.texCoord = Vector2{ cast::Float(lonIndex) / cast::Float(subdivision), 1.0f - cast::Float(latIndex + 1) / cast::Float(subdivision) };
			pointB.normal = VectorNormalize(Vector3{ pointB.position.x, pointB.position.y, pointB.position.z });

			pointC.position = Vector4{ cos(lat) * cos(lon + kLonEvery), sin(lat), cos(lat) * sin(lon + kLonEvery) , 0.0f } * radius;
			pointC.position.w = 1.0f;
			pointC.texCoord = Vector2{ cast::Float(lonIndex + 1) / cast::Float(subdivision), 1.0f - cast::Float(latIndex) / cast::Float(subdivision) };
			pointC.normal = VectorNormalize(Vector3{ pointC.position.x, pointC.position.y, pointC.position.z });

			pointD.position = Vector4{ cos(lat + kLatEvery) * cos(lon + kLonEvery), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery), 0.0f } * radius;
			pointD.position.w = 1.0f;
			pointD.texCoord = Vector2{ cast::Float(lonIndex + 1) / cast::Float(subdivision), 1.0f - cast::Float(latIndex + 1) / cast::Float(subdivision) };
			pointD.normal = VectorNormalize(Vector3{ pointD.position.x, pointD.position.y, pointD.position.z });


			vertexBuffer_->SetVertexData(pointA, vertexDataCount++);

			vertexBuffer_->SetVertexData(pointB, vertexDataCount++);

			vertexBuffer_->SetVertexData(pointC, vertexDataCount++);

			vertexBuffer_->SetVertexData(pointD, vertexDataCount++);


			// 0, 1, 2, 1, 3, 2の順を繰り返す
			// インスタンス内で頂点インデックスを0から数える

			baseIndex = boardCountInSphere * 4;

			indexBuffer_->SetIndexData(baseIndex, indexDataCount++);
			indexBuffer_->SetIndexData(baseIndex + 1, indexDataCount++);
			indexBuffer_->SetIndexData(baseIndex + 2, indexDataCount++);
			indexBuffer_->SetIndexData(baseIndex + 1, indexDataCount++);
			indexBuffer_->SetIndexData(baseIndex + 3, indexDataCount++);
			indexBuffer_->SetIndexData(baseIndex + 2, indexDataCount++);

			boardCountInSphere++;

		}

	}

	this->DrawSphereCall(textureIndex, indexDataCount - indexBuffer_->GetDrewCount(), vertexDataCount - vertexBuffer_->GetDrewCount());

}

void AtrumEngine::PrepareSprite() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// VertexBufferView(VBV)を設定
	commandContextDirect_->GetCommandList()->IASetVertexBuffers(0, 1, spriteVertexBuffer_->PGetVertexBufferView());

	// IndexBufferView(IBV)を設定
	commandContextDirect_->GetCommandList()->IASetIndexBuffer(spriteIndexBuffer_->PGetBufferView());

	// トランスフォームの定数バッファの最初のアドレスを設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, spriteTransformationBuffer_->GetGpuVirtualAddress());

	// マテリアルの定数バッファの最初のアドレスを設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, spriteMaterialBuffer_->GetGpuVirtualAddress());

}

void AtrumEngine::DrawSpriteRect(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& rectTransform, const Vector2& rectSize) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// 三角形のTransform
	Matrix4x4 worldMatrix = this->CreateWorldMatrix(rectTransform);

	TransformationData spriteTransformData{};

	spriteTransformData.wvp = worldMatrix * kOrthographicMatrix;
	spriteTransformData.world = worldMatrix;

	spriteTransformationBuffer_->SetData(spriteTransformData, spriteConstantBufferCount_);


	MaterialData spriteMaterialData{};

	spriteMaterialData.color = textureColor;
	spriteMaterialData.inLightingEnable = false;

	Matrix4x4 uvTransformData = MakeScaleMatrix(uvTransform.scale);
	uvTransformData *= MakeZRotateMatrix(uvTransform.rotate.z);
	uvTransformData *= MakeTranslateMatrix(uvTransform.translate);
	spriteMaterialData.uvTransformMatrix = uvTransformData;

	spriteMaterialBuffer_->SetData(spriteMaterialData, spriteConstantBufferCount_);


	/* 1枚目の三角形 */

	uint32_t vertexCount = spriteVertexBuffer_->GetDrewCount();

	Vector2 halfSize = rectSize * 0.5f;

	uint32_t indexCount = spriteIndexBuffer_->GetDrewCount();

	VertexData spriteVertexData{};

	// 法線の向きは全点共通
	spriteVertexData.normal = Vector3{ 0.0f, 0.0f, -1.0f };

	// 左下
	spriteVertexData.texCoord = { 0.0f, 1.0f };
	spriteVertexData.position = { -halfSize.x, halfSize.y, 0.0f, 1.0f };

	spriteVertexBuffer_->SetVertexData(spriteVertexData, vertexCount++);

	// 左上
	spriteVertexData.texCoord = { 0.0f, 0.0f };
	spriteVertexData.position = { -halfSize.x, -halfSize.y, 0.0f, 1.0f };

	spriteVertexBuffer_->SetVertexData(spriteVertexData, vertexCount++);

	// 右下
	spriteVertexData.texCoord = { 1.0f, 1.0f };
	spriteVertexData.position = { halfSize.x, halfSize.y, 0.0f, 1.0f };

	spriteVertexBuffer_->SetVertexData(spriteVertexData, vertexCount++);

	// 右上
	spriteVertexData.texCoord = { 1.0f, 0.0f };
	spriteVertexData.position = { halfSize.x, -halfSize.y, 0.0f, 1.0f };

	spriteVertexBuffer_->SetVertexData(spriteVertexData, vertexCount++);

	// インスタンス内で頂点インデックスを0から数える
	spriteIndexBuffer_->SetIndexData(0, indexCount++);
	spriteIndexBuffer_->SetIndexData(1, indexCount++);
	spriteIndexBuffer_->SetIndexData(2, indexCount++);
	spriteIndexBuffer_->SetIndexData(1, indexCount++);
	spriteIndexBuffer_->SetIndexData(3, indexCount++);
	spriteIndexBuffer_->SetIndexData(2, indexCount++);

	// 描画
	this->DrawSpriteCall(textureIndex);

}

void AtrumEngine::DrawSpriteLine(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Vector2& start, const Vector2& end, const float& width, const float& posZ) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	Vector2 difference = end - start;
	float length = VectorLength(difference);

	Vector2 rectPos = start + difference * 0.5f;

	Transform rectTransform{};
	rectTransform.translate = { rectPos.x, rectPos.y, posZ };
	rectTransform.scale = { 1.0f, 1.0f, 1.0f };
	rectTransform.rotate = { 0.0f, 0.0f, std::atan2(difference.y, difference.x) };

	DrawSpriteRect(textureIndex, textureColor, uvTransform, rectTransform, Vector2{ length, width });

}

void AtrumEngine::DrawSpriteCall(const uint32_t& textureIndex) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	assert(spriteConstantBufferCount_ + 1 < kSpriteMaxDrawCount);

	// TransformMatrix (WVP) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = spriteTransformationBuffer_->GetGpuVirtualAddress() + (spriteConstantBufferCount_ * sizeof(TransformationData));
	// GPUに設定(rootParameter0)
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// Material (Color) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = spriteMaterialBuffer_->GetGpuVirtualAddress() + (spriteConstantBufferCount_ * sizeof(MaterialData));
	// GPUに設定(rootParameter1)
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	DescriptorAllocator::DescriptorHandle textureHandle{};
	textureHandle = srvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
	commandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 6頂点インデックスで1つのインスタンス
	commandContextDirect_->GetCommandList()->DrawIndexedInstanced(6, 1, spriteIndexBuffer_->GetDrewCount(), spriteVertexBuffer_->GetDrewCount(), 0);

	spriteIndexBuffer_->AddDrewCount(6);

	spriteVertexBuffer_->AddDrewCount(4);

	spriteConstantBufferCount_++;

}

std::vector<AssetMeshNode> AtrumEngine::LoadObjFile(const std::string& directoryPath, const std::string& fileName, std::vector<std::string>& useMaterialNames) {

	// 戻り値用 mtl部分は空
	std::vector<AssetMeshNode> assetMeshNodes{};

	// メッシュ生成用
	std::shared_ptr<AssetMeshData> assetMeshData = std::make_shared<AssetMeshData>();

	// 位置
	std::vector<Vector4> positions{};
	// 法線
	std::vector<Vector3> normals{};
	// テクスチャ座標
	std::vector<Vector2> texCoords{};

	// ファイル1行分
	std::string line;

	// ファイルのパス
	std::string filePath = directoryPath + "/" + fileName;

	// ファイルからの入力
	std::ifstream file(filePath);
	// 開けなかったらエラー
	assert(file.is_open());

	bool isMeshExist = false;
	bool isSkippedUsemtl = false;

	std::string meshName = "";

	while (std::getline(file, line)) {

		std::string identifier;
		std::stringstream s(line);

		// 先頭の識別子を読む
		s >> identifier;

		switch (hash64_str(identifier.c_str())) {
			case "o"_hash64:
			{

				if (isSkippedUsemtl) {
					// usemtlを飛ばしたあとなら

					// 飛ばしフラグを折る
					isSkippedUsemtl = false;

					// メッシュの既存フラグを折る
					isMeshExist = false;

				}

				if (isMeshExist) {

					break;

				}

				meshName.clear();

				s >> meshName;

				meshName = filePath + "?" + meshName;

				auto search = assetMeshMap_.find(hash64_str(meshName));

				if (search != assetMeshMap_.end()) {
					// メッシュが作成済テーブルに存在する場合

					// メッシュ作成完了 配列に保存
					assetMeshNodes.emplace_back(search->second.lock(), nullptr);

					useMaterialNames.emplace_back("");

					// 既存フラグを立てて次のusemtlまで処理を飛ばす
					isMeshExist = true;

					LogFile::GetInstance()->Log("GetMeshFromTable: " + meshName);

					break;

				}

				LogFile::GetInstance()->Log("LoadMesh: " + meshName);

				break;

			}

			case "v"_hash64:
			{

				if (isMeshExist) {

					break;

				}

				Vector4 position{};

				s >> position.x >> position.y >> position.z;
				position.w = 1.0f;

				positions.push_back(position);

				break;

			}

			case "vt"_hash64:
			{

				if (isMeshExist) {

					break;

				}

				Vector2 texCoord{};
				s >> texCoord.x >> texCoord.y;

				texCoords.push_back(texCoord);

				break;

			}

			case "vn"_hash64:
			{

				if (isMeshExist) {

					break;

				}

				Vector3 normal{ 0.0f, 1.0f, 0.0f };
				s >> normal.x >> normal.y >> normal.z;

				normals.push_back(normal);

				break;

			}

			case "f"_hash64:
			{

				if (isMeshExist) {

					break;

				}

				VertexData triangle[3]{};

				// 三角形の集合に限定 その他は対応しない

				for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {

					std::string vertexDefinition;
					s >> vertexDefinition;

					// 頂点の要素へのIndexは「位置/UV/法線」で格納されている
					// 分解してIndexを取得
					std::istringstream v(vertexDefinition);
					size_t elementIndices[3]{};

					for (size_t element = 0; element < 3; ++element) {

						std::string index;
						// /(スラッシュ)区切りでIndexを読んでいく
						std::getline(v, index, '/');
						elementIndices[element] = std::stoi(index);

					}

					Vector4 position = positions[elementIndices[0] - 1];
					position.z *= -1.0f;

					Vector2 texCoord = texCoords[elementIndices[1] - 1];
					texCoord.y = 1.0f - texCoord.y;

					Vector3 normal = normals[elementIndices[2] - 1];
					normal.z *= -1.0f;

					triangle[faceVertex] = { position, texCoord, normal };

				}

				assetMeshNodes.back().mesh->vertices.push_back(triangle[2]);
				assetMeshNodes.back().mesh->vertices.push_back(triangle[1]);
				assetMeshNodes.back().mesh->vertices.push_back(triangle[0]);

				break;

			}

			case "usemtl"_hash64:
			{

				if (isMeshExist) {

					isSkippedUsemtl = true;

					break;

				}

				std::string mtlName{};

				s >> mtlName;

				useMaterialNames.emplace_back(std::move(mtlName));

				// メッシュのポインタを配列に保存
				assetMeshNodes.emplace_back(assetMeshData, nullptr);

				// メッシュのポインタを作成済みテーブルに保存
				assetMeshMap_.emplace(hash64_str(meshName), assetMeshData);

				// 解放・再生成して次に備える
				assetMeshData.reset(new AssetMeshData());

				break;

			}

		}


	}

	file.close();

	size_t bufferSize = 0;

	HRESULT hr;

	for (auto& meshNode : assetMeshNodes) {

		auto& mesh = meshNode.mesh;

		if (mesh->vertexResource) {

			continue;

		}

		bufferSize = sizeof(VertexData) * mesh->vertices.size();

		mesh->vertexResource = CreateDefaultBuffer(bufferSize, renderDevice_->GetDevice());
		ComPtr<ID3D12Resource> intermediateResource = this->CreateIntermediateResource(bufferSize);

		void* pData = nullptr;

		hr = intermediateResource->Map(0u, nullptr, &pData);

		if (SUCCEEDED(hr)) {

			std::memcpy(pData, mesh->vertices.data(), bufferSize);
			intermediateResource->Unmap(0u, nullptr);

		} else {

			assert(false && "LoadObjFile() failed");

		}

		commandContextDirect_->GetCommandList()->CopyBufferRegion(
			mesh->vertexResource.Get(), 0u,
			intermediateResource.Get(), 0u,
			static_cast<UINT64>(bufferSize)
		);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			mesh->vertexResource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		);

		commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

		mesh->vertexBufferView.BufferLocation = mesh->vertexResource->GetGPUVirtualAddress();
		mesh->vertexBufferView.SizeInBytes = static_cast<UINT>(bufferSize);
		mesh->vertexBufferView.StrideInBytes = sizeof(VertexData);

		assetMeshMap_.emplace(hash64_str(filePath.c_str()), mesh);

		temporaryResources_.emplace_back(intermediateResource);

	}

	return assetMeshNodes;

}

std::vector<std::shared_ptr<AssetMaterialData>> AtrumEngine::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName) {

	// 戻り値用
	std::vector<std::shared_ptr<AssetMaterialData>> assetMaterialData{};

	// マテリアルデータ生成用
	std::shared_ptr<AssetMaterialData> assetMaterial = nullptr;

	// 色読み込み用
	std::vector<Vector4> colors{};

	// ライティングフラグ読み込み用
	std::vector<bool> lightingEnableData{};

	// ファイルの1行読み込み
	std::string line;

	// ファイルのパス
	std::string filePath = directoryPath + "/" + fileName;

	// ファイルからの入力
	std::fstream file(filePath);
	// ファイルが開けなければエラー
	assert(file.is_open());

	std::string textureFilePath;

	bool isMaterialExist = false;

	bool isMaterialUseTexture = false;

	std::string mtlName = "";

	while (std::getline(file, line)) {

		std::string identifier;
		std::istringstream s(line);

		s >> identifier;

		switch (hash64_str(identifier)) {

			case "map_Kd"_hash64:
			{

				if (isMaterialExist) {

					break;

				}

				std::string textureFileName;
				s >> textureFileName;

				// 連結してファイルパスにする
				textureFilePath = directoryPath + "/" + textureFileName;

				// ファイルパスを基にテクスチャ取得
				assetMaterial->textureSrvIndex = this->GetTexture(textureFilePath);

				assetMaterialData.emplace_back(assetMaterial);

				assetMaterialMap_.emplace(hash64_str(mtlName), assetMaterial);

				isMaterialUseTexture = true;

#ifdef _DEBUG

				assetMaterial->textureFilePathDebug = textureFilePath;

#endif

				break;

			}

			case "Kd"_hash64:
			{

				if (isMaterialExist) {

					break;

				}

				Vector4 color{};

				s >> color.x >> color.y >> color.z;
				color.w = 1.0f;

				colors.emplace_back(std::move(color));

				break;

			}

			case "illum"_hash64:
			{

				if (isMaterialExist) {

					break;

				}

				UINT illum = 0u;

				s >> illum;

				if (illum > 0u) {

					lightingEnableData.emplace_back(true);

				} else {

					lightingEnableData.emplace_back(false);

				}

				break;

			}

			case "newmtl"_hash64:
			{

				if (assetMaterial) {

					if (isMaterialUseTexture) {

						isMaterialUseTexture = false;

					} else {
						// マテリアルの作成が完了していない(テクスチャが貼られていない)場合

						colors.push_back(Vec4White());


#ifdef _DEBUG
						assetMaterial->textureFilePathDebug = "./Resources/Images/white4x4.png";
#endif

						assetMaterialData.emplace_back(assetMaterial);

						assetMaterialMap_.emplace(hash64_str(mtlName), assetMaterial);

					}

					isMaterialExist = false;

				}

				assetMaterial.reset(new AssetMaterialData());

				mtlName.clear();

				s >> mtlName;

				mtlName = filePath + "?" + mtlName;

				auto search = assetMaterialMap_.find(hash64_str(mtlName));

				if (search != assetMaterialMap_.end()) {

					assert(search->second.lock());

					assetMaterialData.emplace_back(search->second.lock());

					// 色配列に空データを追加
					colors.emplace_back();

					// ライティングフラグ配列に空データを追加
					lightingEnableData.emplace_back(false);

					isMaterialExist = true;

					LogFile::GetInstance()->Log("GetMtlFromTable: " + mtlName);

				}

				LogFile::GetInstance()->Log("LoadMtl: " + mtlName);

				break;

			}

		}

	}

	assert(assetMaterialData.size() == colors.size());
	assert(assetMaterialData.size() == lightingEnableData.size());

	for (size_t i = 0; i < assetMaterialData.size(); ++i) {

		if (assetMaterialData[i]->materialResource) {

			continue;

		}

		// マテリアルリソースの生成
		assetMaterialData[i]->materialResource = CreateUploadBuffer(sizeof(MaterialData), renderDevice_->GetDevice());
		// 書き込み用アドレスの確保
		assetMaterialData[i]->materialResource->Map(0u, nullptr, reinterpret_cast<void**>(&assetMaterialData[i]->materialData));

		assetMaterialData[i]->materialData->uvTransformMatrix = MakeIdentity4x4();

		assetMaterialData[i]->materialData->color = colors[i];

		assetMaterialData[i]->materialData->inLightingEnable = lightingEnableData[i];

	}

	return assetMaterialData;

}

std::shared_ptr<AssetModel> AtrumEngine::CreateModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName) {

	assert(fs::exists(directoryPathObj + "/" + objFileName));

	assert(fs::exists(directoryPathMtl + "/" + mtlFileName));

	std::shared_ptr<AssetModel> assetModelData = std::make_shared<AssetModel>();

	std::vector<std::string> useMaterialNames{};

	// メッシュデータ
	assetModelData->meshNodes_ = LoadObjFile(directoryPathObj, objFileName, useMaterialNames);

	// マテリアルデータの寿命保証
	std::vector<std::shared_ptr<AssetMaterialData>> assetMaterialData = LoadMaterialTemplateFile(directoryPathMtl, mtlFileName);

	for (size_t i = 0; i < assetModelData->meshNodes_.size(); ++i) {

		auto search = assetMaterialMap_.find(hash64_str(directoryPathMtl + "/" + mtlFileName + "?" + useMaterialNames[i]));

		assert(search != assetMaterialMap_.end());

		assetModelData->meshNodes_[i].material = search->second.lock();

	}

	assetMaterialData.clear();

	// 座標変換リソース・データ
	assetModelData->transformationResource_ = CreateUploadBuffer(sizeof(TransformationData), renderDevice_->GetDevice());
	assetModelData->transformationResource_->Map(0u, nullptr, reinterpret_cast<void**>(&assetModelData->transformationData_));

#ifdef _DEBUG

	assetModelData->mtlFilePathDebug_ = mtlFileName;
	assetModelData->objFilePathDebug_ = objFileName;

#endif

	// ファイル名(区切り連結)のハッシュ化
	uint64_t manageHash = hash64_str((objFileName + "|" + mtlFileName).c_str());

	// モデルテーブルへ追加
	assetModelMap_.emplace(manageHash, assetModelData);

	// モデルデータを参照元へ戻す
	return assetModelData;

}

std::shared_ptr<AssetModel> AtrumEngine::GetModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName) {

	// キー検索
	auto search = assetModelMap_.find(hash64_str((directoryPathObj + "/" + objFileName + "|" + directoryPathMtl + "/" + mtlFileName).c_str()));

	if (search != assetModelMap_.end()) {
		// 該当要素がモデルテーブルに見つかった場合

		if (search->second.lock()) {
			// 値が空でなければ戻り値とする

			return search->second.lock();

		}

		// 値が空なら要素を消去する
		assetModelMap_.erase(search);

	}

	// 無ければ新しく作って戻り値とする
	return this->CreateModel(directoryPathObj, objFileName, directoryPathMtl, mtlFileName);

}

void AtrumEngine::DrawModel(AssetModel* model, const Transform& transform, const bool isLighting) {

	model->Draw(transform, viewMatrix_, directionalLightBuffer_->GetGpuVirtualAddress(), commandContextDirect_->GetCommandList(), srvAllocator_.get(), kPerspectiveFovMatrix, isLighting);

}

LeakChecker::~LeakChecker() {

	OutputDebugStringA("\nleakCheck\n\n");

	Microsoft::WRL::ComPtr<IDXGIDebug1> debug;

	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {

		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);

	}


}