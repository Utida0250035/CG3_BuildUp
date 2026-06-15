#include "Audio.h"
#include "Hash64.h"
#include <cassert>
#include <filesystem>
#include <fstream>


void Audio::Initialize() {

	HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr));

	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr));

}

size_t Audio::SoundLoadWave(const char* filePath) {

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
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	// Waveファイルを閉じる
	file.close();

	assert(!file.is_open());

	soundIndexMap_.emplace(hash64_str(filePath), soundDataStorage_.size());

	soundDataStorage_.emplace_back(
		std::make_unique<SoundData>(
			format.fmt,
			reinterpret_cast<BYTE*>(pBuffer),
			data.size
		)
	);

	// 管理番号をを参照元に戻す
	return soundDataStorage_.size() - 1;

}

size_t Audio::SoundGetWave(const char* filePath) {

	auto search = soundIndexMap_.find(hash64_str(filePath));

	if (search != soundIndexMap_.end()) {

		return search->second;

	}

	return SoundLoadWave(filePath);

}

void Audio::SoundPlayWave(size_t soundIndex) {

	assert(soundIndex < soundDataStorage_.size());

	HRESULT hr{};

	auto& soundData = soundDataStorage_[soundIndex];

	// 波形フォーマットをもとにSourceVoiceを生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	hr = xAudio2_->CreateSourceVoice(&pSourceVoice, &(soundData->wfEx));
	assert(SUCCEEDED(hr));


	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData->pBuffer;
	buf.AudioBytes = soundData->bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(hr));

	hr = pSourceVoice->Start();
	assert(SUCCEEDED(hr));

}

Audio::~Audio() {

	xAudio2_.Reset();

	soundDataStorage_.clear();
	soundIndexMap_.clear();

}