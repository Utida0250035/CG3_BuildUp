#include "Audio/Audio.h"
#include "Audio/AudioDecoder.h"
#include "Audio/Streamingsourcevoice.h"

namespace Atrum::Audio {

	bool AudioDecoder::LoadAudio(const std::wstring& filePath, std::vector<uint8_t>& outData, WAVEFORMATEX** outFormat) {
		ComPtr<IMFSourceReader> pReader;
		HRESULT hr = MFCreateSourceReaderFromURL(filePath.c_str(), nullptr, &pReader);

		if (FAILED(hr)) {
			return false;
		}

		// 1. PCM形式の設定（デコーダーに変換を指示）
		ComPtr<IMFMediaType> pNativeType;
		pReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pNativeType);

		ComPtr<IMFMediaType> pPCMType;
		MFCreateMediaType(&pPCMType);
		pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

		// 読み込み時に強制的に 44.1kHz / 16bit / ステレオ に変換する設定
		pPCMType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
		pPCMType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
		pPCMType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
		pPCMType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 4);      // (2ch * 16bit) / 8
		pPCMType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 44100 * 4);

		pReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pPCMType.Get());

		// 2. フォーマット情報の取得
		ComPtr<IMFMediaType> pOutputMediaType;
		pReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pOutputMediaType);
		hr = MFCreateWaveFormatExFromMFMediaType(pOutputMediaType.Get(), outFormat, nullptr);

		assert(SUCCEEDED(hr));

		// 3. 全データを読み込み
		outData.clear();
		while (true) {
			DWORD flags = 0;
			ComPtr<IMFSample> pSample;
			hr = pReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &flags, nullptr, &pSample);

			if (FAILED(hr) || pSample == nullptr) break;

			// バッファの抽出
			ComPtr<IMFMediaBuffer> pBuffer;
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pAudioData = nullptr;
			DWORD cbLength = 0;
			pBuffer->Lock(&pAudioData, nullptr, &cbLength);

			outData.insert(outData.end(), pAudioData, pAudioData + cbLength);

			pBuffer->Unlock();
			if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;
		}

		return true;
	}

	bool AudioDecoder::ReadNextChunk(StreamingSourceVoice& voice, BYTE* pBuffer, DWORD bufferSize, DWORD* pBytesRead) {
		*pBytesRead = 0;
		DWORD cbTotalRead = 0;

		// 1. 前回持ち越したデータのコピー
		if (!voice.remainingData.empty()) {
			DWORD toCopy = std::min((DWORD)voice.remainingData.size(), bufferSize);
			memcpy(pBuffer, voice.remainingData.data(), toCopy);
			cbTotalRead += toCopy;
			voice.remainingData.erase(voice.remainingData.begin(), voice.remainingData.begin() + toCopy);
		}

		// バッファがまだ埋まっていなければReaderから読み込む
		DWORD flags = 0;
		DWORD cbLength = 0;

		while (cbTotalRead < bufferSize) {
			flags = 0;
			ComPtr<IMFSample> pSample;

			HRESULT hr = voice.pReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &flags, nullptr, &pSample);
			if (FAILED(hr) || pSample == nullptr || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) break;

			ComPtr<IMFMediaBuffer> pBufferRaw;
			pSample->ConvertToContiguousBuffer(&pBufferRaw);

			BYTE* pAudioData = nullptr;
			cbLength = 0;
			pBufferRaw->Lock(&pAudioData, nullptr, &cbLength);

			DWORD remaining = bufferSize - cbTotalRead;
			if (cbLength <= remaining) {
				memcpy(pBuffer + cbTotalRead, pAudioData, cbLength);
				cbTotalRead += cbLength;
			} else {
				memcpy(pBuffer + cbTotalRead, pAudioData, remaining);
				// 収まりきらなかった分を確実に保存
				voice.remainingData.assign(pAudioData + remaining, pAudioData + cbLength);
				cbTotalRead += remaining;
			}
			pBufferRaw->Unlock();
		}

		DWORD alignment = 4;
		DWORD remainder = cbTotalRead % alignment;

		if (remainder != 0) {
			DWORD validBytes = cbTotalRead - remainder;

			// 端数分を退避
			std::vector<BYTE> newRemaining(pBuffer + validBytes, pBuffer + cbTotalRead);

			// 元々あった remainingData と結合
			std::vector<BYTE> combined;
			combined.reserve(voice.remainingData.size() + newRemaining.size());
			combined.insert(combined.end(), voice.remainingData.begin(), voice.remainingData.end());
			combined.insert(combined.end(), newRemaining.begin(), newRemaining.end());
			voice.remainingData = std::move(combined);

			*pBytesRead = validBytes;
		} else {
			*pBytesRead = cbTotalRead;
		}

		return *pBytesRead > 0;
	}

	bool AudioDecoder::Seek(StreamingSourceVoice& voice, LONGLONG targetPos100ns) {

		voice.remainingData.clear();

		// 1. ストリームをターゲット位置へ移動
		// PROPVARIANT で時間を指定 (100ナノ秒単位)
		PROPVARIANT var{};
		var.vt = VT_I8;
		var.hVal.QuadPart = targetPos100ns; // 0 を指定すれば先頭

		[[maybe_unused]] HRESULT hr = voice.pReader->SetCurrentPosition(GUID_NULL, var);
		if (FAILED(hr)) {
			assert(false && "Failed to audio seek");

			return false;
		}

		// 2. リーダーのキャッシュをクリア
		// ストリームのシーク直後、以前のデータがバッファに残っているのを防ぐ
		voice.pReader->Flush(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM));

		// 3. 成功
		return true;

	}

}