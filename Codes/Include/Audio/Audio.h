#pragma once

#include "Audio/StreamingSourceVoice.h"
#include "Audio/SourceVoice.h"
#include "Audio/AudioHandle.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace Atrum::Audio {

	class AudioManager {

	private:

		~AudioManager();
		AudioManager() = default;

		static AudioManager* instance_;

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

		size_t FindFreeStreamingVoice() const;

		void PrepareStreamingDecoder(StreamingSourceVoice& voice, const size_t soundIndex, const long long startTime100ns);

		bool SubmitInitialBuffer(StreamingSourceVoice& voice, const bool isLoop);

		void StartStreaming(StreamingSourceVoice& voice);

		bool InitializeStreaming(const size_t voiceIndex, const size_t soundIndex, const bool isLoop, const long long startTime100ns);


		AudioHandle PlayStreaming(const size_t soundIndex, const bool isLoop, const long long startTime100ns);

		size_t TimeToBytes(long long time100ns, const WAVEFORMATEX& format) const {
			
			long long sampleIndex = (time100ns * format.nSamplesPerSec) / 10000000LL;

			return static_cast<size_t>(sampleIndex * format.nBlockAlign);

		}

		bool RefillBuffer(StreamingSourceVoice& voice);

	public:

		void Initialize();

		void Update();

		/// <summary>
		/// 音源の読み込み
		/// </summary>
		/// <param name="filePath"> ファイルパス </param>
		/// <returns> 音源ハンドル </returns>
		size_t Load(const std::string& filePath);

		AudioHandle Play(const size_t soundIndex, const bool isLoop = false, const long long startTime100ns = 0);

		void Stop(const AudioHandle& handle);

		bool IsPlaying(const AudioHandle& handle);

		static AudioManager* GetInstance() {

			if (!instance_) {

				instance_ = new AudioManager();

			}

			return instance_;

		}

		static void Destroy() {

			if (instance_) {

				delete instance_;

				instance_ = nullptr;

			}

		}

		AudioManager(const AudioManager& source) = delete;
		AudioManager operator=(const AudioManager& source) = delete;

	};

	using Manager = AudioManager;

	using Handle = AudioHandle;


	inline LONGLONG To100nsPositive(const float seconds) {

		return static_cast<LONGLONG>(std::max(0.0f, seconds) * 10000000.0f);

	}

}