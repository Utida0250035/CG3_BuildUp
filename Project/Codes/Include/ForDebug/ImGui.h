#pragma once

#ifdef USE_IMGUI

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace ImGui{

	inline bool InputString(const char* label, std::string& str) {

		if (str.empty()) {

			str.reserve(16);

		}

		return ImGui::InputText(label, &str);

	}

	constexpr ImVec2 kChildSize = ImVec2(0.0f, 0.0f);
	constexpr ImGuiChildFlags kChildFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;

}

#endif