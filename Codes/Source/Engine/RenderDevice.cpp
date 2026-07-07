#include "Debug/Log.h"
#include "Engine/RenderDevice.h"
#include "String/ConvertString.h"
#include <cassert>
#include <format>

void RenderDevice::CreateDxgiFactory() {

	assert(!isInitialized_ && "CreateDxgiFactory() is initializeHelper");

	[[maybe_unused]]HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	/*
	初期化の根本的な段階でエラーが出た場合は
	プログラムの間違いか修正不可である場合が多い
	*/
	assert(SUCCEEDED(hr));

	LogFile::GetInstance()->Log("Created DxgiFactory");

}

void RenderDevice::SelectAdapter() {

	assert(!isInitialized_ && "SelectAdapter() is initializeHelper");

	[[maybe_unused]]HRESULT hr;

	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) != DXGI_ERROR_NOT_FOUND; ++i) {
		// パフォーマンスが良い順にアダプタのリストを出させる 

		// アダプターの情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);

		// アダプターの情報が取得できない場合はエラー
		assert(SUCCEEDED(hr));

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// ソフトウェアアダプタでなければ採用

			// 採用したアダプタの情報をログに出力
			LogFile::GetInstance()->Log(WStringToString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));

			break;

		}

		// 次のアダプタへ
		useAdapter_ = nullptr;

	}

	// 適切なアダプターが見当たらない場合は起動不可
	assert(useAdapter_ != nullptr);

	LogFile::GetInstance()->Log("SelectAdapter worked correctly.");

}

void RenderDevice::CreateDevice() {

	assert(!isInitialized_ && "CreateDevice() is initializeHelper");

	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };

	[[maybe_unused]]HRESULT hr;

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 機能レベルが高い順に、生成できるか試していく

		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));

		if (SUCCEEDED(hr)) {
			// 指定した機能レベルでデバイスが生成できた

			// ログ出力

			LogFile::GetInstance()->Log(std::format("FeatureLevel: {}\n", featureLevelStrings[i]));

			break;

		}

	}

	// デバイスの生成が成功しなかった場合は実行不可
	assert(device_ != nullptr);

	device_->SetName(L"device");

	LogFile::GetInstance()->Log("Created ID3D12Device");

}

void RenderDevice::Initialize() {

	assert(!isInitialized_ && "RenderDevice is already initialized");

	this->CreateDxgiFactory();
	
	this->SelectAdapter();

	this->CreateDevice();

	// デバイス初期化完了のログを出す
	LogFile::GetInstance()->Log("Complete Init RenderDevice");

	isInitialized_ = true;

}