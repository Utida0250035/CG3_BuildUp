#pragma once

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <vector>
#include <wrl/client.h>
#include <string>
#include <cassert>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace Atrum::Audio {

    using Microsoft::WRL::ComPtr;

    class AudioDecoder {
    public:

        static void Initialize() {

            [[maybe_unused]] HRESULT hr = MFStartup(MF_VERSION);
            assert(SUCCEEDED(hr));

        }

        // MP3をPCMデータとして読み込む
        static bool LoadAudio(const std::wstring& filePath, std::vector<uint8_t>& outData, WAVEFORMATEX** outFormat) {
            ComPtr<IMFSourceReader> pReader;
            HRESULT hr = MFCreateSourceReaderFromURL(filePath.c_str(), nullptr, &pReader);
            if (FAILED(hr)) return false;

            // 1. PCM形式の設定（デコーダーに変換を指示）
            ComPtr<IMFMediaType> pNativeType;
            pReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pNativeType);

            ComPtr<IMFMediaType> pPCMType;
            MFCreateMediaType(&pPCMType);
            pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
            pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
            pReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pPCMType.Get());

            // 2. フォーマット情報の取得
            ComPtr<IMFMediaType> pOutputMediaType;
            pReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pOutputMediaType);
            hr = MFCreateWaveFormatExFromMFMediaType(pOutputMediaType.Get(), outFormat, nullptr);

            assert(SUCCEEDED(hr));

            // 読み込み時に強制的に 44.1kHz / 16bit / ステレオ に変換する設定
            pPCMType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, 2);
            pPCMType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, 44100);
            pPCMType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
            pPCMType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, 4);      // (2ch * 16bit) / 8
            pPCMType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 44100 * 4);

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

        static void Finalize() {

            MFShutdown();

        }

    };

}