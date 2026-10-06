#pragma once

#include <cstdint>

namespace Atrum::Audio {

	struct AudioHandle {

		size_t soundIndex = 0;
		size_t voiceIndex = 0;
		bool isStreaming = false;
		uint64_t playId = 0;
		bool isSuccess = false;

		explicit operator bool() const {
			return isSuccess;
		}

	};

}