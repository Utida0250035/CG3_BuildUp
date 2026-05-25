#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <vector>

class Fence {
private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12Fence> fence_;
	HANDLE fenceEvent_ = nullptr;

	std::vector<uint64_t> fenceValues_{};
	uint64_t totalFenceCount_ = 0;

	void Initialize(ComPtr<ID3D12Device>& device, const uint32_t backBufferCount);

public:
	Fence() = default;
	~Fence() { if(fenceEvent_) CloseHandle(fenceEvent_); }

};