#pragma once

#include <windows.h>

struct IMFMediaType;

namespace Atrum::Audio {

	// ストリームの出力 PCM。サンプルレートは音源のネイティブ値、形はステレオ 16bit。
	struct StreamPcmFormat {
		WAVEFORMATEX wf{};
		bool valid = false;
	};

	// nativeType のサンプルレートで PCM を組む。レートが取れなければ valid は false。
	StreamPcmFormat BuildStreamPcmFormat(IMFMediaType* nativeType);

	void ApplyStreamPcmMediaType(IMFMediaType* pcmType, const WAVEFORMATEX& format);

}
