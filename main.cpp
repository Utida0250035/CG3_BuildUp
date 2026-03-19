#include "Log.h"
#include "ConvertString.h"
#include "WindowProcedure.h"
#include <cstdint>
#include <format>
#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ウィンドウクラス
	WNDCLASS wc{};

	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;

	// ウィンドウクラス名
	wc.lpszClassName = L"CG2WindowClass";

	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);

	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録
	RegisterClass(&wc);

	// クライアント領域のサイズ
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	// ウィンドウサイズを表す構造体にクライアント領域を代入
	RECT wrc = { 0, 0, kClientWidth, kClientHeight };

	// クライアント領域を基に実際のサイズ情報をwrcに反映させる
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウの生成
	HWND hwnd = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	// ウィンドウの表示
	ShowWindow(hwnd, SW_SHOW);


	// 出力ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!\n");

	std::wstring texturePath = L"./null.png";

	std::string bufferString = WStringToString(texturePath);

	texturePath = StringToWString(bufferString);

	Log(std::format("enemyHp: {}, texturePath: {}, bufferString: {}\n", 10, WStringToString(texturePath), bufferString));


	MSG msg{};

	while (msg.message != WM_QUIT) {
		// ウィンドウの×ボタンが押されるまでループ

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			// OSへのメッセージを最優先で処理

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		} else {

			/*============== メインループ =================*/

			///
			/// ↓ 更新ここから
			///

			///
			/// ↑更新ここまで
			/// 

			///
			/// ↓描画ここから
			/// 

			///
			/// ↑描画ここまで
			/// 

		}

	}

	return 0;

}