#include "Audio/StreamingVoiceCallBack.h"
#include "Audio/StreamingSourceVoice.h"
#include "Audio/AudioDecoder.h"

namespace Atrum::Audio {

	void STDMETHODCALLTYPE StreamingVoiceCallback::OnBufferEnd(void*) {

		parentVoice->state = VoiceState::Streaming;

	}

	void STDMETHODCALLTYPE StreamingVoiceCallback::OnStreamEnd() {

		if (parentVoice == nullptr) {
			return;
		}

		// Waiting だけにすると Update 1 フレーム遅れてループ復帰する。
		// 非ループも Streaming 経由で Refill → Waiting → キュー枯渇待ちへ渡す。
		parentVoice->state = VoiceState::Streaming;

	}

}