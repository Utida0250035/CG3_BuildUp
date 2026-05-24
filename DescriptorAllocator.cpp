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

}

void DescriptorAllocator::Free(const uint32_t index) {

	freeIndices_.push_back(index);

}