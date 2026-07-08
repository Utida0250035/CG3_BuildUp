#pragma once
#include "Engine/VertexData.h"
#include <cstdint>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <wrl/client.h>


class VertexBuffer {

private:

	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	// VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// 頂点データ
	VertexData* vertexData_ = nullptr;

	// 画面上に描画済みの頂点の数
	uint32_t vertexDrewCount_ = 0;

public:

	void CreateVertexBuffer(const uint32_t vertexMaxCount, ID3D12Device* device);

	/* カウント加算 */

	void AddDrewCount(const uint32_t add) { vertexDrewCount_ += add; }

	/* セッター */

	void SetVertexData(const VertexData& vertexData, const uint32_t index) { vertexData_[index] = vertexData; }

	void ResetDrewCount() { vertexDrewCount_ = 0; }

	/* ゲッター */

	uint32_t GetDrewCount() const { return vertexDrewCount_; }

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return vertexResource_->GetGPUVirtualAddress(); }

	D3D12_VERTEX_BUFFER_VIEW* PGetVertexBufferView() { return &vertexBufferView_; }

};