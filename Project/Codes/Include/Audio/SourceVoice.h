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

		~SourceVoice() {

			if (state == VoiceState::Playing) {

				pVoice->Stop();

			}

			pVoice->DestroyVoice();

		}

	};

}