#pragma once
#include "AssetModel.h"
#include "CommandContext.h"
#include "ConstantBuffer.h"
#include "DeltaTime.h"
#include "DescriptorAllocator.h"
#include "DirectionalLightData.h"
#include "Fence.h"
#include "IndexBuffer.h"
#include "Log.h"
#include "MaterialData.h"
#include "Matrix3D.h"
#include "PipelineState.h"
#include "RenderDevice.h"
#include "RootSignature.h"
#include "ShaderCompiler.h"
#include "StaticCast.h"
#include "SwapChain.h"
#include "Texture.h"
#include "Transform.h"
#include "TransformationData.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "VertexBuffer.h"
#include "VertexData.h"
#include <array>
#include <cstdint>
#include <DirectXTex/DirectXTex.h>
#include <memory>
#include <optional>
#include <SDL.h>
#include <SDL_syswm.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib, "dxgi.lib")

#ifdef USE_IMGUI

#include "ImGui.h"

#endif

namespace fs = std::filesystem;

class DirectInput;
class PlayInput;

class AtrumEngine final {

private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	// 初期化済フラグ
	bool isInitialized_ = false;

	/* Window */

	struct Window {
		SDL_Window* ptr = nullptr;

		~Window() {

			if (ptr) {

				SDL_DestroyWindow(ptr);

			}

			SDL_Quit();

			LogFile::GetInstance()->Log("SDL2: Quit");

		}

	};

	// ウィンドウクラス
	Window window_{};

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


	/* DirectXShaderCompiler */

	std::unique_ptr<ShaderCompiler> shaderCompiler_ = nullptr;


	/* RootSignature */

	std::unique_ptr<RootSignature> rootSignature_ = nullptr;


	/* PSO */

	// PipelineStateObject
	std::unique_ptr<PipelineState> graphicsPipelineState_ = nullptr;


	/* Vertex */

	std::unique_ptr<VertexBuffer> vertexBuffer_ = nullptr;

	inline static constexpr uint32_t kMaxDrawCount = 4096;

	/* 頂点インデックス */

	std::unique_ptr<IndexBuffer> indexBuffer_ = nullptr;


	/* Material */

	std::unique_ptr<MultiConstantBuffer<MaterialData>> materialBuffer_ = nullptr;


	/* WVP */

	std::unique_ptr<MultiConstantBuffer<TransformationData>> transformationBuffer_ = nullptr;

	/* DirectionalLight(3D専用) */

	std::unique_ptr<SingleConstantBuffer<DirectionalLightData>> directionalLightBuffer_ = nullptr;


	/* constantBufferCount */

	uint32_t constantBufferCount_ = 0;


	/* depthStencil */

	// DSVディスクリプタヒープ(Depth Stencil View)
	std::unique_ptr<DescriptorAllocator> dsvAllocator_ = nullptr;

	// DepthStencilResource
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;


	/* Sprite用 Vertex */

	std::unique_ptr<VertexBuffer> spriteVertexBuffer_ = nullptr;


	/* Sprite用 頂点インデックス */

	std::unique_ptr<IndexBuffer> spriteIndexBuffer_ = nullptr;

	inline static constexpr uint32_t kSpriteMaxDrawCount = 1024;


	/* Sprite用 Material */

	// MaterialResource
	std::unique_ptr<MultiConstantBuffer<MaterialData>> spriteMaterialBuffer_ = nullptr;


	/* Sprite用 Transform */

	std::unique_ptr<MultiConstantBuffer<TransformationData>> spriteTransformationBuffer_ = nullptr;

	/* Sprite用 constantBufferCount */

	uint32_t spriteConstantBufferCount_ = 0;

private:

	// 3DモデルAsset用 Meshマップ
	std::unordered_map<uint64_t, std::weak_ptr<AssetMeshData>> assetMeshMap_{};

	// 3DモデルAsset用 Materialマップ
	std::unordered_map<uint64_t, std::weak_ptr<AssetMaterialData>> assetMaterialMap_{};

	// 3DモデルAsset用 Modelマップ
	std::unordered_map<uint64_t, std::weak_ptr<AssetModel>> assetModelMap_{};


	/* テクスチャ */

	// Textureのsrv番号テーブル
	std::unordered_map<uint64_t, uint32_t> textureIndexTable_{};

	// Texture
	std::vector<Texture> textures_{};

	/* 射影行列 */

	// 透視投影
	const Matrix4x4 kPerspectiveFovMatrix = MakePerspectiveFovMatrix(0.5f, 1.77777f, 0.125f, 128.0f);

	// 正射影
	const Matrix4x4 kOrthographicMatrix = MakeOrthographicMatrix(0.0f, 0.0f, cast::Float(clientWidth_), cast::Float(clientHeight_), 0.0f, 100.0f);


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


	/* プレイヤー入力 */

	// DirectInput
	DirectInput* directInput_ = nullptr;

	// SDL2入力
	PlayInput* playInput_ = nullptr;

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
	/// 初期化処理 Viewport の設定
	/// </summary>
	void SetUpViewport();

	/// <summary>
	/// 初期化処理 シザー矩形の設定
	/// </summary>
	void SetUpScissorRect();

	/// <summary>
	/// 初期化処理 MaterialBufferの作成
	/// </summary>
	void CreateMaterialBuffer();

	/// <summary>
	/// 初期化処理 TransformationBufferの作成
	/// </summary>
	void CreateTransformationBuffer();

	/// <summary>
	/// 初期化処理 VertexBufferの生成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// 初期化処理 IndexBufferの生成
	/// </summary>
	void CreateIndexBuffer();

	/// <summary>
	/// 初期化処理 平行光源Bufferの作成
	/// </summary>
	void CreateDirectionalLightBuffer();

	/// <summary>
	/// 初期化処理 Sprite用VertexBufferの生成
	/// </summary>
	void CreateSpriteVertexBuffer();

	/// <summary>
	/// 初期化処理 Sprite用IndexBufferの生成
	/// </summary>
	void CreateSpriteIndexBuffer();

	/// <summary>
	/// 初期化処理 Sprite用MaterialBufferの生成
	/// </summary>
	void CreateSpriteMaterialBuffer();

	/// <summary>
	/// 初期化処理 Sprite用TransformBufferの生成
	/// </summary>
	void CreateSpriteTransformationBuffer();

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
	/// 裏の処理と×ボタン判定を行う
	/// </summary>
	/// <returns></returns>
	bool Process() const;

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
	ComPtr<ID3D12Resource> CreateIntermediateResource(const size_t intermediateSize);

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
	/// <param name="uvTransform"> uv座標情報 </param> 
	/// <param name="triangleTransform"> 三角形の座標情報 </param>
	/// <param name="cameraTransform"> カメラの座標情報 </param>
	/// <param name="vertexData"> 三角形のローカル頂点データ 左下 ＞上 > 右下 </param>
	/// <param name="directionalLightData"> 平行光源データ(option) </param>
	void DrawTriangle(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& triangleTransform, const Transform& cameraTransform, const std::array<VertexData, 3>& vertexData, const bool isLighting = false);

	/// <summary>
	/// 球の描画
	/// </summary>
	/// <param name="textureIndex"> テクスチャ番号 </param>
	/// <param name="textureColor"> テクスチャ色(補正) </param>
	/// <param name="uvTransform"> uv座標情報 </param> 
	/// <param name="triangleTransform"> 球の座標情報 </param>
	/// <param name="cameraTransform"> カメラの座標情報 </param>
	/// <param name="vertexData"> 球の半径 </param>
	/// <param name="directionalLightData"> 平行光源データ(option) </param>
	void DrawSphere(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& sphereTransform, const Transform& cameraTransform, const float radius, const uint32_t subdivision, const bool isLighting = false);

	/// <summary>
	/// 正四面体の描画
	/// </summary>
	/// <param name="textureIndex"> テクスチャ番号 </param>
	/// <param name="textureColor"> テクスチャ色(補正) </param>
	/// <param name="uvTransform"> uv座標情報 </param> 
	/// <param name="sphereTransform"></param>
	/// <param name="cameraTransform"></param>
	/// <param name="centerToVertices"></param>
	/// <param name="directionalLightData"></param>
	void DrawRegularTetrahedron(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& tetrahedronTransform, const Transform& cameraTransform, const float centerToVertices, const bool isLighting = false);

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
	/// <param name="directoryPath"> ファイル直上のフォルダまでのパス </param>
	/// <param name="fileName"> ファイル名 </param>
	/// <returns> メッシュデータ </returns>
	std::vector<AssetMeshNode> LoadObjFile(const std::string& directoryPath, const std::string& fileName, std::vector<std::string>& useMaterialNames);

	/// <summary>
	/// mtlファイルの読み込み
	/// </summary>
	/// <param name="directoryPath"> ファイル直上のフォルダまでのパス </param>
	/// <param name="fileName"> ファイル名 </param>
	/// <returns> 寿命保証用マテリアルデータ配列 </returns>
	std::vector<std::shared_ptr<AssetMaterialData>> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// 3Dモデルの生成
	/// </summary>
	/// <param name="objFilePath"> objファイルのパス </param>
	/// <returns> 管理番号(ハッシュ) </returns>
	std::shared_ptr<AssetModel> CreateModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName);

	/// <summary>
	/// 3Dモデルの取得||新規生成
	/// </summary>
	/// <param name="objFilePath"></param>
	/// <param name="mtlFilePath"></param>
	/// <returns></returns>
	std::shared_ptr<AssetModel> GetModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName);

	/// <summary>
	/// 3Dモデルの描画
	/// </summary>
	/// <param name="model"> 3Dモデルインスタンス </param>
	/// <param name="transform"> 3Dモデルの座標変換情報 </param>
	/// <param name="cameraTransform"> カメラの座標変換情報 </param>
	/// <param name="isLighting"> ライティングフラグ </param>
	void DrawModel(AssetModel* model, const Transform& transform, const Transform& cameraTransform, const bool isLighting);

	/* セッター */
	
	void SetDirectionalLightData(const DirectionalLightData& data) { directionalLightBuffer_->SetData(data); }


};

struct LeakChecker {

	~LeakChecker();

};