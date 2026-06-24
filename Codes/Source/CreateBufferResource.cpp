#include "CreateBufferResource.h"
#include "Log.h"
#include <cassert>

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceState, ID3D12Device* device) {

	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = heapType;

	D3D12_RESOURCE_DESC resourceDesc{};

	// バッファリソース テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	resourceDesc.Width = sizeInBytes;

	// バッファの場合はこれらを1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;

	// バッファの場合はコレにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 実際にリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	[[maybe_unused]] HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, resourceState, nullptr, IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	LogFile::GetInstance()->Log("Created BufferResource");

	return resource;

}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadBuffer(size_t sizeInBytes, ID3D12Device* device) {

	return CreateBufferResource(sizeInBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, device);

}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(size_t sizeInBytes, ID3D12Device* device) {

	return CreateBufferResource(sizeInBytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, device);

}