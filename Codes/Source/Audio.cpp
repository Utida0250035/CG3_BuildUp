#include "Audio.h"
#include "AudioDecoder.h"
#include "ConvertString.h"
#include "Hash64.h"
#include <cassert>
#include <filesystem>
#include <fstream>

Audio::~Audio() {

	xAudio2_.Reset();

	soundDataStorage_.clear();
	soundIndexMap_.clear();

	AudioDecoder::Finalize();

}

void Audio::Initialize() {

	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

	AudioDecoder::Initialize();

}

size_t Audio::SeLoadWave(const char* filePath) {

	assert(std::filesystem::exists(filePath));

	// ファイル入力
	std::ifstream file(filePath, std::ios_base::binary);
	// 開けていなければエラー
	assert(file.is_open());

	// RIFFヘッダの読み込み
	RiffHeader riff = {};
	file.read((char*)&riff, sizeof(riff));

	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(false);
	}

	// タイプが.wavかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(false);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};

	file.read((char*)&format, sizeof(ChunkHeader));

	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		// チャンクヘッダの確認

		assert(false);

	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data{};
	file.read((char*)&data, sizeof(data));

	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {

		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));

	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(false);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	std::vector<uint8_t> pBuffer(data.size);
	file.read(reinterpret_cast<char*>(pBuffer.data()), data.size);

	// Waveファイルを閉じる
	file.close();

	assert(!file.is_open());

	soundIndexMap_.emplace(hash64_str(filePath), soundDataStorage_.size());

	soundDataStorage_.emplace_back(
		std::make_unique<SoundData>(
		format.fmt,
		std::move(pBuffer),
		data.size
	)
	);

	// 管理番号をを参照元に戻す
	return soundDataStorage_.size() - 1;

}

size_t Audio::SeGetWave(const char* filePath) {

	auto search = soundIndexMap_.find(hash64_str(filePath));

	if (search != soundIndexMap_.end()) {

		return search->second;

	}

	return SeLoadWave(filePath);

}

void Audio::PlaySe(size_t soundIndex) {

	assert(soundIndex < soundDataStorage_.size());

	HRESULT hr{};

	auto& soundData = soundDataStorage_[soundIndex];

	// 波形フォーマットをもとにSourceVoiceを生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourceVoice, &(soundData->wfEx));
	assert(SUCCEEDED(hr));


	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData->pBuffer.data();
	buf.AudioBytes = soundData->bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hr));

	hr = pSourceVoice->Start();
	assert(SUCCEEDED(hr));

}

size_t Audio::SeLoadMp3(const char* filePath) {

	std::vector<uint8_t> pData;
	WAVEFORMATEX* wfEx = nullptr;

	bool result = AudioDecoder::LoadAudio(StringToWString(filePath), pData, &wfEx);
	assert(result);

	std::unique_ptr<SoundData> soundData = std::make_unique<SoundData>(
		*wfEx,
		std::move(pData),
		static_cast<unsigned int>(pData.size())
	);

	soundIndexMap_.emplace(hash64_str(filePath), soundDataStorage_.size());
	soundDataStorage_.emplace_back(std::move(soundData));

	CoTaskMemFree(wfEx);

	return soundDataStorage_.size() - 1;

}

size_t Audio::SeGetMp3(const char* filePath) {

	auto search = soundIndexMap_.find(hash64_str(filePath));

	if (search != soundIndexMap_.end()) {

		return search->second;

	}

	return SeLoadMp3(filePath);

}

size_t Audio::SeGet(const char* filePath) {

	std::filesystem::path fPath = filePath;

	const wchar_t* extension = fPath.extension().c_str();

	switch (hash64_str(extension)) {

		case L".mp3"_hash64:

			return SeGetMp3(filePath);

			break;

		case L".wav"_hash64:

			return SeGetWave(filePath);

			break;

		default:

			assert(false && "not supported extension(Audio)");

			break;

	}

	return 65536;

}