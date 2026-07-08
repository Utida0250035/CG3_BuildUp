#include "Debug/Log.h"
#include "Engine/CreateBufferResource.h"
#include "Engine/IndexBuffer.h"

void IndexBuffer::CreateIndexBuffer(const uint32_t indexMaxCount, ID3D12Device* device) {

	indexResource_ = CreateUploadBuffer(sizeof(uint32_t) * indexMaxCount, device);

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();

	// 使用するリソースのサイズは 頂点3つ分 * triangleMaxCount のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexMaxCount;

	// 1頂点当たりのサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t);

	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();

	// 使用するリソースのサイズ 大雑把に三角形の描画上限数*3 本来は頂点数
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexMaxCount;

	// 1番号当たりのサイズ
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	LogFile::GetInstance()->Log("Created IndexBuffer");

}