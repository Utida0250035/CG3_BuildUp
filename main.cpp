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
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d_%02d-%02d_&02d-%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
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


	// コマンドキューの生成
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));

	// コマンドキューの生成がうまくかなかったら起動できない
	assert(SUCCEEDED(hr));

	// コマンドアロケータの生成
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

	// コマンドアロケータの生成がうまくいかなかったら起動不可
	assert(SUCCEEDED(hr));


	// コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));

	// コマンドリストの生成がうまくいかなかったら起動不可
	assert(SUCCEEDED(hr));


	// スワップチェーンを生成する
	IDXGISwapChain4* swapChain = nullptr;

	// スワップチェーンに渡す情報
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;
	swapChainDesc.Height = kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// コマンドキュー、ウィンドウハンドル、設定を渡してスワップチェーンを生成
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));


	// ディスクリプタヒープの生成
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap));

	// ディスクリプタヒープが生成できなかったら起動不可
	assert(SUCCEEDED(hr));


	// SwapChainからResourceを引っ張る
	ID3D12Resource* swapChainResources[2] = { nullptr };
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));

	// うまくResourceを取得できなければ起動不可
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));


	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// ディスクリプタの先頭を取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	// 1つめのRTV作成
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);

	// 2つめのディスクリプトハンドルを作る
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// 2つめのRTVを作る
	device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);


	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	// 描画先のRTVを設定
	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);

	// 指定色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

	// コマンドリストの内容を確定させる
	hr = commandList->Close();
	assert(SUCCEEDED(hr));


	// GPUにコマンドリストを実行させる
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);

	// GPUとOSに画面の交換を行なうよう通知する
	swapChain->Present(1, 0);

	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));


	LogFile::GetInstance()->Log("Hello ");

	LogFile::GetInstance()->Log("World.\n");

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

			///
			/// ↓終了処理
			/// 

		}

	}

	return 0;

}