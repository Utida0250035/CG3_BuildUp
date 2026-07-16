#include "Json/JsonTest.h"
#include "Json/JsonHandler.h"
#include "Math/Vector3.h"
#include <vector>

#ifdef USE_IMGUI

#include "Debug/ImGui.h"

#endif

DEFINE_JSON(Vector3, x, y, z);

DEFINE_JSON(JsonTest::JsonData, name, position, positions, numFloat, numInt, numInt32bit, numUINT32bit);

void JsonTest::JsonData::ImGui(const char* jsonFilePath) {

#ifdef USE_IMGUI

	ImGui::Begin("jsonTest");

	ImGui::InputString("name", name);

	ImGui::DragFloat3("position", &position.x);

	ImGui::Text("positions");

	ImGui::SmallButton("emplaceBack");

	if (ImGui::IsItemActivated()) {

		positions.emplace_back();

	}

	ImGui::SameLine();

	ImGui::SmallButton("popBack");

	if (ImGui::IsItemActivated()) {

		if (!positions.empty()) {

			positions.pop_back();

		}

	}

	uint32_t index = 0;
	std::string label = "position";

	for (auto& pos : positions) {

		ImGui::DragFloat3((label + std::to_string(index)).c_str(), &pos.x);

		ImGui::SameLine();

		ImGui::SmallButton("erase");

		if (ImGui::IsItemActivated()) {

			positions.erase(positions.begin() + index);

		}

		index++;

	}

	ImGui::Text("");

	ImGui::DragFloat("numFloat", &numFloat);

	ImGui::DragInt("numInt", &numInt);

	ImGui::DragInt("numInt32bit", &numInt32bit);

	int buffer = numUINT32bit;

	ImGui::DragInt("numUINT32bit", &buffer);

	numUINT32bit = buffer;

	ImGui::SmallButton("Save");

	if (ImGui::IsItemActivated()) {

		JsonHandler::SaveToFile(jsonFilePath, *this);

	}

	ImGui::SameLine();

	ImGui::SmallButton("Load");

	if (ImGui::IsItemActivated()) {

		*this = JsonHandler::LoadFromFile<JsonData>(jsonFilePath);

	}

	ImGui::End();

#endif

}