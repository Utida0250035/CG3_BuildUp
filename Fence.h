#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <vector>

class Fence {
private:

	bool isInitialized_ = false;

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Fence> fence_;
	HANDLE fenceEvent_ = nullptr;

	std::vector<uint64_t> fenceValues_{};
	uint64_t totalFenceCount_ = 0;

public:
	Fence() = default;
	~Fence() { if(fenceEvent_) CloseHandle(fenceEvent_); }

	void Initialize(ComPtr<ID3D12Device>& device, const uint32_t backBufferCount);
	
	void Signal(ComPtr<ID3D12CommandQueue>& commandQueue, const uint32_t backBufferIndex);
	void WaitForNextBuffer(const uint32_t nextBackBufferIndex);
	void ForceSyncGPU(ComPtr<ID3D12CommandQueue>& commandQueue);

};