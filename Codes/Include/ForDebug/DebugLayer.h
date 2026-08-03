#pragma once

#ifdef _DEBUG

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <wrl/client.h>

#endif

namespace Atrum::Debug {

	void EnableDebugLayer() {

#ifdef _DEBUG

		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController;

		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {

			// デバッグレイヤーを有効化
			debugController->EnableDebugLayer();

			// GPU側でもチェックを行なうようにする
			debugController->SetEnableGPUBasedValidation(TRUE);

		}

#endif

	}

}