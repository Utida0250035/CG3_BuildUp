#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <stdint.h>
#include <vector>
#include <cassert>
#include <wrl/client.h>

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

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;

	// 次の未使用ディスクリプタの番号
	uint32_t nextIndex_ = 0;
	// 空いたディスクリプタの番号
	std::vector<uint32_t> freeIndices_{};

	// ディスクリプタヒープの種類
	D3D12_DESCRIPTOR_HEAP_TYPE type_;

	// 作成したディスクリプタの個数
	uint32_t maxDescriptors_ = 0;
	// ディスクリプタのサイズ
	uint32_t descriptorSize_ = 0;

	// CPU側の最初のハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE cpuStart_{ 0 };
	// GPU側の最初のハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE gpuStart_{ 0 };

public:

	DescriptorAllocator() = default;
	~DescriptorAllocator() = default;

	DescriptorHandle Allocate();

	void Free(const uint32_t index);

};