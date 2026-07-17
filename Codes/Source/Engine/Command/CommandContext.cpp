#include "Debug/Log.h"
#include "Engine/Command/CommandContext.h"

namespace Atrum {

	void CommandContext::CreateCommandQueue(ID3D12Device* device) {

		assert(!isInitialized_ && "CreateCommandQueue() is initializeHelper");

		[[maybe_unused]] HRESULT hr;

		// コマンドキューの生成
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
		commandQueueDesc.Type = type_;
		commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.NodeMask = 0;
		hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));

		// コマンドキューの生成がうまくいかなかったら起動できない
		assert(SUCCEEDED(hr));

		hr = commandQueue_->SetName(L"commandQueue");
		assert(SUCCEEDED(hr));

		LogFile::GetInstance()->Log("Created CommandQueue");

	}

	void CommandContext::CreateCommandAllocators(ID3D12Device* device, const UINT backBufferCount) {

		assert(!isInitialized_ && "CreateCommandAllocators() is initializeHelper");

		[[maybe_unused]] HRESULT hr;

		commandAllocators_.resize(static_cast<size_t>(backBufferCount));

		for (auto& commandAllocator : commandAllocators_) {

			// コマンドアロケータの生成
			hr = device->CreateCommandAllocator(type_, IID_PPV_ARGS(&commandAllocator));

			// コマンドアロケータの生成がうまくいかなかったら起動不可
			assert(SUCCEEDED(hr));

			hr = commandAllocator->SetName(L"commandAllocator");
			assert(SUCCEEDED(hr));

		}

		LogFile::GetInstance()->Log("Created CommandAllocators");

	}

	void CommandContext::CreateCommandList(ID3D12Device* device) {

		assert(!isInitialized_ && "CreateCommandList() is initializeHelper");

		[[maybe_unused]] HRESULT hr;

		// コマンドリストの生成
		hr = device->CreateCommandList(0, type_, commandAllocators_[0].Get(), nullptr, IID_PPV_ARGS(&commandList_));

		// コマンドリストの生成がうまくいかなかったら起動不可
		assert(SUCCEEDED(hr));

		hr = commandList_->SetName(L"commandList");
		assert(SUCCEEDED(hr));

		LogFile::GetInstance()->Log("Created CommandList");

	}

	void CommandContext::Initialize(ID3D12Device* device, const UINT backBufferCount, const D3D12_COMMAND_LIST_TYPE type) {

		assert(!isInitialized_ && "CommandContext is already initialized");

		type_ = type;

		this->CreateCommandQueue(device);

		this->CreateCommandAllocators(device, backBufferCount);

		this->CreateCommandList(device);

		isInitialized_ = true;

	}

}