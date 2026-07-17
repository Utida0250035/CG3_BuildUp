#pragma once

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <atomic>
#include <memory>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")


namespace Atrum::Audio {

	enum class VoiceState {
		Stopped,
		Playing,
		Paused,
		Loading
	};

	struct StreamingSourceVoice {

		VoiceState state = VoiceState::Stopped;

		IXAudio2SourceVoice* pVoice = nullptr;
		// メディアデータ読み込み用
		Microsoft::WRL::ComPtr<IMFSourceReader> pReader;

		// 読み込みと再生の状態管理
		std::atomic<bool> isStreaming{ false };

		// ストリーミング用にバッファを複数持つ（ダブルバッファリング）
		inline static constexpr size_t kBufferCount = 3;
		// 64KB単位の読み込みvg
		inline static constexpr size_t kBufferSize = 65536;
		BYTE* pBuffers[kBufferCount];
	};


	class VoiceCallback : public IXAudio2VoiceCallback {
	public:
		std::atomic<bool> isPlaying{ false };

		// 再生完了時に自動で呼ばれる
		void STDMETHODCALLTYPE OnBufferEnd(void*) override {
			isPlaying = false;
		}

		// 他の仮想関数は空実装でOK
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnStreamEnd() override {}
		void STDMETHODCALLTYPE OnBufferStart(void*) override {}
		void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
		void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}
	};

	class Audio {

	private:
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		ComPtr<IXAudio2> xAudio2_ = nullptr;
		IXAudio2MasteringVoice* masterVoice_ = nullptr;

#pragma pack(push, 1)

		struct ChunkHeader {
			char id[4];
			int32_t size;
		};

		struct RiffHeader {
			ChunkHeader chunk;
			char type[4];
		};

		struct FormatChunk {
			ChunkHeader chunk;
			WAVEFORMATEX fmt;
		};

		struct SoundData {
			// 波形フォーマット
			WAVEFORMATEX wfEx;
			// バッファの先頭アドレス
			std::vector<BYTE> pBuffer;
			// バッファのサイズ
			UINT bufferSize;

		};

		struct SourceVoice {
			IXAudio2SourceVoice* pVoice = nullptr;
			std::unique_ptr<VoiceCallback> pCallBack;

			~SourceVoice() {

				if (pCallBack->isPlaying) {

					pVoice->Stop();

				}

				pVoice->DestroyVoice();

			}

		};

#pragma pack(pop)

		std::vector<std::unique_ptr<SoundData>> soundDataStorage_{};
		std::unordered_map<uint64_t, size_t> soundIndexMap_{};
		std::vector<std::unique_ptr<SourceVoice>> sourceVoicePool_{};

		void AddSource(const WAVEFORMATEX& wfEx, std::vector<BYTE>&& pBuffer, const UINT bufferSize, const size_t sourceIndex, const char* filePath);

		void CreateVoicePool();

		size_t SeLoadWave(const char* filePath);

		size_t SeLoadMp3(const char* filePath);

		inline static constexpr size_t kSourceVoiceCount = 64;

	public:

		void Initialize();

		size_t PlaySe(size_t soundIndex);

		/// <summary>
		/// 音源の読み込み
		/// </summary>
		/// <param name="filePath"> ファイルパス </param>
		/// <returns> 音源ハンドル </returns>
		size_t LoadSe(const char* filePath);

		~Audio();

	};

}