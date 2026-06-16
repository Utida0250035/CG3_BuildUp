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
		unsigned int bufferSize;

	};

#pragma pack(pop)

	std::vector<std::unique_ptr<SoundData>> soundDataStorage_{};
	std::unordered_map<uint64_t, size_t> soundIndexMap_{};

	size_t SeLoadWave(const char* filePath);

	size_t SeLoadMp3(const char* filePath);

public:

	void Initialize();

	size_t SeGetWave(const char* filePath);

	void PlaySe(size_t soundIndex);

	size_t SeGetMp3(const char* filePath);

	size_t SeGet(const char* filePath);

	~Audio();

};