#pragma once

#if defined(ATRUM_D3D12_DEBUG_LAYER)

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <wrl/client.h>

#endif

namespace Atrum::Debug {

	inline void SetBreakOnSeverity([[maybe_unused]] ID3D12Device* pDevice) {

#if defined(ATRUM_D3D12_DEBUG_LAYER)

		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

			// 致命的なエラー時にブレーク
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			// 通常のエラー時にブレーク
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

		}

#endif

	}

}