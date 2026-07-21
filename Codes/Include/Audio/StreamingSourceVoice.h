#pragma once

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <atomic>
#include <vector>
#include <wrl/client.h>
#include <windows.h>
#include "Audio/VoiceState.h"
#include "Audio/AudioDecoder.h"

#include <mfreadwrite.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace Atrum::Audio {

	class StreamingVoiceCallback : public IXAudio2VoiceCallback {
	private:

		StreamingSourceVoice* parentVoice = nullptr; // 自身を所有するボイスへの参照

	public:
		StreamingVoiceCallback(StreamingSourceVoice* v) : parentVoice(v) {}

		// OnBufferEnd が呼ばれたら、parentVoice を通じてデータを補充
		void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override {

			uint64_t currentBufferIndex = parentVoice->nextBufferIndex;

			parentVoice->nextBufferIndex = (parentVoice->nextBufferIndex + 1) % StreamingSourceVoice::kBufferCount;

			DWORD bytesRead = 0;

			AudioDecoder::ReadNextChunk(*parentVoice, parentVoice->pBuffers[currentBufferIndex], StreamingSourceVoice::kBufferSize, &bytesRead);

			if (bytesRead > 0) {
				XAUDIO2_BUFFER buf{};
				buf.AudioBytes = bytesRead;
				buf.pAudioData = parentVoice->pBuffers[currentBufferIndex];
				// pContext には parentVoice 自身を入れておくことが一般的
				buf.pContext = pBufferContext;

				parentVoice->pVoice->SubmitSourceBuffer(&buf);
			} else {

				if (parentVoice->isLoop) {

					AudioDecoder::Seek(*parentVoice, 0);

					AudioDecoder::ReadNextChunk(*parentVoice, parentVoice->pBuffers[currentBufferIndex], StreamingSourceVoice::kBufferSize, &bytesRead);

					if (bytesRead > 0) {
						XAUDIO2_BUFFER buf{};
						buf.AudioBytes = bytesRead;
						buf.pAudioData = parentVoice->pBuffers[currentBufferIndex];
						parentVoice->pVoice->SubmitSourceBuffer(&buf);
					}

					return;

				}

				// EOF またはエラー時の処理 (ループ設定の確認など)
				parentVoice->state = VoiceState::Waiting;

			}

		}

		void STDMETHODCALLTYPE OnStreamEnd() override {

			parentVoice->state = VoiceState::Waiting;

		}

		// その他、使わないメソッドも override して空実装にする
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnBufferStart(void*) override {}
		void STDMETHODCALLTYPE OnLoopEnd(void*) override {}

	};

	struct StreamingSourceVoice {

		std::atomic<VoiceState> state = { VoiceState::Stopped };

		std::unique_ptr<StreamingVoiceCallback> pCallBack = nullptr;

		IXAudio2SourceVoice* pVoice = nullptr;
		// メディアデータ読み込み用
		Microsoft::WRL::ComPtr<IMFSourceReader> pReader;

		// 読み込みと再生の状態管理
		std::atomic<bool> isStreaming{ false };

		std::vector<BYTE> remainingData;

		// ストリーミング用にバッファを複数持つ（ダブルバッファリング）
		inline static constexpr size_t kBufferCount = 3;
		// 64KB単位の読み込みvg
		inline static constexpr size_t kBufferSize = 65536;
		BYTE* pBuffers[kBufferCount];

		size_t nextBufferIndex = 0;

		uint64_t playId = 0;

		size_t soundIndex = 0;

		bool isLoop = false;

		~StreamingSourceVoice() {

			if (state == VoiceState::Playing) {

				pVoice->Stop();

			}

			pVoice->DestroyVoice();

		}

	};

}