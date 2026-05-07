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

#include "Matrix3D.h"
#include "Vector3.h"

#ifdef USE_IMGUI

#include "ImGui.h"

#endif

#include <DirectXTex/DirectXTex.h>

#include "Vector2.h"

#include <wrl/client.h>

#include <vector>

#include <memory>

class AtrumEngine {

public:

	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	struct VertexData {
		Vector4 position;
		Vector2 texCoord;
	};

	struct Texture {

		Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};

		// 使用するSRVディスクリプタの番号
		uint32_t srvIndex = 1;
	};

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

	class DescriptorIndexManager {
	private:

		// 次の空きディスクリプタの番号 0はImGui
		uint32_t nextIndex_ = 1;

		// 空いたディスクリプタの番号
		std::vector<uint32_t> freeIndices_{};

	public:

		DescriptorIndexManager() = default;
		~DescriptorIndexManager() = default;

		uint32_t AllocateIndex() {

			if (freeIndices_.empty()) {

				auto index = nextIndex_;

				nextIndex_++;

				return index;

			}

			uint32_t index = freeIndices_.back();

			freeIndices_.pop_back();

			return index;

		}

		void Free(const uint32_t index) {

			freeIndices_.push_back(index);

		}

	};

	// SRV(Shader Resource View)ディスクリプタヒープ
	ID3D12DescriptorHeap* srvDescriptorHeap_ = nullptr;

	// SRVディスクリプタ番号管理
	std::unique_ptr<DescriptorIndexManager> srvDescriptorIndexManager_ = nullptr;

	// SRVハンドルサイズ
	uint32_t srvHandleSize_ = 0;

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

	// InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescriptions_[2]{};

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

	// 頂点リソース
	ID3D12Resource* vertexResource_ = nullptr;

	// VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// 頂点データ
	VertexData* vertexData_ = nullptr;

	// MaterialResource
	ID3D12Resource* materialResource_ = nullptr;

	// MaterialData 色データRGBA
	Vector4* materialData_ = nullptr;

	// WvpResource
	ID3D12Resource* wvpResource_ = nullptr;

	// WvpData 描画座標データ
	Matrix4x4* wvpData_ = nullptr;

	// Texture
	std::vector<Texture> textures_{};

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};

	// ビューポート
	D3D12_VIEWPORT viewport_{};

	// シザー矩形
	D3D12_RECT scissorRect_{};

	// 次フレームまでのカウント
	float countForNextFrame_ = 0.0f;

	// fps(フレーム/s)
	float secondsPerFrame_ = 0.0f;

	// メッセージ
	MSG msg_{};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	AtrumEngine() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
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

	/// <summary>
	/// BufferResource作成
	/// </summary>
	/// <param name="device"> デバイス </param>
	/// <param name="sizeInBytes"> Resourceのサイズ </param>
	/// <returns> Resource </returns>
	ID3D12Resource* CreateBufferResource(size_t sizeInBytes);

	/// <summary>
	/// DescriptorHeap作成
	/// </summary>
	/// <param name="heapType"> Heapの種類 </param>
	/// <param name="descriptorsNum"> Descriptorの数 </param>
	/// <param name="shaderVisible"> Shaderに使用するか </param>
	/// <returns></returns>
	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT descriptorsNum, bool shaderVisible);

	/// <summary>
	/// MaterialResourceの作成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// WvpResource(TransformationMatrix用のリソース)の作成
	/// </summary>
	void CreateWvpResource();

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
	/// Materialにデータ(色)を書き込む
	/// </summary>
	/// <param name="color"> 色(RGBA) 各値0.0fから1.0f </param>
	void SetMaterialData(const Vector4& color);

	/// <summary>
	/// Wvpにデータ(WorldMatrix)を書き込む
	/// </summary>
	/// <param name="wvp"> WorldMatrix </param>
	void SetWvpData(const Matrix4x4& wvp);

	/// <summary>
	/// 描画呼び出し(DrawCall)
	/// </summary>
	void DrawTriangleCall(const uint32_t& textureIndex);

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

	void SetFps(const int32_t& fps);

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

private:

	/// <summary>
	/// OSへのメッセージ処理
	/// </summary>
	/// <returns></returns>
	bool MessageForOs();

	/// <summary>
	/// 次フレーム待ちか
	/// </summary>
	/// <returns></returns>
	bool IsWaitForFrame();

public:

	/// <summary>
	/// フレーム実行の可否
	/// </summary>
	/// <returns> フレーム実行フラグ </returns>
	bool IsExecuteFrame();

#ifdef USE_IMGUI

	/// <summary>
	/// ImGuiにフレーム開始を通知
	/// </summary>
	void ImGuiNewFrame();

	/// <summary>
	/// ImGuiの内部コマンド生成
	/// </summary>
	void ImGuiRender();

#endif

	/// <summary>
	/// 描画処理(前)
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画処理(後)
	/// </summary>
	void PostDraw();

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

	/// <summary>
	/// ワールド行列の作成
	/// </summary>
	/// <param name="transform"> Transform </param>
	/// <returns> ワールド行列 </returns>
	Matrix4x4 CreateWorldMatrix(const Transform& transform);

private:

	/// <summary>
	/// Textureデータの読み込み
	/// </summary>
	/// <param name="filePath"> ファイルパス </param>
	/// <returns> MipMap付きデータ </returns>
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	/// <summary>
	/// TextureResourceの作成
	/// </summary>
	/// <param name="metaData"></param>
	/// <returns></returns>
	ID3D12Resource* CreateTextureResource(const DirectX::TexMetadata& metaData);

	ID3D12Resource* CreateIntermediateResource(ID3D12Resource* texture);

	/// <summary>
	/// textureResourceにデータを転送する
	/// </summary>
	/// <param name="texture"> テクスチャポインタ </param>
	/// <param name="mipImages"> MipMap付データ </param>
	/// <param name="intermediateResource"> 中間リソース </param>
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Resource* intermediateResource);

	/// <summary>
	/// ShaderResourceViewの作成
	/// </summary>
	/// <param name="metaData"> Meta情報 </param>
	void MakeShaderResourceView(Texture& texture, const DirectX::TexMetadata& metaData);

public:

	/// <summary>
	/// テクスチャの取得
	/// </summary>
	/// <param name="filePath"> テクスチャのファイルパス </param>
	/// <returns> テクスチャ番号 </returns>
	uint32_t GetTexture(const std::string& filePath);

	/// <summary>
	/// テクスチャ取得 改良版
	/// </summary>
	/// <param name="filePath"> テクスチャのファイルパス </param>
	/// <returns> テクスチャ番号 </returns>
	uint32_t GetTextureAdvanced(const std::string& filePath);

	/// <summary>
	/// 三角形の描画
	/// </summary>
	/// <param name="textureIndex"> テクスチャ番号 </param>
	void DrawTriangle(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& triangleTransform, const Transform& cameraTransform);

};

void LeakCheck();