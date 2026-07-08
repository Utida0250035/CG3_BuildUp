#include "Debug/Log.h"
#include "Engine/Resource/CreateBufferResource.h"
#include "Engine/Resource/VertexBuffer.h"

void VertexBuffer::CreateVertexBuffer(const uint32_t vertexMaxCount, ID3D12Device* device) {

	vertexResource_ = CreateUploadBuffer(sizeof(VertexData) * vertexMaxCount, device);

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	// 使用するリソースのサイズは 頂点3つ分 * triangleMaxCount のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexMaxCount;

	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	// 使用するリソースのサイズは 頂点3つ分 * triangleMaxCount のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexMaxCount;

	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	LogFile::GetInstance()->Log("Created VertexBuffer");

}