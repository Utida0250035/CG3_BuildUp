#pragma once

#include "Audio/VoiceState.h"
#include "Audio/VoiceCallBack.h"

#include <atomic>
#include <memory>

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

namespace Atrum::Audio {

	struct SourceVoice {
		IXAudio2SourceVoice* pVoice = nullptr;
		std::unique_ptr<VoiceCallback> pCallBack = nullptr;

		std::atomic<VoiceState> state = { VoiceState::Stopped };

		uint64_t playId = 0;

		/// PlayShort(..., isLoop=true) 時のみ使用（BGM 停止・占有解放用）
		bool isLoopPlayback = false;
		size_t loopSoundIndex = static_cast<size_t>(-1);

		~SourceVoice() {

			if (state == VoiceState::Playing) {

				pVoice->Stop();

			}

			pVoice->DestroyVoice();

		}

	};

}