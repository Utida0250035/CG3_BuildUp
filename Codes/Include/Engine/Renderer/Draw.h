#pragma once

#include "Engine/ConstantBuffer.h"
#include "Engine/DirectionalLightData.h"
#include "Engine/MaterialData.h"
#include "Engine/VertexData.h"
#include "Engine/TransformationData.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"

#include <memory>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

class VertexBuffer;
class IndexBuffer;
class CommandContext;
class DescriptorAllocator;

class AssetModel;

struct Quaternion;
struct PyramidMesh;

class AtrumEngine;

class Draw final {

private:

	friend AtrumEngine;

	const Matrix4x4 kPerspectiveFovMatrix = MakePerspectiveFovMatrix(0.5f, 1.77777f, 0.125f, 1024.0f);

	Matrix4x4 viewMatrix_{};

	std::unique_ptr<VertexBuffer> vertexBuffer_ = nullptr;

	std::unique_ptr<IndexBuffer> indexBuffer_ = nullptr;

	std::unique_ptr<MultiConstantBuffer<MaterialData>> materialBuffer_ = nullptr;

	std::unique_ptr<MultiConstantBuffer<TransformationData>> transformationBuffer_ = nullptr;

	uint32_t constantBufferCount_ = 0;


	CommandContext* pCommandContextDirect_ = nullptr;
	DescriptorAllocator* pSrvAllocator_ = nullptr;
	ID3D12Device* pDevice_ = nullptr;

	Draw() = default;
	~Draw() = default;

	static Draw* instance_;


	/// <summary>
	/// 三角形の描画呼び出し
	/// </summary>
	void DrawTriangleCall(const uint32_t& textureIndex, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

	/// <summary>
	/// 3D実体の描画呼び出し(モデル除く)
	/// </summary>
	void DrawCall(const uint32_t& textureIndex, const uint32_t& indexDataCountInInstance, const uint32_t& vertexCountInInstance, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);
	void DrawCall(const uint32_t& textureIndex, const uint32_t& vertexCountInInstance, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

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

	inline static constexpr uint32_t kMaxDrawCount = 4096;

	// 画面上の頂点の最大描画数
	inline static constexpr uint32_t kVertexMaxDrawCount = 16384;

	// ビューポート
	D3D12_VIEWPORT viewport_{};

	// シザー矩形
	D3D12_RECT scissorRect_{};

public:

	void Initialize(ID3D12Device* device, CommandContext* commandContext, DescriptorAllocator* srvAllocator);

	void DrawTriangle(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& triangleTransform, const std::array<VertexData, 3>& vertexData, const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);
	
	void DrawAsymmetricPyramid(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Vector3& scale, const Quaternion& rotate, const Vector3& translate, const PyramidMesh& mesh, const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);
	
	void DrawSphere(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& sphereTransform, const float radius, const uint32_t subdivision, const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

	void DrawModel(AssetModel* model, const Transform& transform, const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress);

	void SetViewMatrix(const Matrix4x4& mat) { viewMatrix_ = mat; }

	Draw operator=(const Draw& source) = delete;
	Draw(const Draw& source) = delete;

	static Draw* GetInstance() {

		if (!instance_) {

			instance_ = new Draw();

		}

		return instance_;

	}

	static void Destroy() {

		if (instance_) {

			delete instance_;

			instance_ = nullptr;

		}

	}

};