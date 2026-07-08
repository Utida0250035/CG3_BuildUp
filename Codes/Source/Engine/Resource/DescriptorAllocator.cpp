#include "Debug/Log.h"
#include "Engine/Resource/DescriptorAllocator.h"
#include "String/ConvertString.h"

void DescriptorAllocator::Initialize(const D3D12_DESCRIPTOR_HEAP_TYPE descriptorType, const uint32_t maxDescriptorCount, const bool isShaderVisible, std::wstring descriptorName, ID3D12Device* device) {

	assert(!isInitialized_ && "DescriptorAllocator is already initialized");

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = descriptorType;
	descriptorHeapDesc.NumDescriptors = maxDescriptorCount;

	type_ = descriptorType;
	maxDescriptorCount_ = maxDescriptorCount;
	descriptorSize_ = device->GetDescriptorHandleIncrementSize(type_);

	if (isShaderVisible) {

		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	} else {

		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	}

	[[maybe_unused]]HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap_));

	// ディスクリプタヒープが生成できなかったら起動不可
	assert(SUCCEEDED(hr));

	cpuStart_ = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	if (isShaderVisible) {

		gpuStart_ = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();

	}

	LogFile::GetInstance()->Log("Created " + WStringToString(descriptorName));

	isInitialized_ = true;

}

DescriptorAllocator::DescriptorHandle DescriptorAllocator::Allocate() {

	assert(isInitialized_ && "DescriptorAllocator is not initialized");

	uint32_t index = 0;

	if (freeIndices_.empty()) {

		// 上限チェック
		assert(nextIndex_ < maxDescriptorCount_ && "Descriptor Heap is full");

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

	LogFile::GetInstance()->Log("Allocate Descriptor");

	return handle;

}

DescriptorAllocator::DescriptorHandle DescriptorAllocator::GetHandle(const uint32_t index) {

	assert(isInitialized_ && "DescriptorAllocator is not initialized");

	DescriptorHandle handle{};

	handle.cpu.ptr = cpuStart_.ptr + static_cast<size_t>(descriptorSize_ * index);
	handle.gpu.ptr = gpuStart_.ptr + static_cast<uint64_t>(descriptorSize_ * index);
	handle.index = index;

	return handle;

}

void DescriptorAllocator::Free(const uint32_t index) {

	assert(isInitialized_ && "DescriptorAllocator is not initialized");

	freeIndices_.push_back(index);

}