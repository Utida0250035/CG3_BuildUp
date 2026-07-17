#include "Debug/Log.h"
#include "Engine/Device/SwapChain.h"
#include <string>

namespace Atrum {

	void SwapChain::Initialize(const int32_t clientWidth, const int32_t clientHeight, ID3D12Device* device, IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue, HWND hwnd, DescriptorAllocator* rtvAllocator, const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc) {

		assert(!isInitialized_ && "SwapChain is already initialized");

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
		[[maybe_unused]] HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
		assert(SUCCEEDED(hr));

		for (UINT i = 0; i < kBackBufferCount; ++i) {

			// バッファの取得
			hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&swapChainResources_[i]));
			assert(SUCCEEDED(hr));

			// バッファの命名
			hr = swapChainResources_[i]->SetName((L"swapChainResource" + std::to_wstring(i)).c_str());
			assert(SUCCEEDED(hr));

		}

		LogFile::GetInstance()->Log("Created SwapChain");

		rtvAllocator->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kBackBufferCount, false, L"rtvDescriptors", device);



		for (uint32_t i = 0; i < kBackBufferCount; ++i) {

			DescriptorAllocator::DescriptorHandle handle = rtvAllocator->GetHandle(i);

			rtvHandles_[i] = handle.cpu;

			device->CreateRenderTargetView(swapChainResources_[i].Get(), &rtvDesc, rtvHandles_[i]);

		}

		isInitialized_ = true;

	}

	void SwapChain::UpdateBackBufferIndex() {

		assert(isInitialized_ && "SwapChain is not initialized");

		backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();

	}

}