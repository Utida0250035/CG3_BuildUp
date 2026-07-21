#pragma once

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

namespace Atrum::Audio {

	struct StreamingSourceVoice;

	class StreamingVoiceCallback : public IXAudio2VoiceCallback {
	private:

		StreamingSourceVoice* parentVoice = nullptr; // 自身を所有するボイスへの参照

		void RefillBuffer(void* pBufferContext);

	public:
		StreamingVoiceCallback(StreamingSourceVoice* v) : parentVoice(v) {}

		// OnBufferEnd が呼ばれたら、parentVoice を通じてデータを補充
		void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;

		void STDMETHODCALLTYPE OnStreamEnd() override;

		// その他、使わないメソッドも override して空実装にする
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnBufferStart(void*) override {}
		void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
		void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}

	};

}