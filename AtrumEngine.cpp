#include "AtrumEngine.h"
#include "StaticCast.h"
#include "CommandContext.h"
#include "RenderDevice.h"
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
#include "Matrix3D.h"

#ifdef USE_IMGUI

#include "ImGui.h"

#include <d3d12sdklayers.h>

#endif

#include <DirectXTex/d3dx12.h>
#include <vector>

#include <memory>
#include <filesystem>
#include <cfloat>
#include <numbers>

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
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME,
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

void AtrumEngine::InitDXC() {

	assert(!isInitialized_ && "InitDXC() is initializeHelper");

	//dxcCompilerを初期化
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr_));
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr_));

	// includeに対応するための設定
	hr_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr_));

	LogFile::GetInstance()->Log("Initialized DXC");

}

IDxcBlob* AtrumEngine::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile
) {

	assert(!isInitialized_ && "CompileShader() is initializeHelper");

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
		includeHandler_.Get(),
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

		shaderError->Release();

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

	LogFile::GetInstance()->Log("shader Compiled");

	// 実行用のバイナリを返却
	return shaderBlob;

}

void AtrumEngine::MakeRootSignature() {

	assert(!isInitialized_ && "MakeRootSignature() is initializeHelper");

	// RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成 [0]:PixelShaderのMaterial [1]:VertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[4] = {};

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

	// CBVを使う
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	// PixelShaderで使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// レジスタ番号1を使う
	rootParameters[3].Descriptor.ShaderRegister = 1;
	

	// ルートパラメータ配列へのポインタ
	descriptionRootSignature.pParameters = rootParameters;
	// 配列の長さ
	descriptionRootSignature.NumParameters = _countof(rootParameters);


	// DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	// 0から始まる
	descriptorRange[0].BaseShaderRegister = 0;
	// 数は1つ
	descriptorRange[0].NumDescriptors = 1;
	// SRVを使う
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// Offsetを自動計算
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// DescriptorTableを使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// PixelShaderで使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	// Tableで利用する数
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	// バイリニアフィルタ
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	// 0~1の範囲外をリピート
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 比較しない
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	// ありったけのMipMapを使う
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	// レジスタ番号0を使う
	staticSamplers[0].ShaderRegister = 0;
	// PixelShaderで使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	// シリアライズしてバイナリにする
	hr_ = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);

	if (FAILED(hr_)) {


		LogFile::GetInstance()->Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));

		assert(false);

	}

	// バイナリを基に生成
	hr_ = renderDevice_->GetDevice()->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr_));

	LogFile::GetInstance()->Log("Created RootSignature");

}

void AtrumEngine::SetUpInputLayout() {

	assert(!isInitialized_ && "SetUpInputLayout() is initializeHelper");

	inputElementDescriptions_[0].SemanticName = "POSITION";
	inputElementDescriptions_[0].SemanticIndex = 0;
	inputElementDescriptions_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescriptions_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescriptions_[1].SemanticName = "TEXCOORD";
	inputElementDescriptions_[1].SemanticIndex = 0;
	inputElementDescriptions_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescriptions_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescriptions_[2].SemanticName = "NORMAL";
	inputElementDescriptions_[2].SemanticIndex = 0;
	inputElementDescriptions_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescriptions_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescriptions_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescriptions_);

	LogFile::GetInstance()->Log("Finished SetUp InputLayout");

}

void AtrumEngine::SetUpBlendState() {

	assert(!isInitialized_ && "SetUpBlendState() is initializeHelper");

	// BlendStateの設定

	// 全ての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

}

void AtrumEngine::SetUpRasterizerState() {

	assert(!isInitialized_ && "SetUpRasterizerState() is initializeHelper");

	// RasterizerStateの設定

	// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;

	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	LogFile::GetInstance()->Log("Finished SetUp RasterizerState");

}

void AtrumEngine::SetUpDepthStencilState() {

	assert(!isInitialized_ && "SetUpDepthStencilState() is initializeHelper");

	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;

	// 書き込みする
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	// 比較関数をLessEqualとする(近ければ描画される)
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

}

void AtrumEngine::PrepareShader() {

	assert(!isInitialized_ && "PrepareShader() is initializeHelper");

	// Shaderをコンパイルする

	vertexShaderBlob_ = this->CompileShader(L"Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(L"Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

	LogFile::GetInstance()->Log("Shader Prepared");

}

ComPtr<ID3D12Resource> AtrumEngine::CreateBufferResource(size_t sizeInBytes) {

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
	ComPtr<ID3D12Resource> resource = nullptr;
	hr_ = renderDevice_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr_));

	LogFile::GetInstance()->Log("Created BufferResource");

	return resource;

}

void AtrumEngine::CreateMaterialResource() {

	assert(!isInitialized_ && "CreateMaterialResource() is initializeHelper");

	// Color * maxCount分サイズを用意
	materialResource_ = this->CreateBufferResource(sizeof(MaterialData) * kTriangleMaxDrawCount);

	// マテリアルにデータを書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	LogFile::GetInstance()->Log("Created MaterialResource");

}

void AtrumEngine::CreateTransformationResource() {

	assert(!isInitialized_ && "CreateWvpResource() is initializeHelper");

	// Matrix4x4 maxCount個分のサイズを用意する
	transformationResource_ = this->CreateBufferResource(sizeof(TransformationMatrix) * kTriangleMaxDrawCount);

	// データを書き込むためのアドレスを取得
	transformationResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationData_));

	LogFile::GetInstance()->Log("Created WvpResource");

}


void AtrumEngine::CreatePSO() {

	assert(!isInitialized_ && "CreatePSO() is initializeHelper");

	// ルートシグネチャを設定
	graphicsPipelineStateDesc_.pRootSignature = rootSignature_.Get();

	// InputLayout
	graphicsPipelineStateDesc_.InputLayout = inputLayoutDesc_;

	// VertexShader
	graphicsPipelineStateDesc_.VS = { vertexShaderBlob_->GetBufferPointer(), vertexShaderBlob_->GetBufferSize() };

	// PixelShader
	graphicsPipelineStateDesc_.PS = { pixelShaderBlob_->GetBufferPointer(), pixelShaderBlob_->GetBufferSize() };

	// Blendの設定
	graphicsPipelineStateDesc_.BlendState = blendDesc_;

	// Rasterizerの設定
	graphicsPipelineStateDesc_.RasterizerState = rasterizerDesc_;

	// DepthStencilの設定
	graphicsPipelineStateDesc_.DepthStencilState = depthStencilDesc_;
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicsPipelineStateDesc_.NumRenderTargets = 1;
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するトポロジ(形状)のタイプ 三角形
	graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 実際に生成
	hr_ = renderDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr_));

	LogFile::GetInstance()->Log("Created PSO");

}

void AtrumEngine::CreateVertexResource() {

	assert(!isInitialized_ && "CreateVertexResource() is initializeHelper");

	// 三角形最大数 * 3 * データ1つ分のサイズ
	vertexResource_ = this->CreateBufferResource(sizeof(VertexData) * 3 * kTriangleMaxDrawCount);

	// データを書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	LogFile::GetInstance()->Log("Created VertexResource");

}

void AtrumEngine::CreateVertexBufferView() {

	assert(!isInitialized_ && "CreateVertexBufferView() is initializeHelper");

	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	// 使用するリソースのサイズは 頂点3つ分 * triangleMaxCount のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3 * kTriangleMaxDrawCount;

	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	LogFile::GetInstance()->Log("Created VertexBufferView");

}

void AtrumEngine::CreateDirectionalLightResource() {

	assert(!isInitialized_ && "CreateDirectionalLightResource() is initializeHelper");

	// Data1つ * triangleMaxCount のサイズを用意
	directionalLightResource_ = this->CreateBufferResource(sizeof(DirectionalLightData) * kTriangleMaxDrawCount);

	// データを書き込むためのアドレスを取得
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

	// デフォルト値
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData_->intensity = 1.0f;

}

void AtrumEngine::CreateSpriteVertexResource() {

	assert(!isInitialized_ && "CreateSpriteVertexResource() is initializeHelper");

	spriteVertexResource_ = this->CreateBufferResource(sizeof(VertexData) * 3 * kSpriteTriangleMaxDrawCount);

	spriteVertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&spriteVertexData_));

}

void AtrumEngine::CreateSpriteVertexBufferView() {

	assert(!isInitialized_ && "CreateSpriteVertexBufferView() is initializeHelper");

	// リソースの先頭のアドレスから使う
	spriteVertexBufferView_.BufferLocation = spriteVertexResource_->GetGPUVirtualAddress();

	// 使用するリソースのサイズ
	spriteVertexBufferView_.SizeInBytes = sizeof(VertexData) * 3 * kSpriteTriangleMaxDrawCount;

	// 1頂点当たりのサイズ
	spriteVertexBufferView_.StrideInBytes = sizeof(VertexData);

}

void AtrumEngine::CreateSpriteMaterialResource() {

	assert(!isInitialized_ && "CreateSpriteMaterialResource() is initializeHelper");

	// Color maxCount個分のサイズを用意
	spriteMaterialResource_ = this->CreateBufferResource(sizeof(MaterialData) * kSpriteTriangleMaxDrawCount);

	// マテリアルにデータを書き込むためのアドレスを取得
	spriteMaterialResource_->Map(0, nullptr, reinterpret_cast<void**>(&spriteMaterialData_));

	spriteMaterialData_->enableLighting = false;

	LogFile::GetInstance()->Log("Created MaterialResource");

}

void AtrumEngine::CreateSpriteTransformationResource() {

	assert(!isInitialized_ && "CreateSpriteTransformationResource() is initializeHelper");

	// 4x4行列 maxCount個分のサイズを用意する
	spriteTransformationMatrixResource_ = this->CreateBufferResource(sizeof(TransformationMatrix) * kSpriteTriangleMaxDrawCount);

	// データを書き込むためのアドレス取得
	spriteTransformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&spriteTransformData_));

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

	assert(triangleDrewCount_ + 1 < kTriangleMaxDrawCount && "triangleCount over maxCount(Triangle)");
	assert(constantBufferCount_ + 1 < kTriangleMaxDrawCount && "constantBufferCount over maxCount(Triangle)");

	// --- TransformMatrix (WVP) のアドレス計算 ---
	D3D12_GPU_VIRTUAL_ADDRESS transformBaseAddr = transformationResource_->GetGPUVirtualAddress();

	// offset = インデックス × 256バイト
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformBaseAddr + (constantBufferCount_ * sizeof(TransformationMatrix));

	// GPUに設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// --- Material (Color) のアドレス計算 ---
	// ※こちらも定数バッファなら同様に256バイトずつずらす必要があります
	D3D12_GPU_VIRTUAL_ADDRESS materialBaseAddr = materialResource_->GetGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBaseAddr + (constantBufferCount_ * sizeof(MaterialData));

	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	DescriptorAllocator::DescriptorHandle textureHandle{};

	textureHandle = srvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
	commandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 3頂点で1つのインスタンス
	commandContextDirect_->GetCommandList()->DrawInstanced(3, 1, triangleDrewCount_ * 3, 0);

	triangleDrewCount_++;

	constantBufferCount_++;

}

void AtrumEngine::DrawSphereCall(const uint32_t& textureIndex, const uint32_t& triangleCountInSphere) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	assert(triangleDrewCount_ + triangleCountInSphere < kTriangleMaxDrawCount && "triangleCount over maxCount(Sphere)");
	assert(constantBufferCount_ + 1 < kTriangleMaxDrawCount && "constantBufferCount over maxCount(Sphere)");

	// --- TransformMatrix (WVP) のアドレス計算 ---
	D3D12_GPU_VIRTUAL_ADDRESS transformBaseAddr = transformationResource_->GetGPUVirtualAddress();

	// offset = インデックス × 256バイト
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformBaseAddr + (constantBufferCount_ * sizeof(TransformationMatrix));

	// GPUに設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// --- Material (Color) のアドレス計算 ---
	// ※こちらも定数バッファなら同様に256バイトずつずらす必要があります
	D3D12_GPU_VIRTUAL_ADDRESS materialBaseAddr = materialResource_->GetGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBaseAddr + (constantBufferCount_ * sizeof(MaterialData));

	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	DescriptorAllocator::DescriptorHandle textureHandle{};

	textureHandle = srvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
	commandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 3頂点で1つのインスタンス
	commandContextDirect_->GetCommandList()->DrawInstanced(triangleCountInSphere * 3, 1, triangleDrewCount_ * 3, 0);

	triangleDrewCount_ += triangleCountInSphere;

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

	swapChainManager_->Initialize(clientWidth_, clientHeight_, renderDevice_->GetDevice(), renderDevice_->GetDxgiFactory(), commandContextDirect_->GetCommandQueue(), hwnd_, rtvAllocator_, rtvDesc);


	// SRVディスクリプタヒープの生成
	srvAllocator_ = std::make_unique<DescriptorAllocator>();
	srvAllocator_->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true, L"srvDescriptors", renderDevice_->GetDevice());

	// fenceの管理インスタンスを生成
	fenceManager_ = std::make_unique<Fence>();
	fenceManager_->Initialize(renderDevice_->GetDevice(), SwapChain::kBackBufferCount);

	// DXCの初期化
	this->InitDXC();

	// ルートシグネチャの作成
	this->MakeRootSignature();

	// InputLayoutの設定
	this->SetUpInputLayout();

	// BlendStateの設定
	this->SetUpBlendState();

	// RasterizerStateの設定
	this->SetUpRasterizerState();

	// DepthStencilStateの設定
	this->SetUpDepthStencilState();

	// シェーダーの用意
	this->PrepareShader();

	// ビューポートの設定
	this->SetUpViewport();

	// シザー矩形の設定
	this->SetUpScissorRect();

	// MaterialResourceの生成
	this->CreateMaterialResource();

	// TransformationResourceの生成
	this->CreateTransformationResource();

	// PSOの生成
	this->CreatePSO();

	// VertexResourceの生成
	this->CreateVertexResource();

	// VertexBufferViewの生成
	this->CreateVertexBufferView();

	// 平行光源リソースの生成
	this->CreateDirectionalLightResource();

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

	// Sprite用 MaterialResourceの作成
	this->CreateSpriteMaterialResource();

	// Sprite用 TransformationMatrixResourceの生成
	this->CreateSpriteTransformationResource();

	// Sprite用 VertexResourceの生成
	this->CreateSpriteVertexResource();

	// Sprite用 VertexBufferViewの生成
	this->CreateSpriteVertexBufferView();


	/* ImGui */

#ifdef USE_IMGUI

	// ImGuiの初期化

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd_);

	DescriptorAllocator::DescriptorHandle imguiSrvHandle{};

	imguiSrvHandle = srvAllocator_->Allocate();

	ImGui_ImplDX12_Init(
		renderDevice_->GetDevice().Get(),
		SwapChain::kBackBufferCount,
		rtvDesc.Format,
		srvAllocator_->GetDescriptorHeap().Get(),
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

	// 初期化完了のログ出力
	LogFile::GetInstance()->Log("Hello World!");

	isInitialized_ = true;

}

bool AtrumEngine::IsProcess() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	if (msg_.message == WM_QUIT) {

		return false;

	}

	return true;

}

bool AtrumEngine::MessageForOs() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	if (PeekMessage(&msg_, NULL, 0, 0, PM_REMOVE)) {
		// OSへのメッセージを最優先で処理

		TranslateMessage(&msg_);
		DispatchMessage(&msg_);

		return true;

	}

	return false;

}

bool AtrumEngine::IsWaitForFrame() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	deltaTimeManager_->CalcDeltaTime();

	countForNextFrame_ += deltaTimeManager_->GetDeltaTime();

	if (countForNextFrame_ >= secondsPerFrame_) {

		countForNextFrame_ -= secondsPerFrame_;

		return false;

	}

	return true;

}

bool AtrumEngine::IsFrameExecute() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	if (this->MessageForOs()) {

		return false;

	}

	if (this->IsWaitForFrame()) {

		return false;

	}

	return true;

}

#ifdef USE_IMGUI

void AtrumEngine::ImGuiNewFrame() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

}

void AtrumEngine::ImGuiRender() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	ImGui::Render();

}

#endif

void AtrumEngine::PreDraw() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	swapChainManager_->UpdateBackBufferIndex();

	// 今回のバリアの型はTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;

	// フラグをNoneにしておく
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

	// バリアを張る対象のリソース(現在のバックバッファに対して行なう)
	barrier_.Transition.pResource = swapChainManager_->GetSwapChainResourceCurrent().Get();

	// 遷移前(現在)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

	// 遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	// TransitionBarrierを張る
	commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier_);

	// 描画先のRTVとDSVを設定
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvAllocator_->GetCpuStart();
	commandContextDirect_->GetCommandList()->OMSetRenderTargets(1, swapChainManager_->PGetRtvHandleCurrent(), false, &dsvHandle);

	// 指定した深度(1.0f)で画面全体をクリアする
	commandContextDirect_->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 指定色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandContextDirect_->GetCommandList()->ClearRenderTargetView(swapChainManager_->GetRtvHandleCurrent(), clearColor, 0, nullptr);

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvAllocator_->GetDescriptorHeap().Get() };
	commandContextDirect_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	// Viewportを設定
	commandContextDirect_->GetCommandList()->RSSetViewports(1, &viewport_);

	// ScissorRectを設定
	commandContextDirect_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);

	// RootSignatureを設定 PSOに設定しているが別途の設定が必要
	commandContextDirect_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());

	// PSOを設定
	commandContextDirect_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());

	// VBVを設定
	commandContextDirect_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	// 形状を設定 PSOに設定しているものとは別で同じものを設定すると考える
	commandContextDirect_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// マテリアルCBufferの場所を設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	// WVP用のCBufferの場所を設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());


}

void AtrumEngine::PostDraw() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

#ifdef USE_IMGUI

	// ImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandContextDirect_->GetCommandList().Get());

#endif

	// 画面に描く処理が終了し画面に映すため状態を遷移
	// RenderTargetからPresentにする
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	// TransitionBarrierを張る
	commandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier_);

	// コマンドリストの内容を確定させる
	hr_ = commandContextDirect_->GetCommandList()->Close();
	assert(SUCCEEDED(hr_));

	// GPUにコマンドリストを実行させる
	ID3D12CommandList* commandLists[] = { commandContextDirect_->GetCommandList().Get() };
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

	hr_ = commandContextDirect_->GetCommandAllocators()[swapChainManager_->GetBackBufferIndex()].Get()->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandContextDirect_->GetCommandList().Get()->Reset(commandContextDirect_->GetCommandAllocators()[swapChainManager_->GetBackBufferIndex()].Get(), nullptr);
	assert(SUCCEEDED(hr_));

	// 三角形のカウントをリセット
	triangleDrewCount_ = 0;
	spriteTriangleDrewCount_ = 0;

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
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif

	// fenceManager_.reset();

	/* */

	// COMの終了処理
	CoUninitialize();

	CloseWindow(hwnd_);

}


Matrix4x4 AtrumEngine::CreateWorldMatrix(const Transform& transform) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	Matrix4x4 result = MakeWorldMatrix(transform.translate, transform.scale, transform.rotate);

	return result;

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

ComPtr<ID3D12Resource> AtrumEngine::CreateIntermediateResource(const ComPtr<ID3D12Resource>& textureResource) {

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

void AtrumEngine::UploadTextureData(const ComPtr<ID3D12Resource>& textureResource, const DirectX::ScratchImage& mipImages, const ComPtr<ID3D12Resource>& intermediateResource) {

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
		destinationLocation.pResource = textureResource.Get();
		destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destinationLocation.SubresourceIndex = i;

		D3D12_TEXTURE_COPY_LOCATION sourceLocation{};
		sourceLocation.pResource = intermediateResource.Get();
		sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		sourceLocation.PlacedFootprint = layouts[i];

		commandContextDirect_->GetCommandList()->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);

	}

	/* バリアを張って利用可能な状態にする */
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textureResource.Get();
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

	auto search = textureIndexTable_.find(filePath);

	if (search != textureIndexTable_.end()) {

		return search->second;

	}

	Texture texture;

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = this->LoadTexture(filePath);
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();

	texture.resource = this->CreateTextureResource(metaData);

	assert(texture.resource);

	ComPtr<ID3D12Resource> intermediateResource = this->CreateIntermediateResource(texture.resource);

	// 中間リソースを用いた転送
	this->UploadTextureData(texture.resource, mipImages, intermediateResource);

	// コマンドリストの内容を確定させる
	hr_ = commandContextDirect_->GetCommandList()->Close();
	assert(SUCCEEDED(hr_));

	// GPUにコマンドリストを実行させる
	ID3D12CommandList* commandLists[] = { commandContextDirect_->GetCommandList().Get() };
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
	hr_ = commandContextDirect_->GetCommandAllocators()[swapChainManager_->GetBackBufferIndex()]->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandContextDirect_->GetCommandList()->Reset(commandContextDirect_->GetCommandAllocators()[swapChainManager_->GetBackBufferIndex()].Get(), nullptr);
	assert(SUCCEEDED(hr_));

	// 実際にShaderResourceViewを作る
	MakeShaderResourceView(texture, metaData);

	// 中間リソースを一時保存
	temporaryResources_.emplace_back(intermediateResource);

	// ファイル名と番号を格納
	textureIndexTable_.emplace(filePath, texture.srvIndex);

	// 配列に所有権を移動
	textures_.emplace_back(texture);

	LogFile::GetInstance()->Log("Got Texture: " + filePath);

	// 作成したテクスチャの番号を返す
	return textures_.back().srvIndex;

}

void AtrumEngine::DrawTriangle(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& triangleTransform, const Transform& cameraTransform, const std::array<VertexData, 3>& vertexData, const std::optional<DirectionalLightData>& directionalLightData) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// カメラのワールド行列
	Matrix4x4 cameraWorldMatrix = this->CreateWorldMatrix(cameraTransform);

	// ビュー行列
	Matrix4x4 viewMatrix = MatrixInverse(cameraWorldMatrix);

	// 透視投影行列
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.5f, 1.77777f, 0.125f, 128.0f);

	// 三角形のTransform
	Matrix4x4 triangleWorldMatrix = this->CreateWorldMatrix(triangleTransform);

	// CPU上のマッピング済みアドレスにオフセットを加えて書き込み
	transformationData_[constantBufferCount_].wvp = triangleWorldMatrix * viewMatrix * projectionMatrix;
	transformationData_[constantBufferCount_].world = triangleWorldMatrix;

	materialData_[constantBufferCount_].data = textureColor;

	if (directionalLightData.has_value()) {

		directionalLightData_[constantBufferCount_] = directionalLightData.value();
		materialData_[constantBufferCount_].enableLighting = true;

	} else {

		materialData_[constantBufferCount_].enableLighting = false;

	}

	// 左下
	vertexData_[triangleDrewCount_ * 3] = vertexData[0];

	// 上
	vertexData_[triangleDrewCount_ * 3 + 1] = vertexData[1];

	// 右下
	vertexData_[triangleDrewCount_ * 3 + 2] = vertexData[2];

	// 描画
	this->DrawTriangleCall(textureIndex);

}

void AtrumEngine::DrawSphere(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& sphereTransform, const Transform& cameraTransform, const float radius, const uint32_t subdivision, const std::optional<DirectionalLightData>& directionalLightData) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// カメラのワールド行列
	Matrix4x4 cameraWorldMatrix = this->CreateWorldMatrix(cameraTransform);

	// ビュー行列
	Matrix4x4 viewMatrix = MatrixInverse(cameraWorldMatrix);

	// 透視投影行列
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.5f, 1.77777f, 0.125f, 128.0f);

	// 球のTransform
	Matrix4x4 sphereWorldMatrix = this->CreateWorldMatrix(sphereTransform);

	transformationData_[constantBufferCount_].wvp = sphereWorldMatrix * viewMatrix * projectionMatrix;
	transformationData_[constantBufferCount_].world = sphereWorldMatrix;

	materialData_[constantBufferCount_].data = textureColor;

	if (directionalLightData.has_value()) {

		directionalLightData_[constantBufferCount_] = directionalLightData.value();
		materialData_[constantBufferCount_].enableLighting = true;

	} else {

		materialData_[constantBufferCount_].enableLighting = false;

	}

	const float kLonEvery = std::numbers::pi_v<float> *2.0f / Float(subdivision);
	const float kLatEvery = std::numbers::pi_v<float> / Float(subdivision);

	VertexData pointA{}, pointB{}, pointC{}, pointD{};

	float lat = 0.0f;
	float lon = 0.0f;

	uint32_t vertexDataIndex = triangleDrewCount_ * 3;

	uint32_t triangleCountInSphere = 0;

	for (size_t latIndex = 0; latIndex < subdivision; ++latIndex) {

		lat = -(std::numbers::pi_v<float> *0.5f) + kLatEvery * Float(latIndex);

		for (size_t lonIndex = 0; lonIndex < subdivision; ++lonIndex) {

			lon = static_cast<float>(lonIndex) * kLonEvery;

			pointA.position = Vector4{ cos(lat) * cos(lon), sin(lat), cos(lat) * sin(lon), 0.0f } * radius;
			pointA.position.w = 1.0f;
			pointA.texCoord = Vector2{ Float(lonIndex) / Float(subdivision), 1.0f - Float(latIndex) / Float(subdivision) };
			pointA.normal = VectorNormalize(Vector3{ pointA.position.x, pointA.position.y, pointA.position.z });

			pointB.position = Vector4{ cos(lat + kLatEvery) * cos(lon), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon), 0.0f } * radius;
			pointB.position.w = 1.0f;
			pointB.texCoord = Vector2{ Float(lonIndex) / Float(subdivision), 1.0f - Float(latIndex + 1) / Float(subdivision) };
			pointB.normal = VectorNormalize(Vector3{ pointB.position.x, pointB.position.y, pointB.position.z });

			pointC.position = Vector4{ cos(lat) * cos(lon + kLonEvery), sin(lat), cos(lat) * sin(lon + kLonEvery) , 0.0f } * radius;
			pointC.position.w = 1.0f;
			pointC.texCoord = Vector2{ Float(lonIndex + 1) / Float(subdivision), 1.0f - Float(latIndex) / Float(subdivision) };
			pointC.normal = VectorNormalize(Vector3{ pointC.position.x, pointC.position.y, pointC.position.z });

			pointD.position = Vector4{ cos(lat + kLatEvery) * cos(lon + kLonEvery), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery), 0.0f } * radius;
			pointD.position.w = 1.0f;
			pointD.texCoord = Vector2{ Float(lonIndex + 1) / Float(subdivision), 1.0f - Float(latIndex + 1) / Float(subdivision) };
			pointD.normal = VectorNormalize(Vector3{ pointD.position.x, pointD.position.y, pointD.position.z });

			vertexData_[vertexDataIndex++] = pointC;

			vertexData_[vertexDataIndex++] = pointA;

			vertexData_[vertexDataIndex++] = pointB;

			vertexData_[vertexDataIndex++] = pointB;

			vertexData_[vertexDataIndex++] = pointD;

			vertexData_[vertexDataIndex++] = pointC;

			triangleCountInSphere += 2;

		}

	}

	this->DrawSphereCall(textureIndex, triangleCountInSphere);

}


void AtrumEngine::PrepareSprite() {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	commandContextDirect_->GetCommandList()->IASetVertexBuffers(0, 1, &spriteVertexBufferView_);

	spriteMaterialData_->enableLighting = false;

}

void AtrumEngine::DrawSpriteRect(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& rectTransform, const Vector2& rectSize) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	// ビュー行列
	Matrix4x4 viewMatrix = MakeIdentityMatrix4x4();

	// 透視投影行列
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, Float(clientWidth_), Float(clientHeight_), 0.0f, 100.0f);

	// 三角形のTransform
	Matrix4x4 worldMatrix = this->CreateWorldMatrix(rectTransform);

	spriteTransformData_[spriteConstantBufferCount_].wvp = worldMatrix * viewMatrix * projectionMatrix;
	spriteTransformData_[spriteConstantBufferCount_].world = worldMatrix;

	spriteMaterialData_[spriteConstantBufferCount_].data = textureColor;

	/* 1枚目の三角形 */

	uint32_t vertexCount = spriteTriangleDrewCount_ * 3;

	Vector2 halfSize = rectSize * 0.5f;

	Vector3 normal = Vector3{ 0.0f, 0.0f, -1.0f };

	// 左下
	spriteVertexData_[vertexCount].texCoord = { 0.0f, 1.0f };
	spriteVertexData_[vertexCount].normal = normal;
	spriteVertexData_[vertexCount++].position = { -halfSize.x, halfSize.y, 0.0f, 1.0f };

	// 左上
	spriteVertexData_[vertexCount].texCoord = { 0.0f, 0.0f };
	spriteVertexData_[vertexCount].normal = normal;
	spriteVertexData_[vertexCount++].position = { -halfSize.x, -halfSize.y, 0.0f, 1.0f };

	// 右下
	spriteVertexData_[vertexCount].texCoord = { 1.0f, 1.0f };
	spriteVertexData_[vertexCount++].position = { halfSize.x, halfSize.y, 0.0f, 1.0f };

	/* 2枚目の三角形 */

	// 左上
	spriteVertexData_[vertexCount].texCoord = { 0.0f, 0.0f };
	spriteVertexData_[vertexCount].normal = normal;
	spriteVertexData_[vertexCount++].position = { -halfSize.x, -halfSize.y, 0.0f, 1.0f };

	// 右上
	spriteVertexData_[vertexCount].texCoord = { 1.0f, 0.0f };
	spriteVertexData_[vertexCount].normal = normal;
	spriteVertexData_[vertexCount++].position = { halfSize.x, -halfSize.y, 0.0f, 1.0f };

	// 右下
	spriteVertexData_[vertexCount].texCoord = { 1.0f, 1.0f };
	spriteVertexData_[vertexCount].normal = normal;
	spriteVertexData_[vertexCount++].position = { halfSize.x, halfSize.y, 0.0f, 1.0f };

	// 描画
	this->DrawSpriteCall(textureIndex);

}

void AtrumEngine::DrawSpriteLine(const uint32_t& textureIndex, const Vector4& textureColor, const Vector2& start, const Vector2& end, const float& width, const float& posZ) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	Vector2 difference = end - start;
	float length = VectorLength(difference);

	Vector2 rectPos = start + difference * 0.5f;

	Transform rectTransform{};
	rectTransform.translate = { rectPos.x, rectPos.y, posZ };
	rectTransform.scale = { 1.0f, 1.0f, 1.0f };
	rectTransform.rotate = { 0.0f, 0.0f, std::atan2(difference.y, difference.x) };

	DrawSpriteRect(textureIndex, textureColor, rectTransform, Vector2{ length, width });

}



void AtrumEngine::DrawSpriteCall(const uint32_t& textureIndex) {

	assert(isInitialized_ && "AtrumEngine is not initialized");

	assert(spriteTriangleDrewCount_ + 2 < kSpriteTriangleMaxDrawCount && "spriteTriangleCount over maxCount");
	assert(spriteConstantBufferCount_ + 1 < kSpriteTriangleMaxDrawCount && "spriteConstantBufferCount over maxCount");

	// --- TransformMatrix (WVP) のアドレス計算 ---
	D3D12_GPU_VIRTUAL_ADDRESS transformBaseAddr = spriteTransformationMatrixResource_->GetGPUVirtualAddress();

	// offset = インデックス × 256バイト
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformBaseAddr + (spriteConstantBufferCount_ * sizeof(TransformationMatrix));

	// GPUに設定
	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);


	D3D12_GPU_VIRTUAL_ADDRESS materialBaseAddr = spriteMaterialResource_->GetGPUVirtualAddress();
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBaseAddr + (spriteConstantBufferCount_ * sizeof(MaterialData));

	commandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	DescriptorAllocator::DescriptorHandle textureHandle{};
	textureHandle = srvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
	commandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	assert(spriteTriangleDrewCount_ < 1024);

	// 描画(DrawCall) 6頂点で1つのインスタンス
	commandContextDirect_->GetCommandList()->DrawInstanced(6, 1, spriteTriangleDrewCount_ * 3, 0);

	spriteTriangleDrewCount_ += 2;

	spriteConstantBufferCount_++;

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