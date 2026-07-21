#pragma once

#include "Audio/VoiceState.h"

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <atomic>
#include <memory>
#include <string>
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

	struct StreamingSourceVoice;

	struct AudioHandle {

		size_t soundIndex;
		size_t voiceIndex;
		bool isStreaming;
		uint64_t playId;

	};

	struct SourceVoice;

	class VoiceCallback : public IXAudio2VoiceCallback {

	private:
		SourceVoice* parentVoice = nullptr;

	public:

		// 再生完了時に自動で呼ばれる
		void STDMETHODCALLTYPE OnBufferEnd(void*) override {
			parentVoice->state = VoiceState::Waiting;
		}

		// 他の仮想関数は空実装でOK
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnStreamEnd() override {}
		void STDMETHODCALLTYPE OnBufferStart(void*) override {}
		void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
		void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}
	};

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

	class AudioManager {

	private:
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		ComPtr<IXAudio2> xAudio2_ = nullptr;
		IXAudio2MasteringVoice* masterVoice_ = nullptr;

#pragma pack(push, 1)

		struct ChunkHeader {
			char id[4]{};
			int32_t size{};
		};

		struct RiffHeader {
			ChunkHeader chunk{};
			char type[4]{};
		};

		struct FormatChunk {
			ChunkHeader chunk{};
			WAVEFORMATEX fmt{};
		};

		struct SoundData {
			// 波形フォーマット
			WAVEFORMATEX wfEx{};
			// バッファの先頭アドレス
			std::vector<BYTE> pBuffer{};
			// バッファのサイズ
			UINT bufferSize = 0;

			std::wstring filePath = L"";

			bool isStreaming = false;

		};

#pragma pack(pop)

		std::vector<std::unique_ptr<SoundData>> soundDataStorage_{};
		std::unordered_map<uint64_t, size_t> soundIndexMap_{};
		std::vector<std::unique_ptr<SourceVoice>> sourceVoicePool_{};
		std::vector<std::unique_ptr<StreamingSourceVoice>> streamingSourceVoicePool_{};

		uint64_t nextPlayId_ = 0;

		void AddSource(const WAVEFORMATEX& wfEx, std::vector<BYTE>&& pBuffer, const UINT bufferSize, const size_t sourceIndex, const char* filePath);

		void CreateVoicePool();

		inline static constexpr size_t kFileSizeThreshold = 5 * 1024 * 1024;

		bool IsSuitableStreaming(const std::string& filePath);

		size_t LoadWave(const char* filePath);

		size_t LoadMp3(const char* filePath);

		inline static constexpr size_t kSourceVoiceMax = 64;
		inline static constexpr size_t kStreamSourceVoiceMax = 8;

		void RefillBuffer(StreamingSourceVoice& voice);

		AudioHandle PlayShort(const size_t soundIndex, const bool isLoop);

		size_t FindFreeStreamingVoice() const;

		void PrepareStreamingDecoder(StreamingSourceVoice& voice, const size_t soundIndex, const long long startTime100ns);

		bool SubmitInitialBuffer(StreamingSourceVoice& voice, const bool isLoop);

		void StartStreaming(StreamingSourceVoice& voice);

		void InitializeStreaming(const size_t voiceIndex, const size_t soundIndex, const bool isLoop, const long long startTime100ns);


		AudioHandle PlayStreaming(const size_t soundIndex, const bool isLoop, const long long startTime100ns);

		size_t TimeToBytes(long long time100ns, const WAVEFORMATEX& format) const {
			// 1秒あたりのバイト数から計算
			double bytesPerSec = (double)format.nAvgBytesPerSec;

			long long alignedTime100ns = AlignToBlock(time100ns, format);

			size_t byteOffset = (size_t)((time100ns / 10000000.0) * bytesPerSec);

			// 【重要】ブロックアラインメントの倍数に切り捨てる
			// これにより、サンプルデータの中途半端な位置（ビットの途中）から読み始めるのを防ぐ
			return (byteOffset / format.nBlockAlign) * format.nBlockAlign;
		}

		// 100ns単位でのブロック境界補正（簡易版）
		long long AlignToBlock(long long time100ns, const WAVEFORMATEX& format) const {
			// 1サンプルあたりの時間を計算
			double secondsPerSample = 1.0 / (double)format.nSamplesPerSec;
			long long timePerSample100ns = (long long)(secondsPerSample * 10000000.0);

			// サンプル単位に切り捨て
			return (time100ns / timePerSample100ns) * timePerSample100ns;
		}

	public:

		void Initialize();

		void Update();

		/// <summary>
		/// 音源の読み込み
		/// </summary>
		/// <param name="filePath"> ファイルパス </param>
		/// <returns> 音源ハンドル </returns>
		size_t Load(const std::string& filePath);

		AudioHandle Play(const size_t soundIndex, const bool isLoop, const long long startTime100ns);

		void Stop(const AudioHandle& handle);

		~AudioManager();

	};

	using Manager = AudioManager;

	using Handle = AudioHandle;

}