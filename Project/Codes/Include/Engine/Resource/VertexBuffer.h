#pragma once
#include "Engine/Resource/VertexData.h"
#include <cstdint>
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <wrl/client.h>


namespace Atrum {

	class VertexBuffer {

	private:

		// 頂点リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

		// VertexBufferView
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

		// 頂点データ（現在のフレームスロットの先頭）
		VertexData* vertexData_ = nullptr;

		// Map した領域全体の先頭
		VertexData* mappedBase_ = nullptr;

		// GPU 実行中のフレームと書き込み領域が重ならないよう、フレームスロット数ぶん確保する
		uint32_t frameSlotCount_ = 1;

		// 画面上に描画済みの頂点の数
		uint32_t vertexDrewCount_ = 0;

		uint32_t vertexMaxCount_ = 0;

	public:

		/// <param name="frameSlotCount"> 同時に GPU 実行され得るフレーム数（通常はバックバッファ数） </param>
		void CreateVertexBuffer(const uint32_t vertexMaxCount, ID3D12Device* device,
			const uint32_t frameSlotCount = 1);

		/// <summary>
		/// 書き込み・描画に使うフレームスロットを切り替える（GPU がそのスロットを使い終えた後に呼ぶ）
		/// </summary>
		void SetFrameSlot(const uint32_t frameSlot);

		void UnMap() {

			if (vertexResource_ && mappedBase_) {

				vertexResource_->Unmap(0, nullptr);
				mappedBase_ = nullptr;
				vertexData_ = nullptr;

			}

		}

		/* カウント加算 */

		void AddDrewCount(const uint32_t add) { vertexDrewCount_ += add; }

		/* セッター */

		void SetVertexData(const VertexData& vertexData, const uint32_t index) {
			if (index >= vertexMaxCount_) {
				return;
			}
			vertexData_[index] = vertexData;
		}

		void ResetDrewCount() { vertexDrewCount_ = 0; }

		/* ゲッター */

		uint32_t GetDrewCount() const { return vertexDrewCount_; }

		uint32_t GetMaxCount() const { return vertexMaxCount_; }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return vertexBufferView_.BufferLocation; }

		D3D12_VERTEX_BUFFER_VIEW* PGetVertexBufferView() { return &vertexBufferView_; }

		/* デストラクタ */

		~VertexBuffer() {

			this->UnMap();

		}

	};

}