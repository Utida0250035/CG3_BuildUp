#pragma once
#include "Cast/StaticCast.h"
#include "Collision/HitMesh.h"
#include "ForDebug/Log.h"
#include "Engine/Command/CommandContext.h"
#include "Engine/Command/Fence.h"
#include "Engine/Device/RenderDevice.h"
#include "Engine/Device/SwapChain.h"
#include "Engine/Pipeline/PipelineState.h"
#include "Engine/Pipeline/RootSignature.h"
#include "Engine/Platform/Window/Window.h"
#include "Engine/Resource/AssetModel.h"
#include "Engine/Resource/ConstantBuffer.h"
#include "Engine/Resource/DescriptorAllocator.h"
#include "Engine/Resource/DirectionalLightData.h"
#include "Engine/Resource/IndexBuffer.h"
#include "Engine/Resource/MaterialData.h"
#include "Engine/Resource/Texture.h"
#include "Engine/Resource/TransformationData.h"
#include "Engine/Resource/VertexBuffer.h"
#include "Engine/Resource/VertexData.h"
#include "Engine/Shader/ShaderCompiler.h"
#include "Geometry/PyramidMesh.h"
#include "Math/Matrix3x3.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Time/DeltaTime.h"
#include <array>
#include <cstdint>
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
#include <DirectXTex/DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")


#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"

#endif

namespace Atrum {

	namespace fs = ::std::filesystem;

	namespace Input {

		class DirectInput;
		class PlayInput;

	}

	class ModelStorage;
	class TextureStorage;

	class Draw;
	class DrawSprite;

	namespace Audio {

		class AudioManager;

	}

	class AtrumEngine final {

	private:

		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		// 初期化済フラグ
		bool isInitialized_ = false;

		std::unique_ptr<Window> window_ = nullptr;

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


		/* DirectionalLight(3D専用) */

		std::unique_ptr<SingleConstantBuffer<DirectionalLightData>> directionalLightBuffer_ = nullptr;


		/* depthStencil */

		// DSVディスクリプタヒープ(Depth Stencil View)
		std::unique_ptr<DescriptorAllocator> dsvAllocator_ = nullptr;

		// DepthStencilResource
		ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;

	private:


		/* 時間管理 */

		// 次フレームまでのカウント
		float countForNextFrame_ = 0.0f;

		// fps(フレーム/s)
		float secondsPerFrame_ = 0.0f;

		// 時間差分
		std::unique_ptr<DeltaTime> deltaTimeManager_ = nullptr;


		/* プレイヤー入力 */

		// DirectInput
		Input::DirectInput* directInput_ = nullptr;

		// SDL2入力
		Input::PlayInput* playInput_ = nullptr;


		/* アセットストレージ */

		ModelStorage* pModelStorage_ = nullptr;

		TextureStorage* pTextureStorage_ = nullptr;


		/* 描画 */

		Draw* pDraw_ = nullptr;

		DrawSprite* pDrawSprite_ = nullptr;


		/* 音源再生 */

		Audio::AudioManager* audio_ = nullptr;

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
		/// <summary>
		/// 初期化処理 平行光源Bufferの作成
		/// </summary>
		void CreateDirectionalLightBuffer();

		/// <summary>
		/// 初期化処理 DepthStencilResourceの作成
		/// </summary>
		/// <param name="width"> 幅 </param>
		/// <param name="height"> 高さ </param>
		/// <returns> DepthStencilResource </returns>
		ComPtr<ID3D12Resource> CreateDepthStencilResource(int32_t width, int32_t height);


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

	public:

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
		void DrawTriangle(const uint32_t& textureIndex, const Math::Vector4& textureColor, const Math::Transform& uvTransform, const Math::Transform& triangleTransform, const std::array<VertexData, 3>& vertexData, const bool isLighting = false);

		/// <summary>
		/// 非対称ピラミッドの描画
		/// </summary>
		/// <param name="textureIndex"></param>
		/// <param name="textureColor"></param>
		/// <param name="uvTransform"></param>
		/// <param name="triangleTransform"></param>
		/// <param name="vertexData"></param>
		/// <param name="isLighting"></param>
		void DrawAsymmetricPyramid(const uint32_t& textureIndex, const Math::Vector4& textureColor, const Math::Transform& uvTransform, const Math::Vector3& scale, const Math::Quaternion& rotate, const Math::Vector3& translate, const Geometry::PyramidMesh& mesh, const bool isLighting = false);

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
		void DrawSphere(const uint32_t& textureIndex, const Math::Vector4& textureColor, const Math::Transform& uvTransform, const Math::Transform& sphereTransform, const float radius, const uint32_t subdivision, const bool isLighting = false);

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
		void DrawSpriteRect(const uint32_t& textureIndex, const Math::Vector4& textureColor, const Math::Transform& uvTransform, const Math::Transform& rectTransform, const Math::Vector2& rectSize);

		/// <summary>
		/// 2D線の描画
		/// </summary>
		/// <param name="textureIndex"> テクスチャ番号 </param>
		/// <param name="textureColor"> テクスチャ色(補正) </param>
		/// <param name="start"> 始点 </param>
		/// <param name="end"> 終点 </param>
		/// <param name="width"> 太さ </param>
		void DrawSpriteLine(const uint32_t& textureIndex, const Math::Vector4& textureColor, const Math::Transform& uvTransform, const Math::Vector2& start, const Math::Vector2& end, const float& width, const float& posZ);

		/// <summary>
		/// 3Dモデルの描画
		/// </summary>
		/// <param name="model"> 3Dモデルインスタンス </param>
		/// <param name="transform"> 3Dモデルの座標変換情報 </param>
		/// <param name="cameraTransform"> カメラの座標変換情報 </param>
		/// <param name="isLighting"> ライティングフラグ </param>
		void DrawModel(AssetModel* model, const Math::Transform& transform, const bool isLighting);



		/// <summary>
		/// テクスチャの取得
		/// </summary>
		/// <param name="filePath"> テクスチャのファイルパス </param>
		/// <returns> テクスチャ番号 </returns>
		uint32_t GetTexture(const std::string& filePath);

		/// <summary>
		/// 3Dモデルの取得||新規生成
		/// </summary>
		/// <param name="objFilePath"></param>
		/// <param name="mtlFilePath"></param>
		/// <returns></returns>
		std::shared_ptr<AssetModel> GetModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName);


		/* セッター */

		void SetDirectionalLightData(const DirectionalLightData& data) { directionalLightBuffer_->SetData(data); }

		void SetViewMatrix(const Math::Matrix4x4& mat);

	};

	using Engine = AtrumEngine;

	struct LeakChecker {

		~LeakChecker();

	};

}