#pragma once

#include "Math/Vector3.h"
#include <memory>
#include <string>
#include <vector>

struct JsonTest {

	struct JsonData {

		std::string name{};
		Vector3 position{};
		std::vector<Vector3> positions{};
		float numFloat = 0.0f;
		int numInt = 0;
		int32_t numInt32bit = 0;
		uint32_t numUINT32bit = 0;

		void ImGui(const char* jsonFilePath);

	};

	JsonData jsonVal;

	std::unique_ptr<Vector3> positionUniquePtr = nullptr;
	Vector3* positionPtr = nullptr;

};