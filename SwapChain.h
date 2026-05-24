#pragma once

#include "DescriptorAllocator.h"
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#include <wrl/client.h>
#include <memory>

class SwapChain {

private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// コマンドアロケータの個数
	inline static constexpr UINT kBackBufferCount = 2u;

	// 使用するコマンドアロケータの番号
	UINT backBufferIndex_ = 0u;

	// スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	// スワップチェーンリソース
	ComPtr<ID3D12Resource> swapChainResources_[kBackBufferCount] = { nullptr };

public:

	void Initialize(ComPtr<ID3D12Device>& device, ComPtr<IDXGIFactory4>& factory, ComPtr<ID3D12CommandQueue>& commandQueue, HWND hwnd, std::unique_ptr<DescriptorAllocator>& rtvAllocator);

};