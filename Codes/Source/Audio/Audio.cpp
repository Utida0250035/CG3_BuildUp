#include "Audio/Audio.h"
#include "Audio/AudioDecoder.h"
#include "Hash/Hash64.h"
#include "String/ConvertString.h"
#include <cassert>
#include <filesystem>
#include <fstream>

namespace Atrum::Audio {

	AudioManager::~AudioManager() {

		sourceVoicePool_.clear();

		xAudio2_.Reset();

		soundDataStorage_.clear();
		soundIndexMap_.clear();

		AudioDecoder::Finalize();

	}

	void AudioManager::Initialize() {

		HRESULT hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
		assert(SUCCEEDED(hr));

		hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
		assert(SUCCEEDED(hr));

		this->CreateVoicePool();

		AudioDecoder::Initialize();

	}

	void AudioManager::CreateVoicePool() {

		// 標準的なフォーマット設定: 44.1kHz, 16bit, ステレオ
		WAVEFORMATEX standardWfEx = {};

		// 非圧縮PCM
		standardWfEx.wFormatTag = WAVE_FORMAT_PCM;

		// ステレオ
		standardWfEx.nChannels = 2;

		// 44.1kHz
		standardWfEx.nSamplesPerSec = 44100;

		// 16bit
		standardWfEx.wBitsPerSample = 16;

		standardWfEx.nBlockAlign = (standardWfEx.nChannels * standardWfEx.wBitsPerSample) / 8;

		standardWfEx.nAvgBytesPerSec = standardWfEx.nSamplesPerSec * standardWfEx.nBlockAlign;

		// PCMの場合は0
		standardWfEx.cbSize = 0;

		for (size_t i = 0; i < kSourceVoiceCount; ++i) {
			// 定数分のSourceVoiceを生成

			sourceVoicePool_.emplace_back();
			auto& pSourceVoice = sourceVoicePool_.back();
			pSourceVoice = std::make_unique<SourceVoice>();
			pSourceVoice->pCallBack = std::make_unique<VoiceCallback>();

			[[maybe_unused]] HRESULT hr = xAudio2_->CreateSourceVoice(&pSourceVoice->pVoice, &standardWfEx, 0, 2.0f, pSourceVoice->pCallBack.get());
			assert(SUCCEEDED(hr));

		}

	}

	void AudioManager::AddSource(const WAVEFORMATEX& wfEx, std::vector<BYTE>&& pBuffer, const UINT bufferSize, const size_t sourceIndex, const char* filePath) {

		std::unique_ptr<SoundData> soundData = std::make_unique<SoundData>(
			wfEx,
			pBuffer,
			bufferSize
		);

		soundDataStorage_.emplace_back(std::move(soundData));

		soundIndexMap_.emplace(Hash64(filePath), sourceIndex);

	}

	size_t AudioManager::SeLoadWave(const char* filePath) {

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

		this->AddSource(format.fmt, std::move(pBuffer), static_cast<UINT>(data.size), soundDataStorage_.size(), filePath);

		// 管理番号をを参照元に戻す
		return soundDataStorage_.size() - 1;

	}

	size_t AudioManager::PlaySe(size_t soundIndex) {

		assert(soundIndex < soundDataStorage_.size());

		HRESULT hr{};

		auto& soundData = soundDataStorage_[soundIndex];

		XAUDIO2_BUFFER buf{};
		buf.pAudioData = soundData->pBuffer.data();
		buf.AudioBytes = soundData->bufferSize;
		buf.Flags = XAUDIO2_END_OF_STREAM;

		size_t playIndex = 0;

		for (auto& pSourceVoice : sourceVoicePool_) {

			if (!pSourceVoice->pCallBack->isPlaying) {

				pSourceVoice->pCallBack->isPlaying = true;

				hr = pSourceVoice->pVoice->SubmitSourceBuffer(&buf);
				assert(SUCCEEDED(hr));

				hr = pSourceVoice->pVoice->Start();
				assert(SUCCEEDED(hr));

				return playIndex;

			}

			playIndex++;

		}

		assert(false && "no empty sourceVoice");

		return 0;

	}

	size_t AudioManager::SeLoadMp3(const char* filePath) {

		std::vector<uint8_t> pBuffer;
		WAVEFORMATEX* wfEx = nullptr;

		[[maybe_unused]] bool result = AudioDecoder::LoadAudio(StringToWString(filePath), pBuffer, &wfEx);
		assert(result);

		this->AddSource(*wfEx, std::move(pBuffer), static_cast<UINT>(pBuffer.size()), soundDataStorage_.size(), filePath);

		CoTaskMemFree(wfEx);

		return soundDataStorage_.size() - 1;

	}

	size_t AudioManager::LoadSe(const char* filePath) {

		assert(!soundIndexMap_.contains(Hash64(filePath)));

		std::filesystem::path fPath = filePath;

		std::filesystem::path extension = fPath.extension().c_str();

		switch (Hash64(extension.c_str())) {

			case L".mp3"_hash64:

				return SeLoadMp3(filePath);

				break;

			case L".wav"_hash64:

				return SeLoadWave(filePath);

				break;

			default:

				assert(false && "not supported extension(Audio)");

				break;

		}

		return 65536;

	}

}