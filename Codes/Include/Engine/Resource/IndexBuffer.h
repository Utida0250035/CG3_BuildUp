#pragma once

#include <cstdint>
#include <wrl/client.h>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

namespace Atrum {

	class IndexBuffer {

	private:

		// インデックスリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;

		// IndexBufferView
		D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

		// 頂点インデックスデータ
		uint32_t* indexData_ = nullptr;

		// 頂点インデックスカウント
		uint32_t indexDrewCount_ = 0;

	public:

		void CreateIndexBuffer(const uint32_t indexMaxCount, ID3D12Device* device);

		/* カウント加算 */

		void AddDrewCount(const uint32_t add) { indexDrewCount_ += add; }

		/* セッター */

		void SetIndexData(const uint32_t indexData, const uint32_t index) { indexData_[index] = indexData; }

		void ResetDrewCount() { indexDrewCount_ = 0; }

		/* ゲッター */

		uint32_t GetDrewCount() const { return indexDrewCount_; }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return indexResource_->GetGPUVirtualAddress(); }

		D3D12_INDEX_BUFFER_VIEW* PGetBufferView() { return &indexBufferView_; }

	};

}