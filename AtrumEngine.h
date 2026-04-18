#pragma once

#include <cstdint>
#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class AtrumEngine {

private:

	// ウィンドウクラス
	WNDCLASS wc_{};

	// クライアント領域ヨコサイズ
	int32_t clientWidth_ = 1280;

	// クライアント領域タテサイズ
	int32_t clientHeight_ = 720;

	// ウィンドウサイズ構造体
	RECT wrc_{};

	// ウィンドウハンドル
	HWND hwnd_{};

	// Windows系エラーコード格納
	HRESULT hr_{};

#ifdef _DEBUG

	ID3D12Debug1* debugController_ = nullptr;

#endif

	// DXGI(DirectX Graphics Infrastructure)オブジェクト生成インターフェース
	IDXGIFactory7* dxgiFactory_ = nullptr;

	// 使用するアダプタ用
	IDXGIAdapter4* useAdapter_ = nullptr;

	// デバイス
	ID3D12Device* device_ = nullptr;

	// コマンドキュー
	ID3D12CommandQueue* commandQueue_ = nullptr;

	// コマンドアロケータ(コマンド割り当て担当)
	ID3D12CommandAllocator* commandAllocator_ = nullptr;

	// コマンドリスト
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	// スワップチェーン
	IDXGISwapChain4* swapChain_ = nullptr;

	// スワップチェーンリソース
	ID3D12Resource* swapChainResources_[2] = { nullptr };

	// RTV(Render Target View)ディスクリプタヒープ
	ID3D12DescriptorHeap* rtvDescriptorHeap_ = nullptr;

	// RTVディスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]{};

	// フェンス
	ID3D12Fence* fence_ = nullptr;

	// フェンス値
	uint64_t fenceValue_ = 0;

	// fenceEvent
	HANDLE fenceEvent_{};

	// メッセージ
	MSG msg_{};

	AtrumEngine() = default;

	~AtrumEngine() = default;

	/// <summary>
	/// 初期化処理 ウィンドウ作成
	/// </summary>
	/// <param name="windowLabel"> ウィンドウタイトル </param>
	/// <param name="clientWidth"> ウィンドウの横幅 </param>
	/// <param name="clientHeight"> ウィンドウの縦幅 </param>
	void PrepareWindow(const std::string& windowLabel, const int32_t& clientWidth, const int32_t& clientHeight);

	/// <summary>
	/// 初期化処理 アダプターの選択
	/// </summary>
	void SelectAdapter();

	/// <summary>
	/// 初期化処理 デバイスの作成
	/// </summary>
	void CreateDevice();

	/// <summary>
	/// 初期化処理 エラー抑制 デバッグ用
	/// </summary>
	void ErrorSuppressionDebug();

public:

	/// <summary>
	/// エンジンの初期化
	/// </summary>
	/// <param name="windowLabel"> ウィンドウのタイトル </param>
	/// <param name="clientWidth"> ウィンドウの横幅 </param>
	/// <param name="clientHeight"> ウィンドウの縦幅 </param>
	void Initialize(const std::string& windowLabel, const int32_t& clientWidth, const int32_t& clientHeight);

	/// <summary>
	/// ×ボタンが押されていないかどうか
	/// </summary>
	/// <returns></returns>
	bool IsProcess();

	/// <summary>
	/// OSへのメッセージ処理
	/// </summary>
	/// <returns></returns>
	bool MessageForOs();

	/// <summary>
	/// ウィンドウのクリア
	/// </summary>
	void ClearWindow();

	/// <summary>
	/// エンジンの終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns> AtrumEngineインスタンス </returns>
	static AtrumEngine* GetInstance() {

		static AtrumEngine instance;

		return &instance;

	}

	// 代入演算子の削除
	AtrumEngine operator=(const AtrumEngine& source) = delete;

	// コピーコンストラクタの削除
	AtrumEngine(const AtrumEngine& source) = delete;

};