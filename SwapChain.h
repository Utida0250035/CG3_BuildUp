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

	void Initialize(const int32_t clientWidth, const int32_t clientHeight, ComPtr<IDXGIFactory4>& factory, ComPtr<ID3D12CommandQueue>& commandQueue, HWND hwnd);

	void UpdateBackBufferIndex();

	/* ゲッター */

	ComPtr<IDXGISwapChain4>& GetSwapChain() { return swapChain_; }
	ComPtr<ID3D12Resource>& GetSwapChainResourceCurrent() { return swapChainResources_[backBufferIndex_]; }
	UINT GetBackBufferIndex() const {return backBufferIndex_};

};