#pragma once

#include "Audio/AudioHandle.h"
#include "Audio/SourceVoice.h"
#include "Audio/StreamingSourceVoice.h"
#if defined(DEVELOPMENT)
#include "Audio/AudioStreamingDebugStats.h"
#endif
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Atrum::Audio {

	class AudioManager {

	public:
		~AudioManager();

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
			ChunkHeader chunk{};
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

			std::wstring filePath;

			bool isSuitableStreaming;

		};

#pragma pack(pop)

		std::vector<std::unique_ptr<SoundData>> soundDataStorage_{};
		std::unordered_map<uint64_t, size_t> soundIndexMap_{};
		std::vector<std::unique_ptr<SourceVoice>> sourceVoicePool_{};
		std::vector<std::unique_ptr<StreamingSourceVoice>> streamingSourceVoicePool_{};

		uint64_t nextPlayId_ = 0;

		inline static constexpr size_t kSourceVoiceMax = 64;
		inline static constexpr size_t kStreamSourceVoiceMax = 8;
		inline static constexpr size_t kFileSizeThreshold = 5 * 1024 * 1024;

		WAVEFORMATEX StandardWaveFormatEx();

		void AddSource(const WAVEFORMATEX& wfEx, std::vector<BYTE>&& pBuffer, const UINT bufferSize, const size_t sourceIndex, const char* filePath);

		void CreateVoicePool();

		bool IsSuitableStreaming(const std::string& filePath);

		size_t SetupStreaming(const char* filePath);

		size_t LoadShort(const char* filePath);

		AudioHandle PlayShort(const size_t soundIndex, const bool isLoop);

		size_t AcquireStreamingVoiceIndex();
		void ResetStreamingVoice(StreamingSourceVoice &voice);
		void ResetShortVoice(SourceVoice &voice);
		void ReleaseWaitingShortVoices();

		bool PrepareStreamingDecoder(StreamingSourceVoice& voice, const size_t soundIndex, const long long startTime100ns);

		bool SubmitInitialBuffer(StreamingSourceVoice& voice, const bool isLoop);

		void StartStreaming(StreamingSourceVoice& voice);

		bool InitializeStreaming(const size_t voiceIndex, const size_t soundIndex, const bool isLoop, const long long startTime100ns);


		AudioHandle PlayStreaming(const size_t soundIndex, const bool isLoop, const long long startTime100ns);

		size_t TimeToBytes(long long time100ns, const WAVEFORMATEX& format) const {
			
			long long sampleIndex = (time100ns * format.nSamplesPerSec) / 10000000LL;

			return static_cast<size_t>(sampleIndex * format.nBlockAlign);

		}

		bool RefillBuffer(StreamingSourceVoice& voice);

		bool TryRecoverLoopingStreamingVoice(StreamingSourceVoice &voice);

		bool SeekSceneBgmStreamingVoice(StreamingSourceVoice& voice, float seconds);

	public:

		struct Token {
			explicit Token() = default;
		};

		explicit AudioManager(Token) {}

		void Initialize();

		void Update();

		/// <summary>
		/// 音源の読み込み
		/// </summary>
		/// <param name="filePath"> ファイルパス </param>
		/// <returns> 音源ハンドル </returns>
		size_t Load(const std::string& filePath);

		/// <summary>
		/// 読み込み済み音源の取得
		/// </summary>
		/// <param name="filePath"> ファイルパス </param>
		/// <returns> 音源ハンドル </returns>
		size_t Get(const std::string& filePath);

		AudioHandle Play(const size_t soundIndex, const bool isLoop = false, const long long startTime100ns = 0);

		/// シーン BGM 専用（ストリーミングは固定ボイス・ループ途切れ時の復帰前提）
		AudioHandle PlaySceneBgm(const size_t soundIndex);

		void Stop(const AudioHandle& handle);

		/// ストリーミング BGM 用ボイスをすべて停止（シーン切替・プール枯渇対策）
		void StopAllStreamingVoices();

		/// 短尺ループ（5MB 未満 BGM 等）の占有を解放
		void StopAllLoopingShortVoices();

		bool IsPlaying(const AudioHandle& handle);

		/// シーン BGM の一時停止（再生スロットは維持。IsPlaying は一時停止中も true）
		void PauseSceneBgm(const AudioHandle& handle);

		/// 一時停止したシーン BGM を再開
		void ResumeSceneBgm(const AudioHandle& handle);

		bool IsSceneBgmPaused(const AudioHandle& handle) const;

		/// ループ BGM の再生位置（秒）。失敗時は false
		bool GetSceneBgmPlaybackSeconds(const AudioHandle& handle, float& outSeconds) const;

		/// ループ BGM の再生位置を指定（秒）。再生中・一時停止中どちらも可
		bool SetSceneBgmPlaybackSeconds(const AudioHandle& handle, float seconds);

		/// ピッチ（周波数比）。XAUDIO2 の許容範囲にクランプ
		bool SetHandleFrequencyRatio(const AudioHandle& handle, float ratio);

		bool GetHandleFrequencyRatio(const AudioHandle& handle, float& outRatio) const;

		/// 音量 0〜1（XAUDIO2 に合わせてクランプ）
		bool SetHandleVolume(const AudioHandle& handle, float volume);

		bool GetHandleVolume(const AudioHandle& handle, float& outVolume) const;

#if defined(DEVELOPMENT)
		void CollectStreamingMemoryDebug(StreamingMemoryDebugSnapshot& outSnapshot) const;
#endif

		AudioManager(const AudioManager& source) = delete;
		AudioManager operator=(const AudioManager& source) = delete;

	};

	using Manager = AudioManager;


	inline LONGLONG To100nsPositive(const float seconds) {

		return static_cast<LONGLONG>(std::max(0.0f, seconds) * 10000000.0f);

	}

}