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

#include "DeltaTime.h"
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
#include <array>

#include <memory>
#include <map>

class AtrumEngine final {

private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

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

		ComPtr<ID3D12Resource> resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};

		// 使用するSRVディスクリプタの番号
		uint32_t srvIndex = 1;
	};

private:

	/* Window */

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


	/* エラー処理 */

	// Windowsエラーコード格納
	HRESULT hr_{};


	/* DirectX インターフェース */

	// DXGI(DirectX Graphics Infrastructure)オブジェクト生成インターフェース
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	// 使用するアダプタ用
	ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	// デバイス
	ComPtr<ID3D12Device> device_ = nullptr;

	// コマンドキュー
	ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	// コマンドアロケータの個数
	inline static constexpr uint8_t kFrameCount_ = 2;

	// コマンドアロケータ(コマンド割り当て担当)
	ComPtr<ID3D12CommandAllocator> commandAllocators_[kFrameCount_] = { nullptr };

	// 使用するコマンドアロケータの番号
	uint8_t frameIndex_ = 0;

	// コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;


	/* SwapChain SwapChainResource */

	// スワップチェーン
	ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	// スワップチェーンリソース
	ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };


	class DescriptorIndexManager {
		/* ディスクリプタ管理補助クラス */
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


	/* SRV */

	// SRV(Shader Resource View)ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;

	// SRVディスクリプタ番号管理
	std::unique_ptr<DescriptorIndexManager> srvDescriptorIndexManager_ = nullptr;

	// SRVハンドルサイズ
	uint32_t srvHandleSize_ = 0;


	/* RTV */

	// RTV(Render Target View)ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;

	// RTVディスクリプタハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2]{};


	/* フェンス / フェンスイベント */

	// フェンス
	ComPtr<ID3D12Fence> fence_ = nullptr;

	// フェンス値
	uint64_t fenceValues_[kFrameCount_] = { 0 };

	// 総フェンス値
	uint64_t totalFenceCount_ = 0;

	// fenceEvent
	HANDLE fenceEvent_{};


	/* 中間リソース */

	// フレーム内の中間リソース保存
	std::vector<ComPtr<ID3D12Resource>> temporaryResources_;


	/* DirectX 補助 / コンパイラ 等 */

	// DXC補助
	ComPtr<IDxcUtils> dxcUtils_ = nullptr;

	// DXCコンパイラ
	ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;

	// インクルードハンドラー
	ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;


	/* RootSignature */

	// RootSignature
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	// RootSignatureの生成結果
	ComPtr<ID3DBlob> signatureBlob_ = nullptr;

	// RootSignatureのエラー結果
	ComPtr<ID3DBlob> errorBlob_ = nullptr;


	/* InputLayout */

	// InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescriptions_[2]{};

	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};


	/* 描画State系 */

	// BlendState
	D3D12_BLEND_DESC blendDesc_{};

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	// DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};


	/* VertexShader / PixelShader コンパイル結果 */

	// vertexShaderのコンパイル結果
	ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;

	// pixelShaderのコンパイル結果
	ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;


	/* PSO */

	// PSOの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};

	// PSO
	ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;


	/* Vertex */

	// 頂点リソース
	ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	// VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// 頂点データ
	VertexData* vertexData_ = nullptr;

	// 画面上に描画済みの三角形の数
	uint32_t triangleDrewCount_ = 0;

	// 画面上の三角形の最大描画数
	const uint32_t triangleMaxDrawCount_ = 1024;

	struct WvpData {
		Matrix4x4 data{};
		float padding[48]{};
	};

	struct MaterialData {
		Vector4 data{};
		float padding[60]{};
	};

	/* Material */

	// MaterialResource
	ComPtr<ID3D12Resource> materialResource_ = nullptr;

	// MaterialData 色データRGBA
	MaterialData* materialData_ = nullptr;


	/* WVP */

	// WvpResource
	ComPtr<ID3D12Resource> wvpResource_ = nullptr;

	// WvpData 描画座標データ
	WvpData* wvpData_ = nullptr;


	/* constantBufferCount */

	uint32_t constantBufferCount_ = 0;


	/* depthStencil */

	// DSVディスクリプタヒープ DSV(Depth Stencil View)
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;

	// DepthStencilResource
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;


	/* Sprite用 Vertex */

	// Sprite用のVertexResource
	ComPtr<ID3D12Resource> spriteVertexResource_ = nullptr;

	// Sprite用のVertexBufferView
	D3D12_VERTEX_BUFFER_VIEW spriteVertexBufferView_{};

	// Sprite用 頂点データ
	VertexData* spriteVertexData_ = nullptr;

	uint32_t spriteTriangleDrewCount_;

	const uint32_t spriteTriangleMaxDrawCount_ = 1024;

	/* Sprite用 Material */

	// MaterialResource
	ComPtr<ID3D12Resource> spriteMaterialResource_ = nullptr;

	// MaterialData
	MaterialData* spriteMaterialData_ = nullptr;


	/* Sprite用 Transform */

	// Sprite用のTransformMatrix用のリソース
	ComPtr<ID3D12Resource> spriteTransformationMatrixResource_ = nullptr;

	// Sprite用 Transformデータ
	WvpData* spriteTransformData_ = nullptr;

	/* Sprite用 constantBufferCount */

	uint32_t spriteConstantBufferCount_ = 0;

	/* テクスチャ */

	// Texture番号テーブル
	std::map<std::string, uint32_t> textureIndexTable_{};

	// Texture
	std::vector<Texture> textures_{};


	/* バリア */

	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};


	/* ウィンドウサイズ */

	// ビューポート
	D3D12_VIEWPORT viewport_{};

	// シザー矩形
	D3D12_RECT scissorRect_{};


	/* 時間管理 */

	// 次フレームまでのカウント
	float countForNextFrame_ = 0.0f;

	// fps(フレーム/s)
	float secondsPerFrame_ = 0.0f;

	// 時間差分
	std::unique_ptr<DeltaTime> deltaTimeManager_ = nullptr;


	/* OSとのやり取り */

	// メッセージ
	MSG msg_{};

	/**/


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
	/// DepthStencilStateの設定
	/// </summary>
	void SetUpDepthStencilState();

	/// <summary>
	/// ルートシグネチャの作成
	/// </summary>
	void MakeRootSignature();

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
	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	/// <summary>
	/// DescriptorHeap作成
	/// </summary>
	/// <param name="heapType"> Heapの種類 </param>
	/// <param name="descriptorsNum"> Descriptorの数 </param>
	/// <param name="shaderVisible"> Shaderに使用するか </param>
	/// <returns></returns>
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT descriptorsNum, bool shaderVisible);

	/// <summary>
	/// MaterialResourceの作成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// WvpResource(TransformationMatrix用のリソース)の作成
	/// </summary>
	void CreateWvpResource();

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
	/// Sprite用VertexResourceの生成
	/// </summary>
	void CreateSpriteVertexResource();

	/// <summary>
	/// Sprite用VertexBufferViewの生成
	/// </summary>
	void CreateSpriteVertexBufferView();

	/// <summary>
	/// Sprite用MaterialResourceの生成
	/// </summary>
	void CreateSpriteMaterialResource();

	/// <summary>
	/// Sprite用TransformResourceの生成
	/// </summary>
	void CreateSpriteTransformationResource();

	/// <summary>
	/// 三角形の描画呼び出し
	/// </summary>
	void DrawTriangleCall(const uint32_t& textureIndex);

	/// <summary>
	/// Spriteの描画呼び出し
	/// </summary>
	void DrawSpriteCall(const uint32_t& textureIndex);

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
	bool IsFrameExecute();

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

private:

	static AtrumEngine* instance_;

public:

	/// <summary>
	/// インスタンスの取得
	/// </summary>
	/// <returns> AtrumEngineインスタンス </returns>
	static AtrumEngine* GetInstance() {

		if (!instance_) {

			instance_ = new AtrumEngine();

		}

		return instance_;

	}

	/// <summary>
	/// インスタンスの破棄
	/// </summary>
	static void Destroy() {

		if (instance_) {

			delete instance_;
			instance_ = nullptr;

		}

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
	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metaData);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="textureResource"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateIntermediateResource(const ComPtr<ID3D12Resource>& textureResource);

	/// <summary>
	/// textureResourceにデータを転送する
	/// </summary>
	/// <param name="texture"> テクスチャポインタ </param>
	/// <param name="mipImages"> MipMap付データ </param>
	/// <param name="intermediateResource"> 中間リソース </param>
	void UploadTextureData(const ComPtr<ID3D12Resource>& textureREsource, const DirectX::ScratchImage& mipImages, const ComPtr<ID3D12Resource>& intermediateResource);

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
	/// 三角形の描画
	/// </summary>
	/// <param name="textureIndex"> テクスチャ番号 </param>
	/// <param name="textureColor"> テクスチャ色(補正) </param>
	/// <param name="triangleTransform"> 三角形の座標情報 </param>
	/// <param name="cameraTransform"> カメラの座標情報 </param>
	/// <param name="vertexData"> 三角形のローカル頂点データ 左下 ＞上 > 右下 </param>
	void DrawTriangle(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& triangleTransform, const Transform& cameraTransform, const std::array<VertexData, 3>& vertexData);

	/// <summary>
	/// Spriteの準備
	/// </summary>
	void PrepareSprite();

	/// <summary>
	/// 2D矩形の描画
	/// </summary>
	/// <param name="textureIndex"> テクスチャ番号 </param>
	/// <param name="textureColor"> テクスチャ色(補正) </param>
	/// <param name="plateTransform"> 板の座標情報 </param>
	void DrawSpriteRect(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& rectTransform, const Vector2& rectSize);

	/// <summary>
	/// 2D線の描画
	/// </summary>
	/// <param name="textureIndex"> テクスチャ番号 </param>
	/// <param name="textureColor"> テクスチャ色(補正) </param>
	/// <param name="start"> 始点 </param>
	/// <param name="end"> 終点 </param>
	/// <param name="width"> 太さ </param>
	void DrawSpriteLine(const uint32_t& textureIndex, const Vector4& textureColor, const Vector2& start, const Vector2& end, const float& width);


	/// <summary>
	/// DepthStencilResourceの作成
	/// </summary>
	/// <param name="width"> 幅 </param>
	/// <param name="height"> 高さ </param>
	/// <returns> DepthStencilResource </returns>
	ComPtr<ID3D12Resource> CreateDepthStencilResource(int32_t width, int32_t height);


	/* ゲッター */

	/// <summary>
	/// ゲッター デバイス
	/// </summary>
	/// <returns> デバイスへの参照 </returns>
	ComPtr<ID3D12Device>& GetDevice() { return device_; }

};

struct LeakChecker {

	~LeakChecker();

};