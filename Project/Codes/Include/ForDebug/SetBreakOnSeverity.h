#pragma once

#ifdef _DEBUG

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <wrl/client.h>

#endif

namespace Atrum::Debug {

	void SetBreakOnSeverity([[maybe_unused]] ID3D12Device* pDevice) {

#ifdef _DEBUG

		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

			// 致命的なエラー時にブレーク
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			// 通常のエラー時にブレーク
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			// 警告時にブレーク
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		}

#endif

	}

}