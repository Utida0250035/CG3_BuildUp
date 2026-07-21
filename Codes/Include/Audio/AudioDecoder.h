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

    struct StreamingSourceVoice;

    class AudioDecoder {

    private:

        template<typename T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;

    public:

        static void Initialize() {

            [[maybe_unused]] HRESULT hr = MFStartup(MF_VERSION);
            assert(SUCCEEDED(hr));

        }

        // MP3をPCMデータとして読み込む
        static bool LoadAudio(const std::wstring& filePath, std::vector<uint8_t>& outData, WAVEFORMATEX** outFormat) ;

        // ストリーミング用：Readerからデータを取り出して渡されたバッファに詰める
        static bool ReadNextChunk(StreamingSourceVoice& voice, BYTE* pBuffer, DWORD bufferSize, DWORD* pBytesRead);

        static bool Seek(StreamingSourceVoice& voice, LONGLONG targetPos100ns);

        static void Finalize() {

            MFShutdown();

        }

    };

}