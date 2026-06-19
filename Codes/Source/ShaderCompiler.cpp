#include "ConvertString.h"
#include "Log.h"
#include "ShaderCompiler.h"
#include <cassert>
#include <format>

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

void ShaderCompiler::Initialize() {

	//dxcCompilerを初期化
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// includeに対応するための設定
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));

	LogFile::GetInstance()->Log("Initialized DXC");

}

void ShaderCompiler::CompileShaders() {

	// Shaderをコンパイルする

	vertexShaderBlob_ = this->Compile(L"Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = this->Compile(L"Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

	LogFile::GetInstance()->Log("Shaders Compiled");

}

ComPtr<IDxcBlob> ShaderCompiler::Compile(const std::wstring& filePath, const wchar_t* profile) {

	// これからシェーダーをコンパイルする旨をログ出力
	LogFile::GetInstance()->Log(WStringToString(std::format(L"Begin CompileShader, path:{}, profile:{}", filePath, profile)));

	/*
	hlslファイルを読む
	*/
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	// 読めなかったら止める
	assert(SUCCEEDED(hr));

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
	hr = dxcCompiler_->Compile(
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
	assert(SUCCEEDED(hr));

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
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	// 成功した旨のログ出力
	LogFile::GetInstance()->Log(WStringToString(std::format(L"Compile Succeeded, path:{}, profile:{}", filePath, profile)));

	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();

	LogFile::GetInstance()->Log("shader Compiled");

	return shaderBlob;

}