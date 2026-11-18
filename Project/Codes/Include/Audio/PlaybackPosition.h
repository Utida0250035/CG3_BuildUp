#pragma once

#include <cstdint>

namespace Atrum::Audio {

	// XAudio の SamplesPlayed を、ボイスへ載せた PCM のサンプルレートで秒に換算する。
	// sampleRate はファイル本来のレートではなく、SourceVoice が数えている PCM のレート。
	// 0 のときは false を返し、44100 などへ置き換えない。
	bool TryPlaybackSecondsFromSamples(
	    std::uint64_t samplesPlayed,
	    std::uint32_t sampleRate,
	    long long startTime100ns,
	    float& outSeconds);

}
