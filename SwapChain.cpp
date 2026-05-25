#include "SwapChain.h"
#include <string>

void SwapChain::Initialize(const int32_t clientWidth, const int32_t clientHeight, ComPtr<IDXGIFactory7>& dxgiFactory, ComPtr<ID3D12CommandQueue>& commandQueue, HWND hwnd) {

	// スワップチェーンに渡す情報
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = clientWidth;
	swapChainDesc.Height = clientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = kBackBufferCount;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// コマンドキュー、ウィンドウハンドル、設定を渡してスワップチェーンを生成
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	for (UINT i = 0; i < kBackBufferCount; ++i) {

		// バッファの取得
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
		assert(SUCCEEDED(hr));

		// バッファの命名
		hr = swapChainResources_[i]->SetName((L"swapChainResource" + std::to_wstring(i)).c_str());
		assert(SUCCEEDED(hr));

	}

}

void SwapChain::UpdateBackBufferIndex() {

	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

}