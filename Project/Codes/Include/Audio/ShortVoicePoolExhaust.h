#pragma once

#include "Audio/AudioHandle.h"

namespace Atrum::Audio {

	// 短尺ボイスが枯渇したときの Handle。Development でも assert せず、その SE だけ鳴らさない。
	inline AudioHandle MakeUnavailableShortVoiceHandle() {
		return {};
	}

}
