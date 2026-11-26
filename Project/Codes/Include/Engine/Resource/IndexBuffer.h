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

		// 頂点インデックスデータ（現在のフレームスロットの先頭）
		uint32_t* indexData_ = nullptr;

		// Map した領域全体の先頭
		uint32_t* mappedBase_ = nullptr;

		// GPU 実行中のフレームと書き込み領域が重ならないよう、フレームスロット数ぶん確保する
		uint32_t frameSlotCount_ = 1;

		// 頂点インデックスカウント
		uint32_t indexDrewCount_ = 0;

		uint32_t indexMaxCount_ = 0;

	public:

		/// <param name="frameSlotCount"> 同時に GPU 実行され得るフレーム数（通常はバックバッファ数） </param>
		void CreateIndexBuffer(const uint32_t indexMaxCount, ID3D12Device* device,
			const uint32_t frameSlotCount = 1);

		/// <summary>
		/// 書き込み・描画に使うフレームスロットを切り替える（GPU がそのスロットを使い終えた後に呼ぶ）
		/// </summary>
		void SetFrameSlot(const uint32_t frameSlot);

		void UnMap() {

			if (indexResource_ && mappedBase_) {

				indexResource_->Unmap(0, nullptr);

				mappedBase_ = nullptr;
				indexData_ = nullptr;

			}

		}

		/* カウント加算 */

		void AddDrewCount(const uint32_t add) { indexDrewCount_ += add; }

		/* セッター */

		void SetIndexData(const uint32_t indexData, const uint32_t index) {
			if (index >= indexMaxCount_) {
				return;
			}
			indexData_[index] = indexData;
		}

		void ResetDrewCount() { indexDrewCount_ = 0; }

		/* ゲッター */

		uint32_t GetDrewCount() const { return indexDrewCount_; }

		uint32_t GetMaxCount() const { return indexMaxCount_; }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return indexBufferView_.BufferLocation; }

		D3D12_INDEX_BUFFER_VIEW* PGetBufferView() { return &indexBufferView_; }

		/* デストラクタ */

		~IndexBuffer() {

			this->UnMap();

		}

	};

}