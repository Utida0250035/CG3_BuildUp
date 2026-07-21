#include "Audio/Audio.h"
#include "Audio/AudioDecoder.h"
#include "Audio/StreamingSourceVoice.h"
#include "Audio/StreamingVoiceCallBack.h"
#include "File/FileSize.h"
#include "Hash/Hash64.h"
#include "String/ConvertString.h"
#include <cassert>
#include <filesystem>
#include <fstream>

#include <mfreadwrite.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace Atrum::Audio {

	AudioManager* AudioManager::instance_ = nullptr;

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

		for (auto& voice : sourceVoicePool_) {

			if (voice->state == VoiceState::Waiting) {

				voice->pVoice->Stop();
				voice->pVoice->FlushSourceBuffers();

				voice->state = VoiceState::Stopped;

			}

		}

		for (auto& streamingVoice : streamingSourceVoicePool_) {

			if (streamingVoice->state == VoiceState::Waiting) {

				streamingVoice->pVoice->Stop();
				streamingVoice->pVoice->FlushSourceBuffers();

				if (streamingVoice->pReader) {

					streamingVoice->pReader->Release();
					streamingVoice->pReader = nullptr;

				}

				streamingVoice->state = VoiceState::Stopped;

			}

		}

	}

	void AudioManager::CreateVoicePool() {

		WAVEFORMATEX standardWfEx = StandardWaveFormatEx();

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

	WAVEFORMATEX AudioManager::StandardWaveFormatEx() {

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

		return standardWfEx;

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

	size_t AudioManager::SetupStreaming(const char* filePath) {

		soundDataStorage_.emplace_back(std::make_unique<SoundData>());

		auto& soundData = soundDataStorage_.back();

		soundData->filePath = StringToWString(filePath);
		soundData->isSuitableStreaming = true;

		soundData->wfEx = StandardWaveFormatEx();

		return soundDataStorage_.size() - 1;

	}

	size_t AudioManager::LoadShort(const char* filePath) {

		std::vector<uint8_t> pBuffer;
		WAVEFORMATEX* wfEx = new WAVEFORMATEX();

		[[maybe_unused]] bool result = AudioDecoder::LoadAudio(StringToWString(filePath), pBuffer, &wfEx);
		assert(result);

		this->AddSource(*wfEx, std::move(pBuffer), static_cast<UINT>(pBuffer.size()), soundDataStorage_.size(), filePath);

		CoTaskMemFree(wfEx);

		return soundDataStorage_.size() - 1;

	}

	size_t AudioManager::Load(const std::string& filePath) {

		assert(!soundIndexMap_.contains(Hash64(filePath)));

		if (IsSuitableStreaming(filePath)) {

			return SetupStreaming(filePath.c_str());

		}

		return LoadShort(filePath.c_str());

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

		return 65536;

	}

	void AudioManager::PrepareStreamingDecoder(StreamingSourceVoice& voice, const size_t soundIndex, const long long startTime100ns) {

		// IMFSourceReader の作成
		// soundDataStorage_ からファイルパスを取得してリーダーを生成
		auto& path = soundDataStorage_[soundIndex]->filePath; // もしパスを保持していれば
		[[maybe_unused]] HRESULT hr = MFCreateSourceReaderFromURL(path.c_str(), nullptr, &voice.pReader);
		assert(SUCCEEDED(hr));

		voice.startTime100ns = startTime100ns;

		AudioDecoder::Seek(voice, startTime100ns);

	}

	bool AudioManager::SubmitInitialBuffer(StreamingSourceVoice& voice, const bool isLoop) {

		// リーダーの設定（必要に応じてオーディオフォーマットの指定など）
		// 通常はデフォルト設定でOK 必要ならここでConfigureSourceReaderを呼ぶ

		voice.nextBufferIndex = 0;

		voice.pBuffers[voice.nextBufferIndex].resize(StreamingSourceVoice::kBufferSize);

		DWORD bytesRead = 0;
		// 3. 最初のバッファを読み込む
		if (AudioDecoder::ReadNextChunk(voice, voice.pBuffers[voice.nextBufferIndex].data(), StreamingSourceVoice::kBufferSize, &bytesRead)) {

			// 4. SubmitSourceBuffer
			XAUDIO2_BUFFER buf{};
			buf.AudioBytes = bytesRead;
			buf.pAudioData = voice.pBuffers[voice.nextBufferIndex].data();
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

	bool AudioManager::InitializeStreaming(const size_t voiceIndex, const size_t soundIndex, const bool isLoop, const long long startTime100ns) {

		auto& voice = *streamingSourceVoicePool_[voiceIndex];

		assert(voice.state == VoiceState::Stopped);

		PrepareStreamingDecoder(voice, soundIndex, startTime100ns);

		if (SubmitInitialBuffer(voice, isLoop)) {

			StartStreaming(voice);

			return true;

		}

		return false;

	}

	AudioHandle AudioManager::PlayStreaming(const size_t soundIndex, const bool isLoop, const long long startTime100ns) {
		assert(soundIndex < soundDataStorage_.size());

		// 空きボイスを探す（ストリーミング用プールから）
		size_t voiceIndex = FindFreeStreamingVoice();

		// IDの発行
		size_t currentPlayId = nextPlayId_++;

		// データの初期準備
		auto& streamingVoice = streamingSourceVoicePool_[voiceIndex];
		streamingVoice->playId = currentPlayId;
		streamingVoice->soundIndex = soundIndex;

		// ストリーミング開始処理
		// ファイルを開き、最初のバッファをSubmitしてStart()する専用の関数
		bool isSuccess = InitializeStreaming(voiceIndex, soundIndex, isLoop, startTime100ns);

		// 5. ハンドルを返す
		return AudioHandle{
			.soundIndex = soundIndex,
			.voiceIndex = voiceIndex,
			.isStreaming = true,
			.playId = currentPlayId,
			.isSuccess = isSuccess
		};
	}

	AudioHandle AudioManager::Play(const size_t soundIndex, const bool isLoop, const long long startTime100ns) {

		auto& data = soundDataStorage_[soundIndex];

		if (data->isSuitableStreaming) {

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
				sSrcVoice->playId = 65536;

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

	bool AudioManager::IsPlaying(const AudioHandle& handle) {

		if (handle.isStreaming) {

			auto& sVoice = streamingSourceVoicePool_[handle.voiceIndex];

			if (sVoice && sVoice->playId == handle.playId) {

				return sVoice->state == VoiceState::Playing;

			}

		} else {

			auto& voice = sourceVoicePool_[handle.voiceIndex];

			if (voice && voice->playId == handle.playId) {

				return voice->state == VoiceState::Playing;

			}

		}

		return false;

	}

}