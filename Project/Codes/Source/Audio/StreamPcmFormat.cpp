#include "Audio/StreamPcmFormat.h"

#include <mfapi.h>
#include <mfidl.h>
#include <wrl/client.h>

namespace Atrum::Audio {

	StreamPcmFormat BuildStreamPcmFormat(IMFMediaType* nativeType) {

		StreamPcmFormat format{};
		if (nativeType == nullptr) {
			return format;
		}

		UINT32 sampleRate = 0;
		if (FAILED(nativeType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sampleRate)) ||
		    sampleRate == 0) {
			return format;
		}

		format.wf.wFormatTag = WAVE_FORMAT_PCM;
		format.wf.nChannels = 2;
		format.wf.nSamplesPerSec = sampleRate;
		format.wf.wBitsPerSample = 16;
		format.wf.nBlockAlign =
		    static_cast<WORD>((format.wf.nChannels * format.wf.wBitsPerSample) / 8);
		format.wf.nAvgBytesPerSec = format.wf.nSamplesPerSec * format.wf.nBlockAlign;
		format.wf.cbSize = 0;
		format.valid = true;
		return format;

	}

	StreamPcmFormat ProbeStreamPcmFormatFromUrl(const wchar_t* url) {

		StreamPcmFormat format{};
		if (url == nullptr) {
			return format;
		}

		Microsoft::WRL::ComPtr<IMFSourceReader> reader;
		const HRESULT hrOpen =
		    MFCreateSourceReaderFromURL(url, nullptr, &reader);
		if (FAILED(hrOpen)) {
			return format;
		}

		Microsoft::WRL::ComPtr<IMFMediaType> nativeType;
		const HRESULT hrType = reader->GetCurrentMediaType(
		    static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &nativeType);
		if (FAILED(hrType)) {
			return format;
		}

		return BuildStreamPcmFormat(nativeType.Get());

	}

	void ApplyStreamPcmMediaType(IMFMediaType* pcmType, const WAVEFORMATEX& format) {

		if (pcmType == nullptr) {
			return;
		}

		pcmType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
		pcmType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
		pcmType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, format.nChannels);
		pcmType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, format.nSamplesPerSec);
		pcmType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, format.wBitsPerSample);
		pcmType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, format.nBlockAlign);
		pcmType->SetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, format.nAvgBytesPerSec);

	}

}
