#pragma once

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <wrl/client.h>
#include <vector>
#include <unordered_map>
#include <memory>

class Audio {

private:
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<IXAudio2> xAudio2_ = nullptr;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;

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
		BYTE* pBuffer;
		// バッファのサイズ
		unsigned int bufferSize;

		~SoundData() {

			delete[] pBuffer;
			
			pBuffer = nullptr;

			bufferSize = 0;

			wfEx = {};

		}

	};

	std::vector<std::unique_ptr<SoundData>> soundDataStorage_{};
	std::unordered_map<uint64_t, size_t> soundIndexMap_{};

	size_t SoundLoadWave(const char* filePath);

public:

	void Initialize();

	size_t SoundGetWave(const char* filePath);

	void SoundPlayWave(size_t soundIndex);

	~Audio();

};