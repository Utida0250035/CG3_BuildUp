#pragma once
#include "DeltaTime.h"
#include "CommandContext.h"
#include "RenderDevice.h"
#include "SwapChain.h"
#include "DescriptorAllocator.h"
#include "Fence.h"
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
#include <array>

#include <memory>
#include <unordered_map>
#include <optional>

namespace fs = std::filesystem;

class AtrumEngine final {

private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	struct Transform {
		Vector3 scale{ 1.0f, 1.0f, 1.0f };
		Vector3 rotate{};
		Vector3 translate{};
	};

	struct VertexData {
		Vector4 position;
		Vector2 texCoord;
		Vector3 normal;
	};

	struct Texture {

		ComPtr<ID3D12Resource> resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};

		// 使用するSRVディスクリプタの番号
		uint32_t srvIndex = 1;
	};

	enum class LightModel : uint32_t {
		Lambert,
		HalfLambert
	};

	struct DirectionalLightData {
		// 平行光源の色
		Vector4 color;
		// 平行光源の向き
		Vector3 direction;
		// 平行光源の輝度
		float intensity;
		// 光源の種類
		LightModel lightModel;

		// 16 + 12 + 4 + 4 = 36
		// 残り220バイト分
		float padding[55];

	};

private:

	// 初期化済フラグ
	bool isInitialized_ = false;

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

	// Windowsエラーハンドル
	[[maybe_unused]] HRESULT hr_{};


	/* RenderDevice */

	std::unique_ptr<RenderDevice> renderDevice_ = nullptr;

	/* Command */

	// コマンド経路(Direct)
	std::unique_ptr<CommandContext> commandContextDirect_ = nullptr;

	/* SwapChain */

	std::unique_ptr<SwapChain> swapChainManager_ = nullptr;


	/* SRV */

	std::unique_ptr<DescriptorAllocator> srvAllocator_ = nullptr;


	/* RTV */

	std::unique_ptr<DescriptorAllocator> rtvAllocator_ = nullptr;


	/* フェンス / フェンスイベント */

	std::unique_ptr<Fence> fenceManager_ = nullptr;


	/* 中間リソース */

	// フレーム内の中間リソース保存
	std::vector<ComPtr<ID3D12Resource>> temporaryResources_;


	/* DirectXShaderCompiler 補助 / コンパイラ本体 */

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
	D3D12_INPUT_ELEMENT_DESC inputElementDescriptions_[3]{};

	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};


	/* 描画State */

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

	// 画面上に描画済みの頂点の数
	uint32_t vertexDrewCount_ = 0;

	// 画面上の三角形の最大描画数
	inline static constexpr uint32_t kTriangleMaxDrawCount = 1024;

	struct TransformationData {
		Matrix4x4 wvp{};
		Matrix4x4 world{};

		// 4 * 16 + 4 * 16 = 128
		// (256 - 128) / 4
		// ConstantBuffer用の詰め物
		float padding[32]{};
	};

	/* 頂点インデックス */

	// インデックスリソース
	ComPtr<ID3D12Resource> indexResource_ = nullptr;

	// IndexBufferView
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	// 頂点インデックスデータ
	uint32_t* indexData_ = nullptr;

	// 頂点インデックスカウント
	uint32_t vertexIndexDrewCount_ = 0;


	/* Material */

	struct MaterialData {
		Vector4 color{};
		Matrix4x4 uvTransform{};
		int32_t inLightingEnable = false;
		// ConstantBuffer用の詰め物
		float padding[43]{};
	};

	// MaterialResource
	ComPtr<ID3D12Resource> materialResource_ = nullptr;

	// MaterialData 色データRGBA
	MaterialData* materialData_ = nullptr;


	/* WVP */

	// WvpResource
	ComPtr<ID3D12Resource> transformationResource_ = nullptr;

	// WvpData 描画座標データ
	TransformationData* transformationData_ = nullptr;

	/* DirectionalLight(3D専用) */

	// 平行光源Resource
	ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;

	// 平行光源Data
	DirectionalLightData* directionalLightData_ = nullptr;


	/* constantBufferCount */

	uint32_t constantBufferCount_ = 0;


	/* depthStencil */

	// DSVディスクリプタヒープ(Depth Stencil View)
	std::unique_ptr<DescriptorAllocator> dsvAllocator_ = nullptr;

	// DepthStencilResource
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;


	/* Sprite用 Vertex */

	// Sprite用のVertexResource
	ComPtr<ID3D12Resource> spriteVertexResource_ = nullptr;

	// Sprite用のVertexBufferView
	D3D12_VERTEX_BUFFER_VIEW spriteVertexBufferView_{};

	// Sprite用 頂点データ
	VertexData* spriteVertexData_ = nullptr;

	// Sprite用 総描画頂点数のカウント
	uint32_t spriteVertexDrewCount_ = 0;


	/* Sprite用 頂点インデックス */

	// Sprite用 IndexResource
	ComPtr<ID3D12Resource> spriteIndexResource_ = nullptr;

	// Sprite用 IndesData
	uint32_t* spriteIndexData_ = nullptr;

	// Sprite用 IndexBufferView
	D3D12_INDEX_BUFFER_VIEW spriteIndexBufferView_{};

	// Sprite用 頂点インデックスのカウント
	uint32_t spriteVertexIndexCount_ = 0;

	inline static constexpr uint32_t kSpriteTriangleMaxDrawCount = 1024;


	/* Sprite用 Material */

	// MaterialResource
	ComPtr<ID3D12Resource> spriteMaterialResource_ = nullptr;

	// MaterialData
	MaterialData* spriteMaterialData_ = nullptr;


	/* Sprite用 Transform */

	// Sprite用のTransformMatrix用のリソース
	ComPtr<ID3D12Resource> spriteTransformationResource_ = nullptr;

	// Sprite用 Transformデータ
	TransformationData* spriteTransformData_ = nullptr;

	/* Sprite用 constantBufferCount */

	uint32_t spriteConstantBufferCount_ = 0;

	/* Asset用 Mesh */
	struct AssetMeshData {

		// 頂点データ
		std::vector<VertexData> vertices;

		// 頂点リソース
		ComPtr<ID3D12Resource> vertexResource = nullptr;

		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	};

	/* Asset用 Material */
	struct AssetMaterialData {

		// テクスチャのファイルパスのハッシュ
		uint64_t textureFileHash = 0u;

		// srvディスクリプタヒープ上の番号
		uint32_t textureSrvIndex = 0u;

#ifdef _DEBUG

		// テクスチャのファイルパス
		std::string textureFilePath = "";

#endif

		// マテリアルリソース
		ComPtr<ID3D12Resource> materialResource = nullptr;

		// マテリアルデータ
		MaterialData* materialData = nullptr;

	};

	struct AssetMeshNode {

		std::shared_ptr<AssetMeshData> mesh;
		std::shared_ptr<AssetMaterialData> material;

	};

public:

	/* Asset用 Model */
	class AssetModel {

	private:

		friend AtrumEngine;

		// 座標変換リソース
		ComPtr<ID3D12Resource> transformationResource_ = nullptr;
		// 座標変換データ
		TransformationData* transformationData_ = nullptr;

		// メッシュの塊の添え字検索
		//std::unordered_map<uint64_t, size_t> nodeHashToIndexTable_{};
		// メッシュと対応マテリアルの塊
		//std::vector<AssetMeshNode> meshNodes_{};

		// objファイルからのメッシュ
		std::shared_ptr<AssetMeshData> mesh_ = nullptr;
		// mtlファイルからのマテリアル
		std::shared_ptr<AssetMaterialData> material_ = nullptr;

#ifdef _DEBUG

		std::string objFilePathDebug_ = "";
		std::string mtlFilePathDebug_ = "";

#endif

	public:

		void Draw(AtrumEngine* atrum, const Transform& transform, const Transform& cameraTransform) {

			// カメラのワールド行列
			Matrix4x4 cameraWorldMatrix = atrum->CreateWorldMatrix(cameraTransform);

			// ビュー行列
			Matrix4x4 viewMatrix = MatrixInverse(cameraWorldMatrix);

			// 透視投影行列
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.5f, 1.77777f, 0.125f, 128.0f);

			// 三角形のTransform
			Matrix4x4 worldMatrix = atrum->CreateWorldMatrix(transform);

			auto commandList = atrum->commandContextDirect_->GetCommandList();

			transformationData_->world = worldMatrix;

			transformationData_->wvp = worldMatrix * viewMatrix * projectionMatrix;

			DescriptorAllocator::DescriptorHandle textureHandle{};

			textureHandle = atrum->srvAllocator_->GetHandle(material_->textureSrvIndex);

			// SRVのDescriptorTableの先頭を設定 rootParameter[2]
			atrum->commandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

			commandList->SetGraphicsRootConstantBufferView(0, material_->materialResource->GetGPUVirtualAddress());

			commandList->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());

			commandList->SetGraphicsRootConstantBufferView(3, atrum->directionalLightResource_->GetGPUVirtualAddress());

			commandList->IASetVertexBuffers(0u, 1u, &mesh_->vertexBufferView);

			commandList->DrawInstanced(static_cast<UINT>(mesh_->vertices.size()), 1, 0, 0);

		}

	};

private:

	// 3DモデルAsset用 Meshテーブル
	std::unordered_map<uint64_t, std::weak_ptr<AssetMeshData>> assetMeshTable_{};

	// 3DモデルAsset用 Materialテーブル
	std::unordered_map<uint64_t, std::weak_ptr<AssetMaterialData>> assetMaterialData_{};

	// 3DモデルAsset用 Modelテーブル
	std::unordered_map<uint64_t, std::weak_ptr<AssetModel>> assetModelTable_{};


	/* テクスチャ */

	// Textureのsrv番号テーブル
	std::unordered_map<uint64_t, uint32_t> textureIndexTable_{};

	// Texture
	std::vector<Texture> textures_{};


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
	/// 初期化処理 エラー抑制 デバッグ用
	/// </summary>
	void ErrorSuppressionDebug();

	/// <summary>
	/// 初期化処理 DXCの初期化 
	/// </summary>
	void InitDXC();

	/// <summary>
	/// 初期化処理 Viewport の設定
	/// </summary>
	void SetUpViewport();

	/// <summary>
	/// 初期化処理 シザー矩形の設定
	/// </summary>
	void SetUpScissorRect();

	/// <summary>
	/// 初期化処理 InputLayoutの設定
	/// </summary>
	void SetUpInputLayout();

	/// <summary>
	/// 初期化処理 BlendStateの設定
	/// </summary>
	void SetUpBlendState();

	/// <summary>
	/// 初期化処理 RasterizerStateの設定
	/// </summary>
	void SetUpRasterizerState();

	/// <summary>
	/// 初期化処理 DepthStencilStateの設定
	/// </summary>
	void SetUpDepthStencilState();

	/// <summary>
	/// 初期化処理 ルートシグネチャの作成
	/// </summary>
	void MakeRootSignature();

	/// <summary>
	/// 初期化処理 Shaderのコンパイル
	/// </summary>
	/// <param name="filePath"> コンパイルするShaderファイルへのパス </param>
	/// <param name="profile"> コンパイルに使用するプロファイル </param>
	/// <returns> コンパイル結果(実行用のバイナリ) </returns>
	IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile
	);

	/// <summary>
	/// 初期化処理 Shaderの準備
	/// </summary>
	void PrepareShader();

	/// <summary>
	/// BufferResource作成
	/// </summary>
	/// <param name="device"> デバイス </param>
	/// <param name="sizeInBytes"> Resourceのサイズ </param>
	/// <returns> Resource </returns>
	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceState);

	/// <summary>
	/// UploadBuffer作成
	/// </summary>
	/// <param name="device"> デバイス </param>
	/// <param name="sizeInBytes"> Bufferのサイズ </param>
	/// <returns> Resource </returns>
	ComPtr<ID3D12Resource> CreateUploadBuffer(size_t sizeInBytes);

	/// <summary>
	/// DefaultBuffer作成
	/// </summary>
	/// <param name="device"> デバイス </param>
	/// <param name="sizeInBytes"> Bufferのサイズ </param>
	/// <returns> Resource </returns>
	ComPtr<ID3D12Resource> CreateDefaultBuffer(size_t sizeInBytes);

	/// <summary>
	/// 初期化処理 MaterialResourceの作成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// 初期化処理 WvpResource(TransformationMatrix用のリソース)の作成
	/// </summary>
	void CreateTransformationResource();

	/// <summary>
	/// 初期化処理 PSOの生成
	/// </summary>
	void CreatePSO();

	/// <summary>
	/// 初期化処理 VertexResourceの生成
	/// </summary>
	void CreateVertexResource();

	/// <summary>
	/// 初期化処理 VertexBufferViewの作成
	/// </summary>
	void CreateVertexBufferView();

	/// <summary>
	/// 初期化処理 IndexResourceの生成
	/// </summary>
	void CreateIndexResource();

	/// <summary>
	/// 初期化処理 IndexBufferViewの生成
	/// </summary>
	void CreateIndexBufferView();

	/// <summary>
	/// 初期化処理 平行光源Resourceの作成
	/// </summary>
	void CreateDirectionalLightResource();

	/// <summary>
	/// 初期化処理 Sprite用VertexResourceの生成
	/// </summary>
	void CreateSpriteVertexResource();

	/// <summary>
	/// 初期化処理 Sprite用VertexBufferViewの生成
	/// </summary>
	void CreateSpriteVertexBufferView();

	/// <summary>
	/// 初期化処理 Sprite用IndexResourceの生成
	/// </summary>
	void CreateSpriteIndexResource();

	/// <summary>
	/// 初期化処理 Sprite用IndexBufferViewの生成
	/// </summary>
	void CreateSpriteIndexBufferView();

	/// <summary>
	/// 初期化処理 Sprite用MaterialResourceの生成
	/// </summary>
	void CreateSpriteMaterialResource();

	/// <summary>
	/// 初期化処理 Sprite用TransformResourceの生成
	/// </summary>
	void CreateSpriteTransformationResource();

	/// <summary>
	/// 初期化処理 DepthStencilResourceの作成
	/// </summary>
	/// <param name="width"> 幅 </param>
	/// <param name="height"> 高さ </param>
	/// <returns> DepthStencilResource </returns>
	ComPtr<ID3D12Resource> CreateDepthStencilResource(int32_t width, int32_t height);

	/// <summary>
	/// 三角形の描画呼び出し
	/// </summary>
	void DrawTriangleCall(const uint32_t& textureIndex);

	/// <summary>
	/// 球の描画呼び出し
	/// </summary>
	void DrawSphereCall(const uint32_t& textureIndex, const uint32_t& indexDataCountInSphere, const uint32_t& vertexCountInSphere);

	/// <summary>
	/// Spriteの描画呼び出し
	/// </summary>
	void DrawSpriteCall(const uint32_t& textureIndex);

public:

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
	bool IsProcess() const;

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
	void ImGuiNewFrame() const;

	/// <summary>
	/// ImGuiの内部コマンド生成
	/// </summary>
	void ImGuiRender() const;

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
	Matrix4x4 CreateWorldMatrix(const Transform& transform) const;



private:

	/// <summary>
	/// 中間リソース生成の汎用関数
	/// </summary>
	/// <param name="resourceSize"> 中間リソースのサイズ </param>
	/// <returns> 中間リソース </returns>
	ComPtr<ID3D12Resource> CreateIntermediateResource(const size_t intermediateSize, const D3D12_RESOURCE_STATES resourceState);

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
	/// テクスチャ読み込み用中間リソースの作成
	/// </summary>
	/// <param name="textureResource"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateTextureIntermediateResource(ID3D12Resource* textureResource);

	/// <summary>
	/// textureResourceにデータを転送する
	/// </summary>
	/// <param name="texture"> テクスチャポインタ </param>
	/// <param name="mipImages"> MipMap付データ </param>
	/// <param name="intermediateResource"> 中間リソース </param>
	void UploadTextureData(ID3D12Resource* textureResource, const DirectX::ScratchImage& mipImages, ID3D12Resource* intermediateResource);

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
	void DrawTriangle(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& triangleTransform, const Transform& cameraTransform, const std::array<VertexData, 3>& vertexData, const std::optional<DirectionalLightData>& directionalLightData = std::nullopt);

	/// <summary>
	/// 球の描画
	/// </summary>
	/// <param name="textureIndex"> テクスチャ番号 </param>
	/// <param name="textureColor"> テクスチャ色(補正) </param>
	/// <param name="triangleTransform"> 球の座標情報 </param>
	/// <param name="cameraTransform"> カメラの座標情報 </param>
	/// <param name="vertexData"> 球の半径 </param>
	void DrawSphere(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& sphereTransform, const Transform& cameraTransform, const float radius, const uint32_t subdivision, const std::optional<DirectionalLightData>& directionalLightData = std::nullopt);

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
	void DrawSpriteRect(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& rectTransform, const Vector2& rectSize);

	/// <summary>
	/// 2D線の描画
	/// </summary>
	/// <param name="textureIndex"> テクスチャ番号 </param>
	/// <param name="textureColor"> テクスチャ色(補正) </param>
	/// <param name="start"> 始点 </param>
	/// <param name="end"> 終点 </param>
	/// <param name="width"> 太さ </param>
	void DrawSpriteLine(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Vector2& start, const Vector2& end, const float& width, const float& posZ);


	/// <summary>
	/// objファイルの読み込み
	/// </summary>
	/// <param name="filePath"> ファイルパス </param>
	/// <returns> メッシュデータ </returns>
	std::shared_ptr<AssetMeshData> LoadObjFile(const std::string& directoryPath, const std::string& fileName);

	std::shared_ptr<AssetMaterialData> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// 3Dモデルの生成
	/// </summary>
	/// <param name="objFilePath"> objファイルのパス </param>
	/// <returns> 管理番号(ハッシュ) </returns>
	std::shared_ptr<AssetModel> CreateModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl ,const std::string& mtlFileName);

	/// <summary>
	/// 3Dモデルの取得||新規生成
	/// </summary>
	/// <param name="objFilePath"></param>
	/// <param name="mtlFilePath"></param>
	/// <returns></returns>
	std::shared_ptr<AssetModel> GetModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName);

};

struct LeakChecker {

	~LeakChecker();

};