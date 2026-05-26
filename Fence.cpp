#include "Fence.h"
#include <cassert>

void Fence::Initialize(ComPtr<ID3D12Device>& device, const uint32_t backBufferCount) {

	assert(!isInitialized_ && "Fence is already initialized");
	assert(backBufferCount > 0 && "BufferCount must be bigger than 0");

	// fenceValues_の要素数をバッファ数に合わせて全要素を0で初期化
	fenceValues_.resize(static_cast<size_t>(backBufferCount), 0u);

	[[maybe_unused]]HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));

	fenceEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	assert(fenceEvent_);

	isInitialized_ = true;

}

void Fence::Signal(ComPtr<ID3D12CommandQueue>& commandQueue, const uint32_t backBufferIndex) {

	assert(isInitialized_ && "Fence is not initialized");

	// 境界チェック(配列外アクセス防止)
	assert(backBufferIndex < fenceValues_.size() && "Invalid backBufferIndex");

	totalFenceCount_++;
	fenceValues_[backBufferIndex] = totalFenceCount_;

	[[maybe_unused]]HRESULT hr = commandQueue->Signal(fence_.Get(), totalFenceCount_);
	assert(SUCCEEDED(hr));

}

void Fence::WaitForNextBuffer(const uint32_t nextBackBufferIndex) {

	assert(isInitialized_ && "Fence is not initialized");

	// 境界チェック
	assert(nextBackBufferIndex < fenceValues_.size() && "Invalid nextBackBufferIndex");

	uint64_t targetValue = fenceValues_[nextBackBufferIndex];

	if (fence_->GetCompletedValue() < targetValue) {

		[[maybe_unused]]HRESULT hr = fence_->SetEventOnCompletion(targetValue, fenceEvent_);
		assert(SUCCEEDED(hr));
		WaitForSingleObject(fenceEvent_, INFINITE);

	}

}

void Fence::ForceSyncGPU(ComPtr<ID3D12CommandQueue>& commandQueue) {

	assert(isInitialized_ && "Fence is not initialized");

	totalFenceCount_++;
	commandQueue->Signal(fence_.Get(), totalFenceCount_);

	if (fence_->GetCompletedValue() < totalFenceCount_) {

		[[maybe_unused]]HRESULT hr = fence_->SetEventOnCompletion(totalFenceCount_, fenceEvent_);
		assert(SUCCEEDED(hr));
		WaitForSingleObject(fenceEvent_, INFINITE);

	}

}