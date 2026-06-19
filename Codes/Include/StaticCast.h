#pragma once

#include <cstdint>

namespace cast {

	inline float Float(const uint32_t num) {

		return static_cast<float>(num);

	}

	inline float Float(const int32_t num) {

		return static_cast<float>(num);

	}

	inline float Float(const uint64_t num) {

		return static_cast<float>(num);

	}

	inline float Float(const int64_t num) {

		return static_cast<float>(num);

	}

	inline float Float(const double num) {

		return static_cast<float>(num);

	}

	inline float Float(const SHORT num) {

		return static_cast<float>(num);

	}

	inline float Float(const LONG num) {

		return static_cast<float>(num);

	}

	inline short Short(const int num) {

		return static_cast<short>(num);

	}

}