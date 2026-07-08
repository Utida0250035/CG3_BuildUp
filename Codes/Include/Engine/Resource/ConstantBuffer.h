#pragma once
#include <cstdint>
#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "Engine/Resource/CreateBufferResource.h"

template <typename T>
class SingleConstantBuffer {

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

	T* data_ = nullptr;

public:

	void CreateBuffer(ID3D12Device* device) {

		// Data1つ分のサイズを用意
		resource_ = CreateUploadBuffer(sizeof(T), device);

		// データを書き込むためのアドレスを取得
		resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	}

	/* ゲッター */

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

	/* セッター */

	void SetData(const T& data) { memcpy(data_, &data, sizeof(T)); }

};

template<typename T>
class MultiConstantBuffer {

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

	T* data_ = nullptr;

public:

	void CreateBuffer(ID3D12Device* device, const size_t resourceCount) {

		// Data1つ分 * resourceCountのサイズを用意
		resource_ = CreateUploadBuffer(resourceCount * sizeof(T), device);

		// データを書き込むためのアドレスを取得
		resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	}

	/* ゲッター */

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

	/* セッター */

	void SetData(const T& data, const size_t index) { data_[index] = data; }

};