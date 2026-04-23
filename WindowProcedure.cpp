#include "WindowProcedure.h"

#ifdef USE_IMGUI

#include "ImGui.h"

#endif

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

#ifdef USE_IMGUI

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {

		return true;

	}

#endif // USE_IMGUI


	switch (msg) {

		case WM_DESTROY:
			// ウィンドウが破壊された

			// OSにアプリの終了を伝達
			PostQuitMessage(0);

			return 0;

	}

	// 標準メッセージ処理を行なう
	return DefWindowProc(hwnd, msg, wParam, lParam);

}