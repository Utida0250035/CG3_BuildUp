#include "Audio/StreamingVoiceCallBack.h"
#include "Audio/StreamingSourceVoice.h"
#include "Audio/AudioDecoder.h"

namespace Atrum::Audio {

	void StreamingVoiceCallback::RefillBuffer(void* pBufferContext) {

		uint64_t currentBufferIndex = reinterpret_cast<uintptr_t>(pBufferContext);;

		parentVoice->nextBufferIndex = (currentBufferIndex + 1) % StreamingSourceVoice::kBufferCount;

		DWORD bytesRead = 0;

		bool isSuccess = AudioDecoder::ReadNextChunk(*parentVoice, parentVoice->pBuffers[currentBufferIndex].data(), StreamingSourceVoice::kBufferSize, &bytesRead);

		if (isSuccess && bytesRead > 0) {
			XAUDIO2_BUFFER buf{};
			buf.AudioBytes = bytesRead;
			buf.pAudioData = parentVoice->pBuffers[currentBufferIndex].data();
			// pContext には parentVoice 自身を入れておくことが一般的
			buf.pContext = pBufferContext;

			parentVoice->pVoice->SubmitSourceBuffer(&buf);
		} else {

			if (parentVoice->isLoop) {

				AudioDecoder::Seek(*parentVoice, parentVoice->startTime100ns);

				bool isSuccess1 = AudioDecoder::ReadNextChunk(*parentVoice, parentVoice->pBuffers[currentBufferIndex].data(), StreamingSourceVoice::kBufferSize, &bytesRead);

				if (isSuccess1 && bytesRead > 0) {
					XAUDIO2_BUFFER buf{};
					buf.AudioBytes = bytesRead;
					buf.pAudioData = parentVoice->pBuffers[currentBufferIndex].data();
					parentVoice->pVoice->SubmitSourceBuffer(&buf);
				}

				return;

			}

			// EOF またはエラー時の処理 (ループ設定の確認など)
			parentVoice->state = VoiceState::Waiting;

		}

	}

	void STDMETHODCALLTYPE StreamingVoiceCallback::OnBufferEnd(void* pBufferContext) {

		RefillBuffer(pBufferContext);

	}

	void STDMETHODCALLTYPE StreamingVoiceCallback::OnStreamEnd() {

		parentVoice->state = VoiceState::Waiting;

	}

}