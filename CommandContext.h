#pragma once

#include <cstdint>
#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include <DirectXTex/DirectXTex.h>

#include <wrl/client.h>

#include <vector>
#include <array>

#include <memory>
#include <map>

class CommandContext {

private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// Type
	D3D12_COMMAND_LIST_TYPE type_ = D3D12_COMMAND_LIST_TYPE_NONE;

	// コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	// コマンドアロケータ(コマンド割り当て担当)
	std::vector<ComPtr<ID3D12CommandAllocator>> commandAllocators_{};

	// コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;


	void CreateCommandQueue(ComPtr<ID3D12Device>& device);

	void CreateCommandAllocators(ComPtr<ID3D12Device>& device, const UINT backBufferContext);

	void CreateCommandList(ComPtr<ID3D12Device>& device);

public:

	void Initialize(ComPtr<ID3D12Device>& device, const UINT beckBufferContext, const D3D12_COMMAND_LIST_TYPE type);

	/* ゲッター */

	ComPtr<ID3D12CommandQueue>& GetCommandQueue() { return commandQueue_; }

	std::vector<ComPtr<ID3D12CommandAllocator>>& GetCommandAllocators() { return commandAllocators_; }

	ComPtr<ID3D12GraphicsCommandList>& GetCommandList() { return commandList_; }

	D3D12_COMMAND_LIST_TYPE GetType()const { return type_; }

};