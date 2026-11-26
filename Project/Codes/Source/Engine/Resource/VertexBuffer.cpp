#include "Engine/Alias/CoreAlias.h"

#include "ForDebug/Log.h"
#include "Engine/Resource/CreateBufferResource.h"
#include "Engine/Resource/VertexBuffer.h"

namespace Atrum {

	void VertexBuffer::CreateVertexBuffer(const uint32_t vertexMaxCount, ID3D12Device* device,
		const uint32_t frameSlotCount) {

		vertexMaxCount_ = vertexMaxCount;
		frameSlotCount_ = frameSlotCount > 0 ? frameSlotCount : 1;

		vertexResource_ = CreateUploadBuffer(
			sizeof(VertexData) * vertexMaxCount * frameSlotCount_, device);

		vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase_));

		// 使用するリソースのサイズは 1 スロット分（vertexMaxCount 頂点）
		vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexMaxCount;

		// 1頂点当たりのサイズ
		vertexBufferView_.StrideInBytes = sizeof(VertexData);

		SetFrameSlot(0);

		D::LogFile::Instance()->Log("Created VertexBuffer");

	}

	void VertexBuffer::SetFrameSlot(const uint32_t frameSlot) {

		const uint32_t slot = frameSlot % frameSlotCount_;
		const size_t slotOffset = static_cast<size_t>(slot) * vertexMaxCount_;

		vertexData_ = mappedBase_ + slotOffset;

		vertexBufferView_.BufferLocation =
			vertexResource_->GetGPUVirtualAddress() + slotOffset * sizeof(VertexData);

	}

}
