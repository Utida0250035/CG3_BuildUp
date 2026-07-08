#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceState, ID3D12Device* device);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadBuffer(size_t sizeInBytes, ID3D12Device* device);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(size_t sizeInBytes, ID3D12Device* device);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateIntermediateResource(const size_t intermediateSize, ID3D12Device* device);