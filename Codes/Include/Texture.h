#pragma once

#include <cstdint>
#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

struct Texture {

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};

	// 使用するSRVディスクリプタの番号
	uint32_t srvIndex = 1;
};