#include "Audio/Audio.h"
#include "Audio/AudioDecoder.h"
#include "Audio/StreamingSourceVoice.h"
#include "Audio/StreamingVoiceCallBack.h"
#include "File/FileSize.h"
#include "System/Hash64.h"
#include "String/ConvertString.h"
#include <cassert>
#include <filesystem>
#include <fstream>

#include <mfreadwrite.h>
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace Atrum::Audio {

namespace {

inline constexpr size_t kBgmStreamingVoiceIndex = 0;

} // namespace

	std::unique_ptr<AudioManager> AudioManager::instance_ = nullptr;

	AudioManager::~AudioManager() {

		sourceVoicePool_.clear();
		streamingSourceVoicePool_.clear();

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

			if (streamingVoice->state == VoiceState::Streaming) {

				if (RefillBuffer(*streamingVoice)) {

					streamingVoice->state = VoiceState::Playing;

				} else {

					streamingVoice->state = VoiceState::Waiting;

				}

			}

			if (streamingVoice->state == VoiceState::Waiting) {

				if (streamingVoice->isLoop &&
				    TryRecoverLoopingStreamingVoice(*streamingVoice)) {
					continue;
				}

				streamingVoice->pVoice->Stop();
				streamingVoice->pVoice->FlushSourceBuffers();

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

		const size_t index = soundDataStorage_.size() - 1;
		soundIndexMap_.emplace(Hash64(filePath), index);
		return index;

	}

	size_t AudioManager::LoadShort(const char* filePath) {

		std::vector<uint8_t> pBuffer;
		WAVEFORMATEX* wfEx = nullptr;

		const bool result =
		    AudioDecoder::LoadAudio(StringToWString(filePath), pBuffer, &wfEx);
		if (!result) {
			CoTaskMemFree(wfEx);
			return static_cast<size_t>(-1);
		}

		this->AddSource(*wfEx, std::move(pBuffer), static_cast<UINT>(pBuffer.size()), soundDataStorage_.size(), filePath);

		CoTaskMemFree(wfEx);

		return soundDataStorage_.size() - 1;

	}

	size_t AudioManager::Load(const std::string& filePath) {

		if (!std::filesystem::exists(filePath)) {
			return static_cast<size_t>(-1);
		}

		const uint64_t pathHash = Hash64(filePath);
		if (const auto found = soundIndexMap_.find(pathHash);
		    found != soundIndexMap_.end()) {
			return found->second;
		}

		if (IsSuitableStreaming(filePath)) {

			return SetupStreaming(filePath.c_str());

		}

		return LoadShort(filePath.c_str());

	}

	size_t AudioManager::Get(const std::string& filePath) {

		auto search = soundIndexMap_.find(Hash64(filePath));

		assert(search != soundIndexMap_.end());

		return search->second;

	}

	void AudioManager::ResetShortVoice(SourceVoice &voice) {

		if (voice.pVoice != nullptr) {

			voice.pVoice->Stop();
			voice.pVoice->FlushSourceBuffers();

		}

		voice.state = VoiceState::Stopped;
		voice.playId = 0;
		voice.isLoopPlayback = false;
		voice.loopSoundIndex = static_cast<size_t>(-1);

	}

	void AudioManager::ReleaseWaitingShortVoices() {

		for (std::unique_ptr<SourceVoice> &sourceVoice : sourceVoicePool_) {

			if (sourceVoice != nullptr &&
			    sourceVoice->state == VoiceState::Waiting) {
				ResetShortVoice(*sourceVoice);
			}

		}

	}

	void AudioManager::StopAllLoopingShortVoices() {

		for (std::unique_ptr<SourceVoice> &sourceVoice : sourceVoicePool_) {

			if (sourceVoice != nullptr && sourceVoice->isLoopPlayback) {
				ResetShortVoice(*sourceVoice);
			}

		}

	}

	AudioHandle AudioManager::PlayShort(const size_t soundIndex, const bool isLoop) {

		if (soundIndex >= soundDataStorage_.size()) {
			return {};
		}

		auto tryPlayShort = [this, soundIndex, isLoop]() -> AudioHandle {
			HRESULT hr{};

			auto &soundData = soundDataStorage_[soundIndex];

			XAUDIO2_BUFFER buf{};
			buf.pAudioData = soundData->pBuffer.data();
			buf.AudioBytes = soundData->bufferSize;
			buf.Flags = XAUDIO2_END_OF_STREAM;
			buf.LoopCount = isLoop ? XAUDIO2_LOOP_INFINITE : 0;

			size_t voiceIndex = 0;

			AudioHandle handle{};
			handle.isStreaming = false;
			handle.soundIndex = soundIndex;

			for (std::unique_ptr<SourceVoice> &pSourceVoice : sourceVoicePool_) {

				if (pSourceVoice != nullptr &&
				    pSourceVoice->state == VoiceState::Stopped) {

					hr = pSourceVoice->pVoice->SubmitSourceBuffer(&buf);
					if (FAILED(hr)) {
						return {};
					}

					hr = pSourceVoice->pVoice->Start();
					if (FAILED(hr)) {
						pSourceVoice->pVoice->FlushSourceBuffers();
						return {};
					}

					pSourceVoice->state = VoiceState::Playing;
					pSourceVoice->isLoopPlayback = isLoop;
					pSourceVoice->loopSoundIndex =
					    isLoop ? soundIndex : static_cast<size_t>(-1);

					handle.voiceIndex = voiceIndex;
					handle.playId = nextPlayId_++;
					handle.isSuccess = true;
					pSourceVoice->playId = handle.playId;

					return handle;
				}

				++voiceIndex;
			}

			return {};
		};

		if (AudioHandle handle = tryPlayShort(); handle.isSuccess) {
			return handle;
		}

		ReleaseWaitingShortVoices();

		if (AudioHandle handle = tryPlayShort(); handle.isSuccess) {
			return handle;
		}

		assert(false && "no empty sourceVoice");

		return {};

	}

	void AudioManager::ResetStreamingVoice(StreamingSourceVoice &voice) {

		if (voice.pVoice != nullptr) {

			voice.pVoice->Stop();
			voice.pVoice->FlushSourceBuffers();

		}

		voice.pReader.Reset();
		voice.remainingData.clear();
		voice.state = VoiceState::Stopped;
		voice.playId = 65536;
		voice.isLoop = false;
		voice.nextBufferIndex = 0;

	}

	void AudioManager::StopAllStreamingVoices() {

		for (std::unique_ptr<StreamingSourceVoice> &streamingVoice :
		     streamingSourceVoicePool_) {

			if (streamingVoice != nullptr) {
				ResetStreamingVoice(*streamingVoice);
			}

		}

	}

	size_t AudioManager::AcquireStreamingVoiceIndex() {

		const size_t poolSize = streamingSourceVoicePool_.size();
		if (poolSize <= 1) {
			return static_cast<size_t>(-1);
		}

		// ボイス 0 は PlaySceneBgm 専用。PlayStreaming では触らない
		const size_t kFirstGeneralStreamingVoiceIndex = 1;

		auto findStoppedVoice = [this](const size_t beginIndex,
		                               const size_t endIndex) -> size_t {
			for (size_t voiceIndex = beginIndex; voiceIndex < endIndex;
			     ++voiceIndex) {
				const std::unique_ptr<StreamingSourceVoice> &streamingVoice =
				    streamingSourceVoicePool_[voiceIndex];
				if (streamingVoice != nullptr &&
				    streamingVoice->state == VoiceState::Stopped) {
					return voiceIndex;
				}
			}

			return static_cast<size_t>(-1);
		};

		if (const size_t voiceIndex = findStoppedVoice(
		        kFirstGeneralStreamingVoiceIndex, poolSize);
		    voiceIndex != static_cast<size_t>(-1)) {
			return voiceIndex;
		}

		for (size_t voiceIndex = kFirstGeneralStreamingVoiceIndex;
		     voiceIndex < poolSize; ++voiceIndex) {
			if (streamingSourceVoicePool_[voiceIndex] != nullptr) {
				ResetStreamingVoice(*streamingSourceVoicePool_[voiceIndex]);
			}
		}

		if (const size_t voiceIndex = findStoppedVoice(
		        kFirstGeneralStreamingVoiceIndex, poolSize);
		    voiceIndex != static_cast<size_t>(-1)) {
			return voiceIndex;
		}

		assert(false && "Streaming voice pool is full");

		return static_cast<size_t>(-1);

	}

	bool AudioManager::PrepareStreamingDecoder(StreamingSourceVoice& voice, const size_t soundIndex, const long long startTime100ns) {

		if (soundIndex >= soundDataStorage_.size()) {
			return false;
		}

		auto& path = soundDataStorage_[soundIndex]->filePath;
		HRESULT hr = MFCreateSourceReaderFromURL(path.c_str(), nullptr, &voice.pReader);
		if (FAILED(hr)) {
			return false;
		}

		ComPtr<IMFMediaType> pPCMType;
		hr = MFCreateMediaType(&pPCMType);
		if (FAILED(hr)) {
			return false;
		}

		pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		pPCMType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
		pPCMType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
		pPCMType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
		pPCMType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 4);
		pPCMType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 44100 * 4);
		hr = voice.pReader->SetCurrentMediaType(
		    static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr,
		    pPCMType.Get());
		if (FAILED(hr)) {
			return false;
		}

		voice.startTime100ns = startTime100ns;

		return AudioDecoder::Seek(voice, startTime100ns);

	}

	bool AudioManager::SubmitInitialBuffer(StreamingSourceVoice& voice, const bool isLoop) {

		// リーダーの設定（必要に応じてオーディオフォーマットの指定など）
		// 通常はデフォルト設定でOK 必要ならここでConfigureSourceReaderを呼ぶ

		voice.nextBufferIndex = 0;

		voice.isLoop = isLoop;

		for (auto& pBuffer : voice.pBuffers) {

			pBuffer.resize(StreamingSourceVoice::kBufferSize);

		}

		return RefillBuffer(voice);

	}

	void AudioManager::StartStreaming(StreamingSourceVoice& voice) {

		// 5. 再生開始
		[[maybe_unused]] HRESULT hr = voice.pVoice->Start();
		assert(SUCCEEDED(hr));

		voice.state = VoiceState::Playing;

	}

	bool AudioManager::InitializeStreaming(const size_t voiceIndex, const size_t soundIndex, const bool isLoop, const long long startTime100ns) {

		auto& voice = *streamingSourceVoicePool_[voiceIndex];

		assert(voice.state == VoiceState::Stopped);

		if (!PrepareStreamingDecoder(voice, soundIndex, startTime100ns)) {
			ResetStreamingVoice(voice);
			return false;
		}

		if (SubmitInitialBuffer(voice, isLoop)) {

			StartStreaming(voice);

			// 次のバッファの用意
			RefillBuffer(voice);

			return true;

		}

		ResetStreamingVoice(voice);

		return false;

	}

	AudioHandle AudioManager::PlayStreaming(const size_t soundIndex, const bool isLoop, const long long startTime100ns) {
		assert(soundIndex < soundDataStorage_.size());

		const size_t voiceIndex = AcquireStreamingVoiceIndex();
		if (voiceIndex == static_cast<size_t>(-1) ||
		    voiceIndex >= streamingSourceVoicePool_.size()) {
			return {};
		}

		// IDの発行
		size_t currentPlayId = nextPlayId_++;

		// データの初期準備
		auto& streamingVoice = streamingSourceVoicePool_[voiceIndex];
		streamingVoice->playId = currentPlayId;
		streamingVoice->soundIndex = soundIndex;

		// ストリーミング開始処理
		// ファイルを開き、最初のバッファをSubmitしてStart()する専用の関数
		bool isSuccess = InitializeStreaming(voiceIndex, soundIndex, isLoop, startTime100ns);

		if (!isSuccess) {
			streamingVoice->playId = 65536;
		}

		// 5. ハンドルを返す
		return AudioHandle{
			.soundIndex = soundIndex,
			.voiceIndex = voiceIndex,
			.isStreaming = true,
			.playId = currentPlayId,
			.isSuccess = isSuccess
		};
	}

	bool AudioManager::RefillBuffer(StreamingSourceVoice& voice) {

		uint64_t currentBufferIndex = voice.nextBufferIndex;

		voice.nextBufferIndex = (currentBufferIndex + 1) % StreamingSourceVoice::kBufferCount;

		DWORD bytesRead = 0;

		bool isSuccess = AudioDecoder::ReadNextChunk(voice, voice.pBuffers[currentBufferIndex].data(), StreamingSourceVoice::kBufferSize, &bytesRead);

		if (isSuccess && bytesRead > 0) {

			XAUDIO2_BUFFER buf{};
			buf.AudioBytes = bytesRead;
			buf.pAudioData = voice.pBuffers[currentBufferIndex].data();
			// pContext には parentVoice 自身を入れておくことが一般的
			buf.pContext = reinterpret_cast<void*>(static_cast<uintptr_t>(currentBufferIndex));

			if (bytesRead < StreamingSourceVoice::kBufferSize) {
				std::fill(voice.pBuffers[currentBufferIndex].begin() + bytesRead,
					voice.pBuffers[currentBufferIndex].end(), static_cast<BYTE>(0));
			}

			voice.pVoice->SubmitSourceBuffer(&buf);

			return true;

		} else {

			if (voice.isLoop) {

				AudioDecoder::Seek(voice, voice.startTime100ns);

				bool isSuccess1 = AudioDecoder::ReadNextChunk(voice, voice.pBuffers[currentBufferIndex].data(), StreamingSourceVoice::kBufferSize, &bytesRead);

				if (isSuccess1 && bytesRead > 0) {
					XAUDIO2_BUFFER buf{};
					buf.AudioBytes = bytesRead;
					buf.pAudioData = voice.pBuffers[currentBufferIndex].data();
					buf.pContext = reinterpret_cast<void*>(static_cast<uintptr_t>(currentBufferIndex));

					if (bytesRead < StreamingSourceVoice::kBufferSize) {
						std::fill(voice.pBuffers[currentBufferIndex].begin() + bytesRead,
							voice.pBuffers[currentBufferIndex].end(), static_cast<BYTE>(0));
					}

					voice.pVoice->SubmitSourceBuffer(&buf);

					return true;

				}

			}

			return false;

		}

	}

	bool AudioManager::TryRecoverLoopingStreamingVoice(StreamingSourceVoice &voice) {

		if (!voice.isLoop || voice.pVoice == nullptr) {
			return false;
		}

		AudioDecoder::Seek(voice, voice.startTime100ns);

		if (!RefillBuffer(voice)) {
			return false;
		}

		const HRESULT hr = voice.pVoice->Start();
		if (FAILED(hr)) {
			return false;
		}

		voice.state = VoiceState::Playing;
		return true;

	}

	AudioHandle AudioManager::PlaySceneBgm(const size_t soundIndex) {

		StopAllStreamingVoices();
		StopAllLoopingShortVoices();

		if (soundIndex >= soundDataStorage_.size()) {
			return {};
		}

		auto &data = soundDataStorage_[soundIndex];

		if (data->isSuitableStreaming) {

			if (kBgmStreamingVoiceIndex >= streamingSourceVoicePool_.size()) {
				return {};
			}

			auto &streamingVoice = *streamingSourceVoicePool_[kBgmStreamingVoiceIndex];
			ResetStreamingVoice(streamingVoice);

			const uint64_t currentPlayId = nextPlayId_++;
			streamingVoice.playId = currentPlayId;
			streamingVoice.soundIndex = soundIndex;

			const bool isSuccess = InitializeStreaming(
			    kBgmStreamingVoiceIndex, soundIndex, true, 0);

			if (!isSuccess) {
				streamingVoice.playId = 65536;
			}

			return AudioHandle{
			    .soundIndex = soundIndex,
			    .voiceIndex = kBgmStreamingVoiceIndex,
			    .isStreaming = true,
			    .playId = currentPlayId,
			    .isSuccess = isSuccess,
			};
		}

		AudioHandle handle = PlayShort(soundIndex, true);
		if (handle.isSuccess) {
			return handle;
		}

		ReleaseWaitingShortVoices();
		StopAllLoopingShortVoices();
		return PlayShort(soundIndex, true);

	}

	AudioHandle AudioManager::Play(const size_t soundIndex, const bool isLoop, const long long startTime100ns) {

		if (soundIndex >= soundDataStorage_.size()) {
			return {};
		}

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

			if (handle.voiceIndex >= streamingSourceVoicePool_.size()) {
				return;
			}

			auto& sSrcVoice = streamingSourceVoicePool_[handle.voiceIndex];

			if (sSrcVoice && sSrcVoice->playId == handle.playId) {
				ResetStreamingVoice(*sSrcVoice);
			}

			return;

		}

		if (handle.voiceIndex >= sourceVoicePool_.size()) {
			return;
		}

		auto& srcVoice = sourceVoicePool_[handle.voiceIndex];

		if (srcVoice && srcVoice->playId == handle.playId) {
			ResetShortVoice(*srcVoice);
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