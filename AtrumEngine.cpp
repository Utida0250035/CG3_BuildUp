#include "AtrumEngine.h"
#include "ConvertString.h"
#include "Log.h"
#include "Vector4.h"
#include "WindowProcedure.h"
#include <cassert>
#include <cstdint>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <format>
#include <string>
#include <strsafe.h>
#include <Windows.h>
#include "DeltaTime.h"
#include "Matrix3D.h"
#include "ImGui.h"

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

	// ウィンドウプロシージャ
	wc_.lpfnWndProc = WindowProc;

	// ウィンドウクラス名
	wc_.lpszClassName = L"CG2WindowClass";

	// インスタンスハンドル
	wc_.hInstance = GetModuleHandle(nullptr);

	// カーソル
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録
	RegisterClass(&wc_);

	// ウィンドウの横の大きさ
	clientWidth_ = clientWidth;

	// ウィンドウの縦の大きさ
	clientHeight_ = clientHeight;

	// ウィンドウサイズ構造体
	wrc_ = { 0, 0, clientWidth_, clientHeight_ };

	// クライアント領域を基に実際のサイズ情報をwrcに反映させる
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	hwnd_ = CreateWindow(
		wc_.lpszClassName,
		StringToWString(windowLabel).c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc_.right - wrc_.left,
		wrc_.bottom - wrc_.top,
		nullptr,
		nullptr,
		wc_.hInstance,
		nullptr
	);

	// ウィンドウの表示
	ShowWindow(hwnd_, SW_SHOW);

}

void AtrumEngine::SelectAdapter() {

	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
		// パフォーマンスが良い順にアダプタのリストを出させる 

		// アダプターの情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter_->GetDesc3(&adapterDesc);

		// アダプターの情報が取得できない場合はエラー
		assert(SUCCEEDED(hr_));

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// ソフトウェアアダプタでなければ採用

			// 採用したアダプタの情報をログに出力
			Log(WStringToString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));

			break;

		}

		// 次のアダプタへ
		useAdapter_ = nullptr;

	}

	// 適切なアダプターが見当たらない場合は起動不可
	assert(useAdapter_ != nullptr);

}

void AtrumEngine::CreateDevice() {

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 機能レベルが高い順に、生成できるか試していく

		hr_ = D3D12CreateDevice(useAdapter_, featureLevels[i], IID_PPV_ARGS(&device_));

		if (SUCCEEDED(hr_)) {
			// 指定した機能レベルでデバイスが生成できた

			// ログ出力

			Log(std::format("FeatureLevel: {}\n", featureLevelStrings[i]));

			break;

		}

	}

	// デバイスの生成が成功しなかった場合は実行不可
	assert(device_ != nullptr);

}

void AtrumEngine::ErrorSuppressionDebug() {

#ifdef _DEBUG

	ID3D12InfoQueue* infoQueue = nullptr;

	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

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

void AtrumEngine::InitDXC() {

	//dxcCompilerを初期化
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr_));
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr_));

	// includeに対応するための設定
	hr_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr_));

}

IDxcBlob* AtrumEngine::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile
) {

	// これからシェーダーをコンパイルする旨をログ出力
	LogFile::GetInstance()->Log(WStringToString(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile)));

	/*
	hlslファイルを読む
	*/
	IDxcBlobEncoding* shaderSource = nullptr;
	hr_ = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	// 読めなかったら止める
	assert(SUCCEEDED(hr_));

	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	// UTF8の文字コードであることを通知
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;


	/*
	コンパイルする
	*/
	LPCWSTR arguments[] = {

		// コンパイル対象のhlslファイル名
		filePath.c_str(),

		// エントリーポイントの指定
		L"-E", L"main",

		// ShaderProfileの設定
		L"-T", profile,

		// デバッグ用の情報を埋め込む
		L"-Zi", L"-Qembed_debug",

		// 最適化を外しておく
		L"-Od",

		// メモリレイアウトは行優先
		L"-Zpr"

	};

	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr_ = dxcCompiler_->Compile(
		// 読み込んだファイル
		&shaderSourceBuffer,
		// コンパイル設定
		arguments,
		// コンパイル設定の数
		_countof(arguments),
		// includeが含まれた諸々
		includeHandler_,
		// コンパイル結果
		IID_PPV_ARGS(&shaderResult)
	);

	// コンパイルエラーではないがDXCが起動できない等の致命的な情報を感知
	assert(SUCCEEDED(hr_));

	/*
	警告・エラーが出たらログ出力して止める
	*/
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		// 警告・エラーがある場合はログ出力して止める

		LogFile::GetInstance()->Log(shaderError->GetStringPointer());

		assert(false);

	}

	/*
	警告・エラーが無ければコンパイル結果を取得して返す
	*/

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr_ = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr_));

	// 成功した旨のログ出力
	LogFile::GetInstance()->Log(WStringToString(std::format(L"Compile Succeeded, path:{}, profile:{}", filePath, profile)));

	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();

	// 実行用のバイナリを返却
	return shaderBlob;

}

void AtrumEngine::MakeRootSignature() {

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成 [0]:PixelShaderのMaterial [1]:VertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[2] = {};

	// CBVを使う
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// PixelShaderで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// レジスタ番号0にバインド
	rootParameters[0].Descriptor.ShaderRegister = 0;

	// CBVを使う
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// VertexShaderで使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	// レジスタ番号0を使う
	rootParameters[1].Descriptor.ShaderRegister = 0;

	// ルートパラメータ配列へのポインタ
	descriptionRootSignature.pParameters = rootParameters;
	// 配列の長さ
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	// シリアライズしてバイナリにする
	hr_ = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);

	if (FAILED(hr_)) {


		LogFile::GetInstance()->Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));

		assert(false);

	}

	// バイナリを基に生成
	hr_ = device_->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr_));

}

void AtrumEngine::SetUpInputLayout() {
	// InputLayoutの設定

	inputElementDescriptions_[0].SemanticName = "POSITION";
	inputElementDescriptions_[0].SemanticIndex = 0;
	inputElementDescriptions_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescriptions_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescriptions_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescriptions_);

}

void AtrumEngine::SetUpBlendState() {
	// BlendStateの設定

	// 全ての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

}

void AtrumEngine::SetUpRasterizerState() {
	// RasterizerStateの設定

	// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;

	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

}

void AtrumEngine::PrepareShader() {
	// Shaderをコンパイルする

	vertexShaderBlob_ = this->CompileShader(L"Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(L"Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

}

ID3D12Resource* AtrumEngine::CreateBufferResource(size_t sizeInBytes) {

	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC resourceDesc{};

	// バッファリソース テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	resourceDesc.Width = sizeInBytes;

	// バッファの場合はこれらを1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;

	// バッファの場合はコレにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 実際にリソースを作る
	ID3D12Resource* resource = nullptr;
	hr_ = device_->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr_));

	return resource;

}

ID3D12DescriptorHeap* AtrumEngine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {

	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;

	if (shaderVisible) {

		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	} else {

		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	}

	hr_ = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));

	// ディスクリプタヒープが生成できなかったら起動不可
	assert(SUCCEEDED(hr_));

	return descriptorHeap;

}

void AtrumEngine::CreateMaterialResource() {

	// Color1つ分のサイズを用意
	materialResource_ = CreateBufferResource(sizeof(Vector4));

	// マテリアルにデータを書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	*materialData_ = { 1.0f, 1.0f, 1.0f, 1.0f };

}

void AtrumEngine::CreateWvpResource() {

	// Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = CreateBufferResource(sizeof(Matrix4x4));

	// データを書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));

	// 単位行列を書き込んでおく
	*wvpData_ = MakeIdentityMatrix4x4();

}


void AtrumEngine::CreatePSO() {

	graphicsPipeLineStateDesc_.pRootSignature = rootSignature_;

	graphicsPipeLineStateDesc_.InputLayout = inputLayoutDesc_;

	graphicsPipeLineStateDesc_.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };

	graphicsPipeLineStateDesc_.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };

	graphicsPipeLineStateDesc_.BlendState = blendDesc_;

	graphicsPipeLineStateDesc_.RasterizerState = rasterizerDesc_;

	// 書き込むRTVの情報
	graphicsPipeLineStateDesc_.NumRenderTargets = 1;
	graphicsPipeLineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するトポロジ(形状)のタイプ 三角形
	graphicsPipeLineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// どのように画面に色を打ち込むかの設定
	graphicsPipeLineStateDesc_.SampleDesc.Count = 1;
	graphicsPipeLineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 実際に生成
	hr_ = device_->CreateGraphicsPipelineState(&graphicsPipeLineStateDesc_, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr_));

}

void AtrumEngine::CreateVertexResource() {

	vertexResource_ = CreateBufferResource(sizeof(Vector4) * 3);

}

void AtrumEngine::CreateVertexBufferView() {

	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(Vector4) * 3;

	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(Vector4);

}

void AtrumEngine::WriteVertexResource() {
	// 頂点リソースにデータを書き込む

	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// 左下
	vertexData_[0] = { -0.5f, -0.5f, 0.0f, 1.0f };

	// 上
	vertexData_[1] = { 0.0f, 0.5f, 0.0f, 1.0f };

	// 右下
	vertexData_[2] = { 0.5f, -0.5f, 0.0f, 1.0f };

}

void AtrumEngine::SetMaterialData(const Vector4& color) {

	*materialData_ = color;

}

void AtrumEngine::SetWvpData(const Matrix4x4& wvp) {

	*wvpData_ = wvp;

}

void AtrumEngine::SetUpViewport() {
	// クライアント領域のサイズと同等にして画面全体を表示領域とする

	viewport_.Width = static_cast<FLOAT>(clientWidth_);
	viewport_.Height = static_cast<float>(clientHeight_);
	viewport_.TopLeftX = 0.0f;
	viewport_.TopLeftY = 0.0f;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

}

void AtrumEngine::SetUpScissorRect() {

	scissorRect_.left = 0;
	scissorRect_.right = clientWidth_;
	scissorRect_.top = 0;
	scissorRect_.bottom = clientHeight_;

}

void AtrumEngine::DrawCall() {

	// Viewportを設定
	commandList_->RSSetViewports(1, &viewport_);

	// ScissorRectを設定
	commandList_->RSSetScissorRects(1, &scissorRect_);

	// RootSignatureを設定 PSOに設定しているが別途の設定が必要
	commandList_->SetGraphicsRootSignature(rootSignature_);

	// PSOを設定
	commandList_->SetPipelineState(graphicsPipelineState_);

	// VBVを設定
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);

	// 形状を設定 PSOに設定しているものとは別で同じものを設定すると考える
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// WVP用のCBufferの場所を設定
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	// 描画(DrawCall) 3頂点で1つのインスタンス
	commandList_->DrawInstanced(3, 1, 0, 0);

}


void AtrumEngine::SetFps(const int32_t& fps) {

	secondsPerFrame_ = 1.0f / static_cast<float>(fps);

}

void AtrumEngine::Initialize(const std::string& windowLabel, const int32_t& clientWidth, const int32_t& clientHeight) {

	// SEH例外が補足されなかった場合(Unhandled)に補足する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

	this->PrepareWindow(windowLabel, clientWidth, clientHeight);

#ifdef _DEBUG

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController_)))) {

		// デバッグレイヤーを有効化
		debugController_->EnableDebugLayer();

		// GPU側でもチェックを行なうようにする
		debugController_->SetEnableGPUBasedValidation(TRUE);

	}

#endif

	hr_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	/*
	初期化の根本的な段階でエラーが出た場合は
	プログラムの間違いか修正不可である場合が多い
	*/
	assert(SUCCEEDED(hr_));

	this->SelectAdapter();

	this->CreateDevice();

	// 初期化完了のログを出す
	LogFile::GetInstance()->Log("Complete create D3D12Device!!!\n");


	// コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));

	// コマンドキューの生成がうまくいかなかったら起動できない
	assert(SUCCEEDED(hr_));


	// コマンドアロケータの生成
	hr_ = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));

	// コマンドアロケータの生成がうまくいかなかったら起動不可
	assert(SUCCEEDED(hr_));


	// コマンドリストの生成
	hr_ = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_, nullptr, IID_PPV_ARGS(&commandList_));

	// コマンドリストの生成がうまくいかなかったら起動不可
	assert(SUCCEEDED(hr_));


	// スワップチェーンに渡す情報
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = clientWidth_;
	swapChainDesc.Height = clientHeight_;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// コマンドキュー、ウィンドウハンドル、設定を渡してスワップチェーンを生成
	hr_ = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_, hwnd_, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain_));
	assert(SUCCEEDED(hr_));


	// RTVディスクリプタヒープの生成
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	// SRVディスクリプタヒープの生成
	srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// SwapChainからResourceを引っ張る
	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));

	// うまくResourceを取得できなければ起動不可
	assert(SUCCEEDED(hr_));
	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(hr_));


	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// ディスクリプタの先頭を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();


	// 1つめのRTV作成
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[0], &rtvDesc, rtvHandles_[0]);

	// 2つめのディスクリプトハンドルを作る
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 2つめのRTVを作る
	device_->CreateRenderTargetView(swapChainResources_[1], &rtvDesc, rtvHandles_[1]);


	// 初期値0でFenceを作成
	hr_ = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));

	// fenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);

	this->InitDXC();

	this->MakeRootSignature();

	this->SetUpInputLayout();

	this->SetUpBlendState();

	this->SetUpRasterizerState();

	this->PrepareShader();

	this->SetUpViewport();

	this->SetUpScissorRect();

	this->CreateMaterialResource();

	this->CreateWvpResource();

#ifdef USE_IMGUI

	// ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd_);
	ImGui_ImplDX12_Init(
		device_,
		swapChainDesc.BufferCount,
		rtvDesc.Format,
		srvDescriptorHeap_,
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
	);
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Build();

#endif

	// 初期化完了のログ出力
	LogFile::GetInstance()->Log("Hello World!");

}

bool AtrumEngine::IsProcess() {

	if (msg_.message == WM_QUIT) {

		return false;

	}

	return true;

}

bool AtrumEngine::MessageForOs() {

	if (PeekMessage(&msg_, NULL, 0, 0, PM_REMOVE)) {
		// OSへのメッセージを最優先で処理

		TranslateMessage(&msg_);
		DispatchMessage(&msg_);

		return true;

	}

	return false;

}

bool AtrumEngine::IsWaitForFrame() {

	DeltaTime::GetInstance()->CalcDeltaTime();

	countForNextFrame_ += DeltaTime::GetInstance()->GetDeltaTime();

	if (countForNextFrame_ >= secondsPerFrame_) {

		countForNextFrame_ -= secondsPerFrame_;

		return false;

	}

	return true;

}

bool AtrumEngine::IsExecuteFrame() {

	if (this->MessageForOs()) {

		return false;

	}

	if (this->IsWaitForFrame()) {

		return false;

	}

	return true;

}

void AtrumEngine::UpdateWindow() {

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();


	// デバッグ用
	assert(swapChainResources_[backBufferIndex] != nullptr);
	assert(rtvHandles_[backBufferIndex].ptr != 0);


	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};

	// 今回のバリアの型はTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	// フラグをNoneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// バリアを張る対象のリソース(現在のバックバッファに対して行なう)
	barrier.Transition.pResource = swapChainResources_[backBufferIndex];

	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	// 描画先のRTVを設定
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, nullptr);

	// 指定色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);


	this->DrawCall();


	// 画面に描く処理が終了し画面に映すため状態を遷移
	// RenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	// コマンドリストの内容を確定させる
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));

	// GPUにコマンドリストを実行させる
	ID3D12CommandList* commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// fenceの値を更新
	fenceValue_++;

	// GPUがここまでたどり着いたときにFenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_, fenceValue_);

	if (fence_->GetCompletedValue() < fenceValue_) {

		// 指定したsignalにたどり着くまでイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);

		// イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	// GPUとOSに画面の交換を行なうよう通知する
	swapChain_->Present(1, 0);

	// 次のフレーム用のコマンドリストを準備
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandList_->Reset(commandAllocator_, nullptr);
	assert(SUCCEEDED(hr_));

}

void AtrumEngine::Finalize() {

	fenceValue_++;
	commandQueue_->Signal(fence_, fenceValue_);

	if (fence_->GetCompletedValue() < fenceValue_) {
		// GPUの完了を待つ

		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	/* 解放処理 */

	wvpResource_->Release();
	materialResource_->Release();
	vertexResource_->Release();
	graphicsPipelineState_->Release();
	signatureBlob_->Release();

	if (errorBlob_) {

		errorBlob_->Release();

	}

	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();

	CloseHandle(fenceEvent_);
	fence_->Release();
	rtvDescriptorHeap_->Release();
	swapChainResources_[0]->Release();
	swapChainResources_[1]->Release();
	swapChain_->Release();
	commandList_->Release();
	commandAllocator_->Release();
	commandQueue_->Release();
	device_->Release();
	useAdapter_->Release();
	dxgiFactory_->Release();

#ifdef _DEBUG

	debugController_->Release();

#endif

#ifdef USE_IMGUI

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif

	CloseWindow(hwnd_);

	// リソースリークチェック
	IDXGIDebug1* debug;

	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {

		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();

	}

}


Matrix4x4 AtrumEngine::CreateWorldMatrix(const Transform& transform) {

	Matrix4x4 result = MakeWorldMatrix(transform.translate, transform.scale, transform.rotate);

	return result;

}