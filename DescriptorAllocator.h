#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <cstdint>
#include <vector>
#include <cassert>
#include <wrl/client.h>
#include <string>

class DescriptorAllocator {

public:

	// 割り当て用
	struct DescriptorHandle {
		// CPUのハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE cpu;
		// GPUのハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE gpu;
		// 管理番号
		uint32_t index;
	};

private:

	bool isInitialized_ = false;

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

	// 次の未使用ディスクリプタの番号
	uint32_t nextIndex_ = 0;
	// 空いたディスクリプタの番号
	std::vector<uint32_t> freeIndices_{};

	// ディスクリプタヒープの種類 初期値は種類数(無効な種類)
	D3D12_DESCRIPTOR_HEAP_TYPE type_ = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;

	// 作成したディスクリプタの個数
	uint32_t maxDescriptorCount_ = 0;
	// ディスクリプタのサイズ
	uint32_t descriptorSize_ = 0;

	// CPU側の最初のハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart_{ 0 };
	// GPU側の最初のハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE gpuStart_{ 0 };

public:

	DescriptorAllocator() = default;
	~DescriptorAllocator() = default;

	void Initialize(const D3D12_DESCRIPTOR_HEAP_TYPE descriptorType, const uint32_t maxDescriptorCount, const bool isShaderVisible, const std::wstring descriptorName, ComPtr<ID3D12Device>& device);

	DescriptorHandle Allocate();
	DescriptorHandle GetHandle(const uint32_t index);
	void Free(const uint32_t index);

	uint32_t GetMaxDescriptorCount() const { return maxDescriptorCount_; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuStart()const { return cpuStart_; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuStart()const { return gpuStart_; }
	ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap() { return descriptorHeap_; }

};