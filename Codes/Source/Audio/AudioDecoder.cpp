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

		// 前回持ち越したデータがあればそれを優先してコピーする
		if (!voice.remainingData.empty()) {
			DWORD toCopy = std::min((DWORD)voice.remainingData.size(), bufferSize);
			memcpy(pBuffer, voice.remainingData.data(), toCopy);

			cbTotalRead += toCopy;

			// コピーした分を削除（ベクタの先頭を削除）
			voice.remainingData.erase(voice.remainingData.begin(), voice.remainingData.begin() + toCopy);
		}

		// IMFSourceReader作成後、読み込みループに入る前に実行
		ComPtr<IMFMediaType> pPCMType;
		MFCreateMediaType(&pPCMType);
		pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		pPCMType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
		pPCMType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
		pPCMType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
		pPCMType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 4); // 2ch * 16bit / 8
		pPCMType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 44100 * 4);

		voice.pReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pPCMType.Get());

		// バッファがまだ埋まっていなければReaderから読み込む
		while (cbTotalRead < bufferSize) {
			DWORD flags = 0;
			ComPtr<IMFSample> pSample;

			HRESULT hr = voice.pReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &flags, nullptr, &pSample);
			if (FAILED(hr) || pSample == nullptr || (flags & MF_SOURCE_READERF_ENDOFSTREAM)) break;

			ComPtr<IMFMediaBuffer> pBufferRaw;
			pSample->ConvertToContiguousBuffer(&pBufferRaw);

			BYTE* pAudioData = nullptr;
			DWORD cbLength = 0;
			pBufferRaw->Lock(&pAudioData, nullptr, &cbLength);

			DWORD remaining = bufferSize - cbTotalRead;
			if (cbLength <= remaining) {
				// 全てコピー可能
				memcpy(pBuffer + cbTotalRead, pAudioData, cbLength);
				cbTotalRead += cbLength;
			} else {
				// バッファに収まりきらない分をコピーし、残りを state.remainingData に退避
				memcpy(pBuffer + cbTotalRead, pAudioData, remaining);

				voice.remainingData.assign(pAudioData + remaining, pAudioData + cbLength);
				cbTotalRead += remaining;
			}

			pBufferRaw->Unlock();
		}

		*pBytesRead = (cbTotalRead / 4) * 4;
		return cbTotalRead > 0;
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