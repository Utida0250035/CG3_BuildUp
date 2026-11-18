#include "Audio/PlaybackPosition.h"

namespace Atrum::Audio {

	namespace {

		// 100ナノ秒単位の時刻を秒へ換算する分母。
		constexpr long long kHundredNanosecondsPerSecond = 10000000;

	}

	bool TryPlaybackSecondsFromSamples(
	    const std::uint64_t samplesPlayed,
	    const std::uint32_t sampleRate,
	    const long long startTime100ns,
	    float& outSeconds) {

		if (sampleRate == 0) {
			return false;
		}

		const float sinceVoiceStart =
		    static_cast<float>(samplesPlayed) / static_cast<float>(sampleRate);
		const float startSeconds =
		    static_cast<float>(startTime100ns) /
		    static_cast<float>(kHundredNanosecondsPerSecond);
		outSeconds = startSeconds + sinceVoiceStart;
		return true;

	}

}
