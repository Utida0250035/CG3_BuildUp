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

public:

	// コマンドアロケータの個数
	inline static constexpr UINT kBackBufferCount = 2u;

private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool isInitialized_ = false;

	// 使用するコマンドアロケータの番号
	UINT backBufferIndex_ = 0u;

	// スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	// スワップチェーンリソース
	ComPtr<ID3D12Resource> swapChainResources_[kBackBufferCount] = { nullptr };

	// RTVディスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[kBackBufferCount]{};

public:

	void Initialize(const int32_t clientWidth, const int32_t clientHeight, ID3D12Device* device, IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue, HWND hwnd, DescriptorAllocator* rtvAllocator, const D3D12_RENDER_TARGET_VIEW_DESC& rtvDesc);

	void UpdateBackBufferIndex();

	/* ゲッター */

	ComPtr<IDXGISwapChain4>& GetSwapChain() { return swapChain_; }
	ComPtr<ID3D12Resource>& GetSwapChainResourceCurrent() { return swapChainResources_[backBufferIndex_]; }
	ComPtr<ID3D12Resource>& GetSwapChainResource(const size_t index) { return swapChainResources_[index]; }
	UINT GetBackBufferIndex() const { return backBufferIndex_; }
	D3D12_CPU_DESCRIPTOR_HANDLE* PGetRtvHandles() { return rtvHandles_; }
	D3D12_CPU_DESCRIPTOR_HANDLE* PGetRtvHandleCurrent() { return &rtvHandles_[backBufferIndex_]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandleCurrent() const { return rtvHandles_[backBufferIndex_]; }

};