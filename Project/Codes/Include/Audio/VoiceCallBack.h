#pragma once

#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

namespace Atrum::Audio {

	struct SourceVoice;

	class VoiceCallback : public IXAudio2VoiceCallback {

	private:
		SourceVoice* parentVoice = nullptr;

	public:
		VoiceCallback(SourceVoice* v) : parentVoice(v) {}

		// 再生完了時に自動で呼ばれる
		void STDMETHODCALLTYPE OnBufferEnd(void*) override;

		// 他の仮想関数は空実装でOK
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnStreamEnd() override {}
		void STDMETHODCALLTYPE OnBufferStart(void*) override {}
		void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
		void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}

	};

}