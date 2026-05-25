#include "DescriptorAllocator.h"

DescriptorAllocator::DescriptorHandle DescriptorAllocator::Allocate() {

	uint32_t index = 0;

	if (freeIndices_.empty()) {

		// 上限チェック
		assert(nextIndex_ < maxDescriptors_ && "Descriptor Heap is full");

		index = nextIndex_;

		nextIndex_++;

	} else {

		index = freeIndices_.back();

		freeIndices_.pop_back();

	}

	DescriptorHandle handle;
	handle.index = index;
	handle.cpu.ptr = cpuStart_.ptr + static_cast<size_t>(descriptorSize_ * handle.index);
	handle.gpu.ptr = gpuStart_.ptr + static_cast<size_t>(descriptorSize_ * handle.index);

	return handle;

}

void DescriptorAllocator::Free(const uint32_t index) {

	freeIndices_.push_back(index);

}