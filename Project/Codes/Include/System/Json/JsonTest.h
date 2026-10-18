#pragma once

#include "Math/Vector3.h"
#include <memory>
#include <string>
#include <vector>

namespace Atrum::Json {

	struct JsonTest {

		struct JsonData {

			std::string name{};
			Math::Vector3 position{};
			std::vector<Math::Vector3> positions{};
			float numFloat = 0.0f;
			int numInt = 0;
			int32_t numInt32bit = 0;
			uint32_t numUINT32bit = 0;

			void ImGui(const char* jsonFilePath);

		};

		JsonData jsonVal;

		std::unique_ptr<Math::Vector3> positionUniquePtr = nullptr;
		Math::Vector3* positionPtr = nullptr;

	};

}