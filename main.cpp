#include "ConvertString.h"
#include "Log.h"
#include "WindowProcedure.h"
#include <cstdint>
#include <format>
#include <Windows.h>

#include <cassert>
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")

#include <strsafe.h>

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {

	// Dumpsフォルダを作成
	CreateDirectory(L"./Dumps", nullptr);

	// 現在時刻を名前に入れたファイルをDumpsフォルダ以下に作成
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d_%02d-%02d_&02d-%02d-%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	// processId(このexeのId)とクラッシュ(例外)の発生した、threadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();

	// 設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION miniDumpInformation{ 0 };
	miniDumpInformation.ThreadId = threadId;
	miniDumpInformation.ExceptionPointers = exception;
	miniDumpInformation.ClientPointers = TRUE;

	// Dumpを出力(MiniDumpNormalフラグで最低限の情報を出力させるようにする)
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &miniDumpInformation, nullptr, nullptr);

	/* 他に関連付けられているSEH例外ハンドラがあれば記述を追加する */
	
	return EXCEPTION_EXECUTE_HANDLER;

}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// SEH例外が補足されなかった場合(Unhandled)に補足する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

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


	// ログ出力ファイルクラスの初期化
	LogFile::GetInstance()->Initialize();

	// ログ出力
	LogFile::GetInstance()->Log("Hello,DirectX!\n");


	std::wstring texturePath = L"./null.png";

	std::string bufferString = WStringToString(texturePath);

	texturePath = StringToWString(bufferString);

	Log(std::format("enemyHp: {}, texturePath: {}, bufferString: {}\n", 10, WStringToString(texturePath), bufferString));

	IDXGIFactory7* dxgiFactory = nullptr;

	// HRESULTはWindows系のエラーコード
	// 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));

	/*
	初期化の根本的な部分でエラーが出た場合は
	プログラムの間違いか修正不可能である場合が多い
	*/
	assert(SUCCEEDED(hr));

	// 使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter = nullptr;

	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		// パフォーマンスが良い順にアダプタのリストを出させる 

		// アダプターの情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);

		// アダプターの情報が取得できない場合はエラー
		assert(SUCCEEDED(hr));

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// ソフトウェアアダプタでなければ採用
			
			// 採用したアダプタの情報をログに出力
			Log(WStringToString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));

			break;

		}

		// 次のアダプタへ
		useAdapter = nullptr;

	}

	// 適切なアダプターが見当たらない場合は起動不可
	assert(useAdapter != nullptr);


	ID3D12Device* device = nullptr;

	// 機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	};

	const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };

	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 機能レベルが高い順に、生成できるか試していく

		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));

		if (SUCCEEDED(hr)) {
			// 指定した機能レベルでデバイスが生成できた

			// ログ出力
			Log(std::format("FeatureLevel: {}\n", featureLevelStrings[i]));

			break;

		}

	}

	// デバイスの生成が成功しなかった場合は実行不可
	assert(device != nullptr);

	// 初期化完了のログを出す
	Log("Complete create D3D12Device!!!\n");

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