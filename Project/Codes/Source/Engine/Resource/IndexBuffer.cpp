#include "Engine/Alias/CoreAlias.h"

#include "ForDebug/Log.h"
#include "Engine/Resource/CreateBufferResource.h"
#include "Engine/Resource/IndexBuffer.h"

namespace Atrum {

	void IndexBuffer::CreateIndexBuffer(const uint32_t indexMaxCount, ID3D12Device* device,
		const uint32_t frameSlotCount) {

		indexMaxCount_ = indexMaxCount;
		frameSlotCount_ = frameSlotCount > 0 ? frameSlotCount : 1;

		indexResource_ = CreateUploadBuffer(
			sizeof(uint32_t) * indexMaxCount * frameSlotCount_, device);

		indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase_));

		// 使用するリソースのサイズは 1 スロット分（indexMaxCount 個）
		indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexMaxCount;

		// 1番号当たりのサイズ
		indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

		SetFrameSlot(0);

		D::LogFile::Instance()->Log("Created IndexBuffer");

	}

	void IndexBuffer::SetFrameSlot(const uint32_t frameSlot) {

		const uint32_t slot = frameSlot % frameSlotCount_;
		const size_t slotOffset = static_cast<size_t>(slot) * indexMaxCount_;

		indexData_ = mappedBase_ + slotOffset;

		indexBufferView_.BufferLocation =
			indexResource_->GetGPUVirtualAddress() + slotOffset * sizeof(uint32_t);

	}

}
