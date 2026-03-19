// ウィンドウプロシージャ

#include "WindowProcedure.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

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

//