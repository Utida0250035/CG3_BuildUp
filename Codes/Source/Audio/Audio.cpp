#include "Audio/Audio.h"
#include "Audio/AudioDecoder.h"
#include "Audio/StreamingSourceVoice.h"
#include "File/FileSize.h"
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

	void AudioManager::Update() {

		for (auto& streamingVoice : streamingSourceVoicePool_) {
		// ストリーミング再生中かつ、バッファを補充する必要があるかチェック
			if (streamingVoice->state == VoiceState::Playing) {

				XAUDIO2_VOICE_STATE state;
				streamingVoice->pVoice->GetState(&state);

				// 再生中のバッファ数が kBufferCount 未満なら補充する
				if (state.BuffersQueued < streamingVoice->kBufferCount) {
					// 補充処理（ReadNextChunk を呼んで SubmitSourceBuffer する）
					RefillBuffer(*streamingVoice);
				}
			}
		}

	}

	void AudioManager::RefillBuffer(StreamingSourceVoice& voice) {

		// 現在のインデックスのバッファを使う
		BYTE* pBuffer = voice.pBuffers[voice.nextBufferIndex];
		DWORD bytesRead = 0;

		// 1. ファイルからデータを読み込む
		if (AudioDecoder::ReadNextChunk(voice, pBuffer, voice.kBufferSize, &bytesRead)) {

			// 2. XAudio2用バッファ構造体の設定
			XAUDIO2_BUFFER xBuffer = {};
			xBuffer.AudioBytes = bytesRead;
			xBuffer.pAudioData = pBuffer;
			xBuffer.Flags = 0;

			// 3. ボイスへ送信
			voice.pVoice->SubmitSourceBuffer(&xBuffer);

			// 4. インデックスを更新（循環させる）
			voice.nextBufferIndex = (voice.nextBufferIndex + 1) % voice.kBufferCount;

		} else {

			// 読み込みができなかった場合（ファイルの終端など）の処理
			// ここで state を Stopped に変えるか、ループ再生なら先頭に戻す処理を行う
			voice.state = VoiceState::Stopped;

		}

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

		for (size_t i = 0; i < kSourceVoiceMax; ++i) {
			// 定数分のSourceVoiceを生成

			sourceVoicePool_.emplace_back();
			auto& pSourceVoice = sourceVoicePool_.back();
			pSourceVoice = std::make_unique<SourceVoice>();
			pSourceVoice->pCallBack = std::make_unique<VoiceCallback>(pSourceVoice.get());

			[[maybe_unused]] HRESULT hr = xAudio2_->CreateSourceVoice(&pSourceVoice->pVoice, &standardWfEx, 0, 2.0f, pSourceVoice->pCallBack.get());
			assert(SUCCEEDED(hr));

		}

		for (size_t i = 0; i < kStreamSourceVoiceMax; ++i) {

			streamingSourceVoicePool_.emplace_back();
			auto& pStrmSourceVoice = streamingSourceVoicePool_.back();
			pStrmSourceVoice = std::make_unique<StreamingSourceVoice>();
			pStrmSourceVoice->pCallBack = std::make_unique<StreamingVoiceCallback>(pStrmSourceVoice.get());

			[[maybe_unused]] HRESULT hr = xAudio2_->CreateSourceVoice(&pStrmSourceVoice->pVoice, &standardWfEx, 0, 2.0f, pStrmSourceVoice->pCallBack.get());
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

	bool AudioManager::IsSuitableStreaming(const std::string& filePath) {

		return (File::GetFileSize(filePath) > kFileSizeThreshold);

	}

	size_t AudioManager::LoadWave(const char* filePath) {

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

	size_t AudioManager::LoadMp3(const char* filePath) {

		std::vector<uint8_t> pBuffer;
		WAVEFORMATEX* wfEx = nullptr;

		[[maybe_unused]] bool result = AudioDecoder::LoadAudio(StringToWString(filePath), pBuffer, &wfEx);
		assert(result);

		this->AddSource(*wfEx, std::move(pBuffer), static_cast<UINT>(pBuffer.size()), soundDataStorage_.size(), filePath);

		CoTaskMemFree(wfEx);

		return soundDataStorage_.size() - 1;

	}

	size_t AudioManager::Load(const std::string& filePath) {

		assert(!soundIndexMap_.contains(Hash64(filePath)));

		std::filesystem::path fPath = filePath;

		std::filesystem::path extension = fPath.extension().c_str();

		switch (Hash64(extension.c_str())) {

			case L".mp3"_hash64:

				return LoadMp3(filePath.c_str());

				break;

			case L".wav"_hash64:

				return LoadWave(filePath.c_str());

				break;

			default:

				assert(false && "not supported extension(Audio)");

				break;

		}

		return 65536;

	}

	AudioHandle AudioManager::PlayShort(const size_t soundIndex, const bool isLoop) {

		assert(soundIndex < soundDataStorage_.size());

		HRESULT hr{};

		auto& soundData = soundDataStorage_[soundIndex];

		XAUDIO2_BUFFER buf{};
		buf.pAudioData = soundData->pBuffer.data();
		buf.AudioBytes = soundData->bufferSize;
		buf.Flags = XAUDIO2_END_OF_STREAM;

		if (isLoop) {

			buf.LoopCount = XAUDIO2_LOOP_INFINITE;

		} else {

			buf.LoopCount = 0;

		}

		size_t voiceIndex = 0;

		AudioHandle handle{};
		handle.isStreaming = false;
		handle.soundIndex = soundIndex;

		for (auto& pSourceVoice : sourceVoicePool_) {

			if (pSourceVoice->state == VoiceState::Stopped) {

				pSourceVoice->state = VoiceState::Playing;

				hr = pSourceVoice->pVoice->SubmitSourceBuffer(&buf);
				assert(SUCCEEDED(hr));

				hr = pSourceVoice->pVoice->Start();
				assert(SUCCEEDED(hr));

				handle.voiceIndex = voiceIndex;
				handle.playId = nextPlayId_++;

				pSourceVoice->playId = handle.playId;

				return handle;

			}

			voiceIndex++;

		}

		assert(false && "no empty sourceVoice");

		return {};

	}

	size_t AudioManager::FindFreeStreamingVoice() const {

		size_t voiceIndex = 0;

		for (auto& pSourceVoice : streamingSourceVoicePool_) {

			if (pSourceVoice->state == VoiceState::Stopped) {

				return voiceIndex;

			}

			voiceIndex++;

		}

		assert(false && "Streaming voice pool is full");

		return -1;

	}

	void AudioManager::PrepareStreamingDecoder(StreamingSourceVoice& voice, const size_t soundIndex, const long long startTime100ns) {

		// IMFSourceReader の作成
		// soundDataStorage_ からファイルパスを取得してリーダーを生成
		auto& path = soundDataStorage_[soundIndex]->filePath; // もしパスを保持していれば
		[[maybe_unused]] HRESULT hr = MFCreateSourceReaderFromURL(path.c_str(), nullptr, &voice.pReader);
		assert(SUCCEEDED(hr));

		AudioDecoder::Seek(voice, startTime100ns);

	}

	bool AudioManager::SubmitInitialBuffer(StreamingSourceVoice& voice, const bool isLoop) {

		// リーダーの設定（必要に応じてオーディオフォーマットの指定など）
		// 通常はデフォルト設定でOK 必要ならここでConfigureSourceReaderを呼ぶ

		DWORD bytesRead = 0;
		// 3. 最初のバッファを読み込む
		if (AudioDecoder::ReadNextChunk(voice, voice.pBuffers[voice.nextBufferIndex], StreamingSourceVoice::kBufferSize, &bytesRead)) {

			// 4. SubmitSourceBuffer
			XAUDIO2_BUFFER buf{};
			buf.AudioBytes = bytesRead;
			buf.pAudioData = voice.pBuffers[0];
			buf.Flags = 0;

			[[maybe_unused]] HRESULT hr{};

			hr = voice.pVoice->SubmitSourceBuffer(&buf);
			assert(SUCCEEDED(hr));

			voice.isLoop = isLoop;

			return true;

		}

		return false;

	}

	void AudioManager::StartStreaming(StreamingSourceVoice& voice) {

		// 5. 再生開始
		[[maybe_unused]] HRESULT hr = voice.pVoice->Start();
		assert(SUCCEEDED(hr));

		voice.state = VoiceState::Playing;

		voice.nextBufferIndex = (voice.nextBufferIndex + 1) % StreamingSourceVoice::kBufferCount;

	}

	void AudioManager::InitializeStreaming(const size_t voiceIndex, const size_t soundIndex, const bool isLoop, const long long startTime100ns) {

		auto& voice = *streamingSourceVoicePool_[voiceIndex];

		assert(voice.state == VoiceState::Stopped);

		PrepareStreamingDecoder(voice, soundIndex, startTime100ns);

		if (SubmitInitialBuffer(voice, isLoop)) {

			StartStreaming(voice);

		}

	}

	AudioHandle AudioManager::PlayStreaming(const size_t soundIndex, const bool isLoop, const long long startTime100ns) {
		assert(soundIndex < soundDataStorage_.size());

		// 1. 空きボイスを探す（ストリーミング用プールから）
		size_t voiceIndex = FindFreeStreamingVoice();

		// 2. IDの発行
		uint64_t currentPlayId = nextPlayId_++;

		// 3. データの初期準備
		auto& streamingVoice = streamingSourceVoicePool_[voiceIndex];
		streamingVoice->playId = currentPlayId;
		streamingVoice->soundIndex = soundIndex;

		// 4. ストリーミング開始処理
		// ファイルを開き、最初のバッファをSubmitしてStart()する専用の関数
		InitializeStreaming(voiceIndex, soundIndex, isLoop, startTime100ns);

		AudioDecoder::Seek(*streamingVoice, startTime100ns);

		// 5. ハンドルを返す
		return AudioHandle{
			.soundIndex = soundIndex,
			.voiceIndex = voiceIndex,
			.isStreaming = true,
			.playId = currentPlayId
		};
	}

	AudioHandle AudioManager::Play(const size_t soundIndex, const bool isLoop = false, const long long startTime100ns = 0) {

		auto& data = soundDataStorage_[soundIndex];

		if (data->isStreaming) {

			return PlayStreaming(soundIndex, isLoop, startTime100ns);

		}

		return PlayShort(soundIndex, isLoop);

	}

	void AudioManager::Stop(const AudioHandle& handle) {

		if (handle.voiceIndex >= kSourceVoiceMax) {
			assert(false && "Invalid voice index");

			return;
		}

		if (handle.isStreaming) {

			auto& sSrcVoice = streamingSourceVoicePool_[handle.voiceIndex];

			if (sSrcVoice && sSrcVoice->playId == handle.playId) {

				sSrcVoice->pVoice->Stop();
				sSrcVoice->pVoice->FlushSourceBuffers();

				sSrcVoice->state = VoiceState::Stopped;
				sSrcVoice->playId = -1;

			}

			return;

		}

		auto& srcVoice = sourceVoicePool_[handle.voiceIndex];

		if (srcVoice && srcVoice->playId == handle.playId) {

			srcVoice->pVoice->Stop();
			srcVoice->pVoice->FlushSourceBuffers();

			srcVoice->state = VoiceState::Stopped;

		}


	}

}