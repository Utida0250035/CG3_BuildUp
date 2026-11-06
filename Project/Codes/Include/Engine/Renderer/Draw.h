#pragma once

#include "Engine/Blend/BlendMode.h"
#include "Engine/Resource/ConstantBuffer.h"
#include "Engine/Resource/LightData.h"
#include "Engine/Resource/MaterialData.h"
#include "Engine/Resource/ParticleGpuData.h"
#include "Engine/Resource/TransformationData.h"
#include "Engine/Resource/VertexData.h"
#include "Geometry/PyramidMesh.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "Math/Vector3.h"

#include <array>
#include <memory>
#include <vector>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <wrl/client.h>

namespace Atrum {

	class VertexBuffer;
	class IndexBuffer;
	class CommandContext;
	class DescriptorAllocator;

	class AssetModel;

	class AtrumEngine;

	class Camera;

	class Draw final {

	private:

		struct Token {
			explicit Token() = default;
		};

		friend AtrumEngine;

		Camera* camera_ = nullptr;

		std::unique_ptr<VertexBuffer> vertexBuffer_ = nullptr;

		std::unique_ptr<IndexBuffer> indexBuffer_ = nullptr;

		std::unique_ptr<MultiConstantBuffer<MaterialData>> materialBuffer_ = nullptr;

		std::unique_ptr<MultiConstantBuffer<TransformationData>>
			transformationBuffer_ = nullptr;

		// パーティクル用（インスタンスごとの StructuredBuffer + フレーム CBV）
		std::unique_ptr<SingleConstantBuffer<ParticleFrameGpuData>>
			particleFrameBuffer_ = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> particleInstanceBufferResource_ =
		    nullptr;
		ParticleInstanceGpuData* particleInstanceData_ = nullptr;
		size_t particleInstanceCapacity_ = 0;
		DescriptorAllocator::DescriptorHandle particleInstanceSrv_{};

		// PreDraw 時点のブレンドに対応する Object3d PSO / パーティクル用 PSO
		std::array<ID3D12PipelineState *,
		           static_cast<size_t>(BlendMode::MODE_COUNT)>
		    object3dPipelineByBlend_{};
		std::array<ID3D12PipelineState *,
		           static_cast<size_t>(BlendMode::MODE_COUNT)>
		    particlePipelineByBlend_{};
		BlendMode activeBlendMode_ = BlendMode::NORMAL;

		uint32_t constantBufferCount_ = 0;

		CommandContext* pCommandContextDirect_ = nullptr;
		DescriptorAllocator* pSrvAllocator_ = nullptr;
		ID3D12Device* pDevice_ = nullptr;

		Draw() = default;

	public:

		~Draw() = default;

	private:

		static std::unique_ptr<Draw> instance_;

		/// <summary>
		/// 三角形の描画呼び出し
		/// </summary>
		void DrawTriangleCall(const uint32_t& textureIndex,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		/// <summary>
		/// 3D実体の描画呼び出し(モデル除く)
		/// </summary>
		void DrawCall(const uint32_t& textureIndex,
			const uint32_t& indexDataCountInInstance,
			const uint32_t& vertexCountInInstance,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);
		void DrawCall(const uint32_t& textureIndex,
			const uint32_t& vertexCountInInstance,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		/// <summary>
		/// 初期化処理 MaterialBufferの作成
		/// </summary>
		void CreateMaterialBuffer();

		/// <summary>
		/// 初期化処理 TransformationBufferの作成
		/// </summary>
		void CreateTransformationBuffer();

		void CreateParticleGpuResources();

		void CreateParticleInstanceSrv();

		/// <summary>
		/// 球メッシュの頂点・インデックスをバッチバッファの指定位置に書き込む
		/// </summary>
		void WriteSphereMeshToBatch(const uint32_t subdivision, const float radius,
			uint32_t vertexWriteStart, uint32_t indexWriteStart,
			uint32_t& outVertexCount, uint32_t& outIndexCount);

		/// <summary>
		/// ビルボード用クワッドの頂点・インデックスをバッチバッファに書き込む
		/// </summary>
		void WriteBillboardQuadToBatch(uint32_t vertexWriteStart,
			uint32_t indexWriteStart);

		/// <summary>
		/// パーティクル用パイプラインへ切り替え、描画後に Object3d へ戻す
		/// </summary>
		void DrawParticleIndexedInstanced(uint32_t textureIndex,
			uint32_t indexCountPerInstance, uint32_t vertexCountPerInstance,
			uint32_t startVertexLocation, uint32_t startIndexLocation,
			int32_t baseVertexLocation, uint32_t instanceCount,
			uint32_t instanceBufferBase,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress,
			const MaterialData *materialOverride = nullptr);

		void DrawParticleNonIndexedInstanced(
			uint32_t textureIndex, const D3D12_VERTEX_BUFFER_VIEW *meshVbv,
			uint32_t vertexCountPerInstance, uint32_t instanceCount,
			uint32_t instanceBufferBase,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress,
			const MaterialData *materialOverride);

		/// <summary>
		/// モデル描画後にバッチ用 VB/IB を PreDraw と同じ状態へ戻す
		/// </summary>
		void RestoreBatchGeometryBindings();

		/// <summary>
		/// 初期化処理 VertexBufferの生成
		/// </summary>
		void CreateVertexBuffer();

		/// <summary>
		/// 初期化処理 IndexBufferの生成
		/// </summary>
		void CreateIndexBuffer();

		inline static constexpr uint32_t kMaxDrawCount = 4096;

		// 画面上の頂点・インデックスの最大数（背景球＋弾＋戦闘パーティクルが同時に乗る量）
		inline static constexpr uint32_t kVertexMaxDrawCount = 65536;

		// 1 フレームあたりのパーティクルインスタンス上限（レイヤーごとバッチの先頭オフセットに使用）
		inline static constexpr uint32_t kMaxParticleInstances = 16384;

		// ビューポート
		D3D12_VIEWPORT viewport_{};

		// シザー矩形
		D3D12_RECT scissorRect_{};

	public:

		explicit Draw(Token) {};

		void Initialize(ID3D12Device* device, CommandContext* commandContext,
			DescriptorAllocator* srvAllocator);

		/// PreDraw で選択されているブレンドに対応する PSO を登録（通常描画／パーティクル）
		void RegisterObject3dAndParticlePipelines(
		    const std::array<ID3D12PipelineState *,
		                    static_cast<size_t>(BlendMode::MODE_COUNT)>
		        &object3dPipelines,
		    const std::array<ID3D12PipelineState *,
		                     static_cast<size_t>(BlendMode::MODE_COUNT)>
		        &particlePipelines);

		void SetActiveBlendMode(BlendMode mode) { activeBlendMode_ = mode; }

		void DrawTriangle(const uint32_t& textureIndex,
			const Math::Vector4& textureColor,
			const Math::Transform& uvTransform,
			const Math::Matrix4x4& worldMatrix,
			const std::array<VertexData, 3>& vertexData,
			const bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		void DrawAsymmetricPyramid(
			const uint32_t& textureIndex, const Math::Vector4& textureColor,
			const Math::Transform& uvTransform,
			const Math::Matrix4x4& worldMatrix,
			const Geometry::PyramidMesh& mesh, const bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		void DrawSphere(const uint32_t& textureIndex,
			const Math::Vector4& textureColor,
			const Math::Transform& uvTransform,
			const Math::Matrix4x4& worldMatrix, const float radius,
			const uint32_t subdivision, const bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		void DrawBillboard(const uint32_t& textureIndex,
			const Math::Vector4& textureColor,
			const Math::Transform& uvTransform,
			const Math::Matrix4x4& worldMatrix,
			const bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		/// ワールド座標の太線（カメラ向きリボン、Object3d パイプライン）
		void DrawLine3D(const uint32_t& textureIndex,
			const Math::Vector4& textureColor,
			const Math::Transform& uvTransform,
			const Math::Vector3& worldStart, const Math::Vector3& worldEnd,
			const float width, const bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		/// ワールド座標の破線（DrawLine3D を dash / gap 長で分割）
		void DrawDashedLine3D(const uint32_t& textureIndex,
			const Math::Vector4& textureColor,
			const Math::Transform& uvTransform,
			const Math::Vector3& worldStart, const Math::Vector3& worldEnd,
			const float width, const float dashLength, const float gapLength,
			const bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		void DrawModel(AssetModel* model, const Math::Matrix4x4& worldMatrix,
			const Math::Vector4& color, const bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		/// 同一形状・同一テクスチャのパーティクルを 1 ドローで描画（InstanceCount > 1）
		void DrawSphereParticlesInstanced(uint32_t textureIndex,
			const Math::Transform& uvTransform,
			const std::vector<ParticleInstanceGpuData>& instances,
			uint32_t subdivision, bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		void DrawBillboardParticlesInstanced(uint32_t textureIndex,
			const Math::Transform& uvTransform,
			const std::vector<ParticleInstanceGpuData>& instances,
			bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		void DrawModelParticlesInstanced(AssetModel *model,
			const std::vector<ParticleInstanceGpuData> &instances,
			bool isLighting,
			D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

		void SetCamera(Camera* camera) { camera_ = camera; }

		Draw operator=(const Draw& source) = delete;
		Draw(const Draw& source) = delete;

		void Finalize();

		static Draw* Instance() {

			if (!instance_) {

				instance_ = std::make_unique<Draw>(Token{});
			}

			return instance_.get();
		}

		static void Destroy() {

			if (instance_) {
				instance_.reset();
			}
		}
	};

} // namespace Atrum