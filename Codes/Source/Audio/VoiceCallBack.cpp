#include "Audio/VoiceCallBack.h"
#include "Audio/SourceVoice.h"

namespace Atrum::Audio {

	void STDMETHODCALLTYPE VoiceCallback::OnBufferEnd(void*) {
		parentVoice->state = VoiceState::Waiting;
	}

}