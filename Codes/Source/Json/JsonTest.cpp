#include "Math/Vector3.h"

#include "Json/JsonTest.h"
#include "Json/JsonHandler.h"

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

#ifdef DEVELOPMENT

namespace Atrum::Math {
	// nlohmann/json がこの関数を自動的に見つけます
	static void to_json(nlohmann::json& j, const Vector3& v) {
		j = nlohmann::json{ {"x", v.x}, {"y", v.y}, {"z", v.z} };
	}

	static void from_json(const nlohmann::json& j, Vector3& v) {
		// v = ... という代入演算子を通さない書き方なので確実に動きます
		j.at("x").get_to(v.x);
		j.at("y").get_to(v.y);
		j.at("z").get_to(v.z);
	}
}

#endif

namespace Atrum::Json {

#ifdef DEVELOPMENT

	DEFINE_JSON(Atrum::Json::JsonTest::JsonData, name, position, positions, numFloat, numInt, numInt32bit, numUINT32bit);

#endif

	void JsonTest::JsonData::ImGui([[maybe_unused]]const char* jsonFilePath) {

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

}