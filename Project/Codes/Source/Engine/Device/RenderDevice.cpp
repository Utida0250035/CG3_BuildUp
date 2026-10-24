#include "ForDebug/Log.h"
#include "Engine/Device/RenderDevice.h"
#include "String/ConvertString.h"
#include <Windows.h>
#include <cassert>
#include <cstdlib>
#include <format>

namespace Atrum {

namespace {

[[noreturn]] void ReportRenderDeviceFatalError(const wchar_t *message) {
  MessageBoxW(nullptr, message, L"CG2 BuildUp", MB_OK | MB_ICONERROR);
  std::abort();
}

} // namespace

	void RenderDevice::CreateDxgiFactory() {

		assert(!isInitialized_ && "CreateDxgiFactory() is initializeHelper");

		[[maybe_unused]] HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

		/*
		初期化の根本的な段階でエラーが出た場合は
		プログラムの間違いか修正不可である場合が多い
		*/
		assert(SUCCEEDED(hr));

		Debug::LogFile::Instance()->Log("Created DxgiFactory");

	}

	void RenderDevice::SelectAdapter() {

		assert(!isInitialized_ && "SelectAdapter() is initializeHelper");

		[[maybe_unused]] HRESULT hr;

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
				Debug::LogFile::Instance()->Log(WStringToString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));

				break;

			}

			// 次のアダプタへ
			useAdapter_ = nullptr;

		}

		// 適切なアダプターが見当たらない場合は起動不可
		if (useAdapter_ == nullptr) {
			ReportRenderDeviceFatalError(
			    L"D3D12 対応の GPU が見つかりません。\n"
			    L"グラフィックドライバを更新するか、対応 GPU で実行してください。");
		}
		assert(useAdapter_ != nullptr);

		Debug::LogFile::Instance()->Log("SelectAdapter worked correctly.");

	}

	void RenderDevice::CreateDevice() {

		assert(!isInitialized_ && "CreateDevice() is initializeHelper");

		D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
		};

		const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };

		[[maybe_unused]] HRESULT hr;

		for (size_t i = 0; i < _countof(featureLevels); ++i) {
			// 機能レベルが高い順に、生成できるか試していく

			hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));

			if (SUCCEEDED(hr)) {
				// 指定した機能レベルでデバイスが生成できた

				// ログ出力

				Debug::LogFile::Instance()->Log(std::format("FeatureLevel: {}\n", featureLevelStrings[i]));

				break;

			}

		}

		// デバイスの生成が成功しなかった場合は実行不可
		if (device_ == nullptr) {
			ReportRenderDeviceFatalError(
			    L"Direct3D 12 デバイスを作成できませんでした。\n"
			    L"Feature Level 12.0 以上が必要です。");
		}
		assert(device_ != nullptr);

		device_->SetName(L"device");

		Debug::LogFile::Instance()->Log("Created ID3D12Device");

	}

	void RenderDevice::Initialize() {

		assert(!isInitialized_ && "RenderDevice is already initialized");

		this->CreateDxgiFactory();

		this->SelectAdapter();

		this->CreateDevice();

		// デバイス初期化完了のログを出す
		Debug::LogFile::Instance()->Log("Complete Init RenderDevice");

		isInitialized_ = true;

	}

}