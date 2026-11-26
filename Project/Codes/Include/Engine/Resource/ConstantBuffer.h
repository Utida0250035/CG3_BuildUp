#pragma once
#include <cstdint>
#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include "Engine/Resource/CreateBufferResource.h"

namespace Atrum {

	template <typename T>
	class SingleConstantBuffer {

	private:

		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		T* data_ = nullptr;

	public:

		void CreateBuffer(ID3D12Device* device) {

			// Data1つ分のサイズを用意
			resource_ = CreateUploadBuffer(sizeof(T), device);

			// データを書き込むためのアドレスを取得
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

		}

		void UnMap() {

			if (resource_ && data_) {

				resource_->Unmap(0, nullptr);

				data_ = nullptr;

			}

		}

		/* ゲッター */

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }

		/* セッター */

		void SetData(const T& data) { memcpy(data_, &data, sizeof(T)); }

		/* デストラクタ */

		~SingleConstantBuffer() {

			this->UnMap();

		}

	};

	template<typename T>
	class MultiConstantBuffer {

	private:

		Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

		// 現在のフレームスロットの先頭
		T* data_ = nullptr;

		// Map した領域全体の先頭
		T* mappedBase_ = nullptr;

		size_t resourceCount_ = 0;

		// GPU 実行中のフレームと書き込み領域が重ならないよう、フレームスロット数ぶん確保する
		uint32_t frameSlotCount_ = 1;

		D3D12_GPU_VIRTUAL_ADDRESS slotGpuAddress_ = 0;

	public:

		/// <param name="frameSlotCount"> 同時に GPU 実行され得るフレーム数（通常はバックバッファ数） </param>
		void CreateBuffer(ID3D12Device* device, const size_t resourceCount,
			const uint32_t frameSlotCount = 1) {

			resourceCount_ = resourceCount;
			frameSlotCount_ = frameSlotCount > 0 ? frameSlotCount : 1;

			// Data1つ分 * resourceCount * フレームスロット数のサイズを用意
			resource_ = CreateUploadBuffer(
				resourceCount * sizeof(T) * frameSlotCount_, device);

			// データを書き込むためのアドレスを取得
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedBase_));

			SetFrameSlot(0);

		}

		/// <summary>
		/// 書き込み・描画に使うフレームスロットを切り替える（GPU がそのスロットを使い終えた後に呼ぶ）
		/// </summary>
		void SetFrameSlot(const uint32_t frameSlot) {

			const size_t slotOffset =
				static_cast<size_t>(frameSlot % frameSlotCount_) * resourceCount_;

			data_ = mappedBase_ + slotOffset;
			slotGpuAddress_ = resource_->GetGPUVirtualAddress() + slotOffset * sizeof(T);

		}

		void UnMap() {

			if (resource_ && mappedBase_) {

				resource_->Unmap(0, nullptr);

				mappedBase_ = nullptr;
				data_ = nullptr;

			}

		}

		/* ゲッター */

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return slotGpuAddress_; }

		/* セッター */

		void SetData(const T& data, const size_t index) {
			if (index >= resourceCount_) {
				return;
			}
			data_[index] = data;
		}

		/* デストラクタ */

		~MultiConstantBuffer() {

			this->UnMap();

		}

	};

}