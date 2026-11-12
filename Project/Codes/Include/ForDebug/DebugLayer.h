#pragma once

// 既定の Debug 構成では無効。検証が必要なときだけ vcxproj に
// ATRUM_D3D12_DEBUG_LAYER を追加する（Development 相当の実行性を優先）。

#if defined(ATRUM_D3D12_DEBUG_LAYER)

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <wrl/client.h>

#endif

namespace Atrum::Debug {

	inline void EnableDebugLayer() {

#if defined(ATRUM_D3D12_DEBUG_LAYER)

		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;

		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
			debugController->SetEnableGPUBasedValidation(FALSE);
		}

#endif

	}

}