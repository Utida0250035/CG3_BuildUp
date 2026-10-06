#include "Audio/StreamingVoiceCallBack.h"
#include "Audio/StreamingSourceVoice.h"
#include "Audio/AudioDecoder.h"

namespace Atrum::Audio {

	void STDMETHODCALLTYPE StreamingVoiceCallback::OnBufferEnd(void*) {

		parentVoice->state = VoiceState::Streaming;

	}

	void STDMETHODCALLTYPE StreamingVoiceCallback::OnStreamEnd() {

		parentVoice->state = VoiceState::Waiting;

	}

}