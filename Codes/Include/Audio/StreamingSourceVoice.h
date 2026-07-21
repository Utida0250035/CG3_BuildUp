#pragma once

#include "Audio/VoiceState.h"
#include "Audio/StreamingVoiceCallBack.h"

#include <atomic>
#include <vector>
#include <wrl/client.h>
#include <windows.h>
#include <memory>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

namespace Atrum::Audio {

	struct StreamingSourceVoice {

		std::atomic<VoiceState> state = { VoiceState::Stopped };

		std::unique_ptr<StreamingVoiceCallback> pCallBack = nullptr;

		IXAudio2SourceVoice* pVoice = nullptr;
		// メディアデータ読み込み用
		Microsoft::WRL::ComPtr<IMFSourceReader> pReader = nullptr;

		// 読み込みと再生の状態管理
		std::atomic<bool> isStreaming{ false };

		std::vector<BYTE> remainingData;

		// ストリーミング用にバッファを複数持つ（ダブルバッファリング）
		inline static constexpr size_t kBufferCount = 3;
		// 64KB単位の読み込みvg
		inline static constexpr size_t kBufferSize = 1024 * 64;
		std::vector<BYTE> pBuffers[kBufferCount]{};

		size_t nextBufferIndex = 0;

		uint64_t playId = 0;

		size_t soundIndex = 0;

		bool isLoop = false;

		long long startTime100ns = 0;

		~StreamingSourceVoice() {

			if (state == VoiceState::Playing) {

				pVoice->Stop();

			}

			pVoice->DestroyVoice();

		}

	};

}