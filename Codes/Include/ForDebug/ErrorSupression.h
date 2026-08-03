#pragma once

#include <cassert>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

namespace Atrum::Debug {

	void ErrorSuppressionDebug([[maybe_unused]] ID3D12Device* pDevice) {

#ifdef _DEBUG

		ID3D12InfoQueue* infoQueue = nullptr;

		if (SUCCEEDED(pDevice->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

			// 深刻なエラー時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

			// エラー時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

			// 警告時に止まる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			D3D12_MESSAGE_ID denyIds[] = {
				// Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
				D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
			};

			// 抑制するレベル
			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
			D3D12_INFO_QUEUE_FILTER filter{};
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = severities;

			// 指定したメッセージの表示を抑制する
			infoQueue->PushStorageFilter(&filter);

			// エラー情報キューの解放
			infoQueue->Release();

		}

#endif

	}

}