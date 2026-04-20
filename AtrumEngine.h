#pragma once

#include <cstdint>
#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include "Vector4.h"

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


	// DXC補助
	IDxcUtils* dxcUtils_ = nullptr;

	// DXCコンパイラ
	IDxcCompiler3* dxcCompiler_ = nullptr;

	// インクルードハンドラー
	IDxcIncludeHandler* includeHandler_ = nullptr;

	// RootSignature
	ID3D12RootSignature* rootSignature_ = nullptr;

	// RootSignatureの生成結果
	ID3DBlob* signatureBlob_ = nullptr;

	// RootSignatureのエラー結果
	ID3DBlob* errorBlob_ = nullptr;

	// inputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescriptions_[1] = {};

	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

	// BlendState
	D3D12_BLEND_DESC blendDesc_{};

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	// vertexShaderのコンパイル結果
	IDxcBlob* vertexShaderBlob_ = nullptr;

	// pixelShaderのコンパイル結果
	IDxcBlob* pixelShaderBlob_ = nullptr;

	// PSOの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipeLineStateDesc_{};

	// PSO
	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties_{};

	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc_{};

	// 頂点リソース
	ID3D12Resource* vertexResource_ = nullptr;

	// VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// 頂点データ
	Vector4* vertexData_ = nullptr;


	// ビューポート
	D3D12_VIEWPORT viewport_{};

	// シザー矩形
	D3D12_RECT scissorRect_{};

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

	/// <summary>
	/// 初期化処理 DXCの初期化 
	/// </summary>
	void InitDXC();

	/// <summary>
	/// Viewport の設定
	/// </summary>
	void SetUpViewport();

	/// <summary>
	/// シザー矩形の設定
	/// </summary>
	void SetUpScissorRect();

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	void MakeRootSignature();

	/// <summary>
	/// InputLayoutの設定
	/// </summary>
	void SetUpInputLayout();

	/// <summary>
	/// BlendStateの設定
	/// </summary>
	void SetUpBlendState();

	/// <summary>
	/// RasterizerStateの設定
	/// </summary>
	void SetUpRasterizerState();

	/// <summary>
	/// Shaderの準備
	/// </summary>
	void PrepareShader();

public:

	/* 三角形のみ描画可能になっている */

	/// <summary>
	/// PSOの生成
	/// </summary>
	void CreatePSO();

	/// <summary>
	/// VertexResourceの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// VertexBufferViewの作成
	/// </summary>
	void CreateVertexBufferView();

	/// <summary>
	/// VertexResourceにデータを書き込む
	/// </summary>
	void WriteVertexResource();

	/// <summary>
	/// 描画呼び出し(DrawCall)
	/// </summary>
	void DrawCall();


public:

	/// <summary>
	/// Shaderのコンパイル
	/// </summary>
	/// <param name="filePath"> コンパイルするShaderファイルへのパス </param>
	/// <param name="profile"> コンパイルに使用するプロファイル </param>
	/// <returns> コンパイル結果(実行用のバイナリ) </returns>
	IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile
	);

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
	void UpdateWindow();

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