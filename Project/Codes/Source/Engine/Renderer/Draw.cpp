#include "Engine/Alias/CoreAlias.h"

#include "Cast/StaticCast.h"
#include "Engine/Command/CommandContext.h"
#include "Engine/Device/SwapChain.h"
#include "Engine/Pipeline/PipelineState.h"
#include "Engine/Pipeline/RootSignature.h"
#include "Engine/Renderer/Draw.h"
#include "Engine/Resource/AssetModel.h"
#include "Engine/Resource/DescriptorAllocator.h"
#include "Engine/Resource/IndexBuffer.h"
#include "Engine/Resource/VertexBuffer.h"
#include "Geometry/PyramidMesh.h"
#include "Math/Quaternion.h"
#include "Camera/Camera.h"

#include <array>
#include <numbers>
#ifdef DEVELOPMENT
#include <chrono>
#include <fstream>
#include <format>
#endif

namespace {

#ifdef DEVELOPMENT

// #region agent log
void AgentDebugLogDraw(const char *location, const char *message,
                       const char *hypothesisId, const std::string &dataJson) {
  const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now().time_since_epoch())
                             .count();
  std::ofstream log(
      R"(./.cursor/debug-db6cac.log)",
      std::ios::app);
  if (!log) {
    return;
  }
  log << std::format(
             R"({{"sessionId":"db6cac","runId":"pre-fix","hypothesisId":"{}","location":"{}","message":"{}","data":{},"timestamp":{}}})",
             hypothesisId, location, message, dataJson, timestamp)
      << "\n";
}
// #endregion

#endif

} // namespace

namespace Atrum {

	Draw* Draw::instance_ = nullptr;

	void Draw::Initialize(ID3D12Device* device,
		CommandContext* commandContextDirect,
		DescriptorAllocator* srvAllocator) {

		pDevice_ = device;

		pCommandContextDirect_ = commandContextDirect;

		pSrvAllocator_ = srvAllocator;

		CreateVertexBuffer();

		CreateIndexBuffer();

		CreateMaterialBuffer();

		CreateTransformationBuffer();
	}

	void Draw::CreateVertexBuffer() {

		vertexBuffer_ = std::make_unique<VertexBuffer>();

		vertexBuffer_->CreateVertexBuffer(kVertexMaxDrawCount, pDevice_);
	}

	void Draw::CreateIndexBuffer() {

		indexBuffer_ = std::make_unique<IndexBuffer>();

		indexBuffer_->CreateIndexBuffer(kVertexMaxDrawCount, pDevice_);
	}

	void Draw::CreateMaterialBuffer() {

		materialBuffer_ = std::make_unique<MultiConstantBuffer<MaterialData>>();

		materialBuffer_->CreateBuffer(pDevice_, kMaxDrawCount);
	}

	void Draw::CreateTransformationBuffer() {

		transformationBuffer_ =
			std::make_unique<MultiConstantBuffer<TransformationData>>();

		transformationBuffer_->CreateBuffer(pDevice_, kMaxDrawCount);
	}

	void Draw::DrawTriangleCall(const uint32_t& textureIndex,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (constantBufferCount_ + 1 >= kMaxDrawCount) {
			return;
		}

		assert(constantBufferCount_ + 1 < kMaxDrawCount &&
			"constantBufferCount over maxCount(Sphere)");

	 // TransformMatrix (WVP) のアドレス計算
		D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr =
			transformationBuffer_->GetGpuVirtualAddress() +
			(constantBufferCount_ * sizeof(TransformationData));
		// GPUに設定(rootParameter0)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			1, transformOffsetAddr);

		// Material (Color) のアドレス計算
		D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr =
			materialBuffer_->GetGpuVirtualAddress() +
			(constantBufferCount_ * sizeof(MaterialData));
		// GPUに設定(rootParameter1)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			0, materialOffsetAddr);

		// GPUに設定(rootParameter3)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			3, directionalLightAddress);

		DescriptorAllocator::DescriptorHandle textureHandle{};

		textureHandle = pSrvAllocator_->GetHandle(textureIndex);

		// SRVのDescriptorTableの先頭を設定 rootParameter[2]
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(
			2, textureHandle.gpu);

		// 描画(DrawCall) 3頂点で1つのインスタンス
		pCommandContextDirect_->GetCommandList()->DrawIndexedInstanced(
			3, 1, indexBuffer_->GetDrewCount(), vertexBuffer_->GetDrewCount(), 0);

		vertexBuffer_->AddDrewCount(3);
		indexBuffer_->AddDrewCount(3);

		constantBufferCount_++;
	}

	void Draw::DrawCall(const uint32_t& textureIndex,
		const uint32_t& indexDataCountInInstance,
		const uint32_t& vertexCountInInstance,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (constantBufferCount_ + 1 >= kMaxDrawCount) {
			return;
		}

		assert(constantBufferCount_ + 1 < kMaxDrawCount &&
			"constantBufferCount over maxCount(Sphere)");

	 // TransformMatrix (WVP) のアドレス計算
		D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr =
			transformationBuffer_->GetGpuVirtualAddress() +
			(constantBufferCount_ * sizeof(TransformationData));
		// GPUに設定(rootParameter0)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			1, transformOffsetAddr);

		// Material (Color) のアドレス計算
		D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr =
			materialBuffer_->GetGpuVirtualAddress() +
			(constantBufferCount_ * sizeof(MaterialData));

		// GPUに設定(rootParameter1)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			0, materialOffsetAddr);

		// GPUに設定(rootParameter3)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			3, directionalLightAddress);

		DescriptorAllocator::DescriptorHandle textureHandle{};

		textureHandle = pSrvAllocator_->GetHandle(textureIndex);

		// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(
			2, textureHandle.gpu);

		// 描画(DrawCall) 3頂点で1つのインスタンス
		pCommandContextDirect_->GetCommandList()->DrawIndexedInstanced(
			indexDataCountInInstance, 1, indexBuffer_->GetDrewCount(),
			vertexBuffer_->GetDrewCount(), 0);

		indexBuffer_->AddDrewCount(indexDataCountInInstance);

		vertexBuffer_->AddDrewCount(vertexCountInInstance);

		constantBufferCount_++;
	}

	void Draw::DrawCall(const uint32_t& textureIndex,
		const uint32_t& vertexCountInInstance,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (constantBufferCount_ + 1 >= kMaxDrawCount) {
			return;
		}

		assert(constantBufferCount_ + 1 < kMaxDrawCount &&
			"constantBufferCount over maxCount(Sphere)");

	 // TransformMatrix (WVP) のアドレス計算
		D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr =
			transformationBuffer_->GetGpuVirtualAddress() +
			(constantBufferCount_ * sizeof(TransformationData));
		// GPUに設定(rootParameter0)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			1, transformOffsetAddr);

		// Material (Color) のアドレス計算
		D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr =
			materialBuffer_->GetGpuVirtualAddress() +
			(constantBufferCount_ * sizeof(MaterialData));

		// GPUに設定(rootParameter1)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			0, materialOffsetAddr);

		// GPUに設定(rootParameter3)
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(
			3, directionalLightAddress);

		DescriptorAllocator::DescriptorHandle textureHandle{};

		textureHandle = pSrvAllocator_->GetHandle(textureIndex);

		// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
		pCommandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(
			2, textureHandle.gpu);

		// 描画(DrawCall) 3頂点で1つのインスタンス
		pCommandContextDirect_->GetCommandList()->DrawInstanced(
			vertexCountInInstance, 1, vertexBuffer_->GetDrewCount(), 0);

		vertexBuffer_->AddDrewCount(vertexCountInInstance);

		constantBufferCount_++;
	}

	void Draw::DrawTriangle(const uint32_t& textureIndex,
		const M::Vector4& textureColor,
		const M::Transform& uvTransform,
		const M::Transform& triangleTransform,
		const std::array<VertexData, 3>& vertexData,
		const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

// 三角形のM::Transform
		M::Matrix4x4 triangleWorldMatrix = triangleTransform.MakeWorldMatrix();

		TransformationData transformationData{};

		transformationData.wvp =
			triangleWorldMatrix * camera_->GetViewProjectionMatrix();
		transformationData.world = triangleWorldMatrix;

		transformationBuffer_->SetData(transformationData, constantBufferCount_);

		MaterialData materialData{};

		materialData.color = textureColor;

		M::Matrix4x4 uvTransformMatrix = M::Matrix4x4::Scale(uvTransform.scale);
		uvTransformMatrix *= M::Matrix4x4::RotateZ(uvTransform.rotate.z);
		uvTransformMatrix *= M::Matrix4x4::Translate(uvTransform.translate);
		materialData.uvTransformMatrix = uvTransformMatrix;

		materialData.isLightingEnable = isLighting;

		materialBuffer_->SetData(materialData, constantBufferCount_);

		uint32_t vertexDrewCount = vertexBuffer_->GetDrewCount();

		vertexBuffer_->SetVertexData(vertexData[0], vertexDrewCount);

		vertexBuffer_->SetVertexData(vertexData[1], vertexDrewCount + 1);

		vertexBuffer_->SetVertexData(vertexData[2], vertexDrewCount + 2);

		uint32_t indexDrewCount = indexBuffer_->GetDrewCount();

		indexBuffer_->SetIndexData(0, indexDrewCount);
		indexBuffer_->SetIndexData(1, indexDrewCount + 1);
		indexBuffer_->SetIndexData(2, indexDrewCount + 2);

		// 描画
		this->DrawTriangleCall(textureIndex, directionalLightAddress);
	}

	void Draw::DrawSphere(const uint32_t& textureIndex,
		const M::Vector4& textureColor,
		const M::Transform& uvTransform,
		const M::Transform& sphereTransform, const float radius,
		const uint32_t subdivision, const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		// 球のM::Transform
		M::Matrix4x4 sphereWorldMatrix = sphereTransform.MakeWorldMatrix();

		this->DrawSphere(textureIndex, textureColor, uvTransform, sphereWorldMatrix, radius, subdivision, isLighting, directionalLightAddress);
	}

	void Draw::DrawSphere(const uint32_t& textureIndex,
		const Math::Vector4& textureColor,
		const Math::Transform& uvTransform,
		const Math::Matrix4x4& worldMatrix, const float radius,
		const uint32_t subdivision, const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		TransformationData transformationData{};

		transformationData.wvp =
			worldMatrix * camera_->GetViewProjectionMatrix();
		transformationData.world = worldMatrix;

		transformationBuffer_->SetData(transformationData, constantBufferCount_);

		MaterialData materialData{};

		materialData.color = textureColor;

		M::Matrix4x4 uvTransformMatrix = M::Matrix4x4::Scale(uvTransform.scale);
		uvTransformMatrix *= M::Matrix4x4::RotateZ(uvTransform.rotate.z);
		uvTransformMatrix *= M::Matrix4x4::Translate(uvTransform.translate);
		materialData.uvTransformMatrix = uvTransformMatrix;

		materialData.isLightingEnable = isLighting;

		materialBuffer_->SetData(materialData, constantBufferCount_);

		const float kLonEvery =
			std::numbers::pi_v<float> *2.0f / Cast::Float(subdivision);
		const float kLatEvery = std::numbers::pi_v<float> / Cast::Float(subdivision);

		VertexData pointA{}, pointB{}, pointC{}, pointD{};

		float lat = 0.0f;
		float lon = 0.0f;

		uint32_t vertexDataCount = vertexBuffer_->GetDrewCount();
		uint32_t indexDataCount = indexBuffer_->GetDrewCount();
		uint32_t boardCountInSphere = 0;
		uint32_t baseIndex = 0;

		for (uint32_t latIndex = 0; latIndex < subdivision; ++latIndex) {

			lat =
				-(std::numbers::pi_v<float> *0.5f) + kLatEvery * Cast::Float(latIndex);

			for (uint32_t lonIndex = 0; lonIndex < subdivision; ++lonIndex) {

				lon = static_cast<float>(lonIndex) * kLonEvery;

				pointA.position =
					M::Vector4{ cos(lat) * cos(lon), sin(lat), cos(lat) * sin(lon), 0.0f } *
					radius;
				pointA.position.w = 1.0f;
				pointA.texCoord =
					M::Vector2{ Cast::Float(lonIndex) / Cast::Float(subdivision),
							   1.0f - Cast::Float(latIndex) / Cast::Float(subdivision) };
				pointA.normal = { pointA.position.x, pointA.position.y, pointA.position.z };
				pointA.normal.Normalize();

				pointB.position =
					M::Vector4{ cos(lat + kLatEvery) * cos(lon), sin(lat + kLatEvery),
							   cos(lat + kLatEvery) * sin(lon), 0.0f } *
					radius;
				pointB.position.w = 1.0f;
				pointB.texCoord = M::Vector2{
					Cast::Float(lonIndex) / Cast::Float(subdivision),
					1.0f - Cast::Float(latIndex + 1) / Cast::Float(subdivision) };
				pointB.normal = { pointB.position.x, pointB.position.y, pointB.position.z };
				pointB.normal.Normalize();

				pointC.position = M::Vector4{ cos(lat) * cos(lon + kLonEvery), sin(lat),
											 cos(lat) * sin(lon + kLonEvery), 0.0f } *
					radius;
				pointC.position.w = 1.0f;
				pointC.texCoord =
					M::Vector2{ Cast::Float(lonIndex + 1) / Cast::Float(subdivision),
							   1.0f - Cast::Float(latIndex) / Cast::Float(subdivision) };
				pointC.normal = { pointC.position.x, pointC.position.y, pointC.position.z };
				pointC.normal.Normalize();

				pointD.position =
					M::Vector4{ cos(lat + kLatEvery) * cos(lon + kLonEvery),
							   sin(lat + kLatEvery),
							   cos(lat + kLatEvery) * sin(lon + kLonEvery), 0.0f } *
					radius;
				pointD.position.w = 1.0f;
				pointD.texCoord = M::Vector2{
					Cast::Float(lonIndex + 1) / Cast::Float(subdivision),
					1.0f - Cast::Float(latIndex + 1) / Cast::Float(subdivision) };
				pointD.normal = { pointD.position.x, pointD.position.y, pointD.position.z };
				pointD.normal.Normalize();

				vertexBuffer_->SetVertexData(pointA, vertexDataCount++);

				vertexBuffer_->SetVertexData(pointB, vertexDataCount++);

				vertexBuffer_->SetVertexData(pointC, vertexDataCount++);

				vertexBuffer_->SetVertexData(pointD, vertexDataCount++);

				// 0, 1, 2, 1, 3, 2の順を繰り返す
				// インスタンス内で頂点インデックスを0から数える

				baseIndex = boardCountInSphere * 4;

				indexBuffer_->SetIndexData(baseIndex, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 1, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 2, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 1, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 3, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 2, indexDataCount++);

				boardCountInSphere++;
			}
		}

		const uint32_t sphereIndexCount =
			indexDataCount - indexBuffer_->GetDrewCount();
		const uint32_t sphereVertexCount =
			vertexDataCount - vertexBuffer_->GetDrewCount();

#ifdef DEVELOPMENT

		// #region agent log
		{
			static uint32_t sphereLogCount = 0;
			if (sphereLogCount < 5) {
				const bool vertexWriteOverflow = vertexDataCount > kVertexMaxDrawCount;
				const bool indexWriteOverflow = indexDataCount > kVertexMaxDrawCount;
				AgentDebugLogDraw(
					"Draw.cpp:DrawSphere",
					"sphere batch sizes",
					"A,E",
					std::format(
						R"({{"subdivision":{},"sphereVertexCount":{},"sphereIndexCount":{},"vertexWriteIndex":{},"indexWriteIndex":{},"vertexMax":{},"constantBufferIndex":{},"vertexWriteOverflow":{},"indexWriteOverflow":{}}})",
						subdivision, sphereVertexCount, sphereIndexCount, vertexDataCount,
						indexDataCount, kVertexMaxDrawCount, constantBufferCount_,
						vertexWriteOverflow, indexWriteOverflow));
				++sphereLogCount;
			}
		}
		// #endregion

#endif

		this->DrawCall(textureIndex, sphereIndexCount, sphereVertexCount,
			directionalLightAddress);

	}

	namespace {

	// 行ベクトル規約（mul(v, M)）向けのカメラ正対ビルボード行列
	M::Matrix4x4 MakeCameraFacingBillboardWorld(const M::Vector3& worldPosition,
		const M::Vector3& worldScale,
		const M::Matrix4x4& viewMatrix) {

		M::Matrix4x4 rotation = M::Matrix4x4::Identity();
		rotation.m[0][0] = viewMatrix.m[0][0];
		rotation.m[0][1] = viewMatrix.m[1][0];
		rotation.m[0][2] = viewMatrix.m[2][0];
		rotation.m[1][0] = viewMatrix.m[0][1];
		rotation.m[1][1] = viewMatrix.m[1][1];
		rotation.m[1][2] = viewMatrix.m[2][1];
		rotation.m[2][0] = viewMatrix.m[0][2];
		rotation.m[2][1] = viewMatrix.m[1][2];
		rotation.m[2][2] = viewMatrix.m[2][2];

		const M::Vector3 halfScale{
			std::max(worldScale.x, 1e-5f) * 0.5f,
			std::max(worldScale.y, 1e-5f) * 0.5f,
			1.0f,
		};

		return M::Matrix4x4::Scale(halfScale) * rotation *
			M::Matrix4x4::Translate(worldPosition);
	}

	M::Vector4 TransformPoint(const M::Matrix4x4& matrix, const M::Vector4& point) {
		M::Vector4 result{};
		for (size_t column = 0; column < 4; ++column) {
			result.x += point.x * matrix.m[0][column];
			result.y += point.y * matrix.m[1][column];
			result.z += point.z * matrix.m[2][column];
			result.w += point.w * matrix.m[3][column];
		}
		return result;
	}

	M::Vector3 TransformNormal(const M::Matrix4x4& matrix, const M::Vector3& normal) {
		const M::Vector4 transformed =
			TransformPoint(matrix, M::Vector4{ normal.x, normal.y, normal.z, 0.0f });
		M::Vector3 result{ transformed.x, transformed.y, transformed.z };
		result.Normalize();
		return result;
	}

	} // namespace

	void Draw::DrawBillboard(const uint32_t& textureIndex,
		const Math::Vector4& textureColor,
		const Math::Transform& uvTransform,
		const Math::Vector3& worldPosition,
		const Math::Vector3& worldScale,
		const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (camera_ == nullptr) {
			assert(false && "DrawBillboard: SetCamera を呼んでから描画してください");
			return;
		}

		const M::Matrix4x4 worldMatrix = MakeCameraFacingBillboardWorld(
			worldPosition, worldScale, camera_->GetViewMatrix());

		TransformationData transformationData{};
		transformationData.wvp =
			worldMatrix * camera_->GetViewProjectionMatrix();
		transformationData.world = worldMatrix;

		transformationBuffer_->SetData(transformationData, constantBufferCount_);

		MaterialData materialData{};
		materialData.color = textureColor;

		M::Matrix4x4 uvTransformMatrix = M::Matrix4x4::Scale(uvTransform.scale);
		uvTransformMatrix *= M::Matrix4x4::RotateZ(uvTransform.rotate.z);
		uvTransformMatrix *= M::Matrix4x4::Translate(uvTransform.translate);
		materialData.uvTransformMatrix = uvTransformMatrix;
		materialData.isLightingEnable = isLighting;
		materialData.isUseTexture = true;

		materialBuffer_->SetData(materialData, constantBufferCount_);

		const M::Vector3 faceNormal =
			TransformNormal(worldMatrix, M::Vector3{ 0.0f, 0.0f, 1.0f });

		const std::array<M::Vector4, 4> localPositions{
			M::Vector4{ -1.0f, -1.0f, 0.0f, 1.0f },
			M::Vector4{ 1.0f, -1.0f, 0.0f, 1.0f },
			M::Vector4{ -1.0f, 1.0f, 0.0f, 1.0f },
			M::Vector4{ 1.0f, 1.0f, 0.0f, 1.0f },
		};

		const std::array<M::Vector2, 4> texCoords{
			M::Vector2{ 0.0f, 1.0f },
			M::Vector2{ 1.0f, 1.0f },
			M::Vector2{ 0.0f, 0.0f },
			M::Vector2{ 1.0f, 0.0f },
		};

		const uint32_t vertexStart = vertexBuffer_->GetDrewCount();
		for (size_t i = 0; i < localPositions.size(); ++i) {
			VertexData vertex{};
			vertex.position = TransformPoint(worldMatrix, localPositions[i]);
			vertex.texCoord = texCoords[i];
			vertex.normal = faceNormal;
			vertexBuffer_->SetVertexData(vertex, vertexStart + static_cast<uint32_t>(i));
		}

		const uint32_t indexStart = indexBuffer_->GetDrewCount();
		const std::array<uint32_t, 6> indices{ 0, 1, 2, 1, 3, 2 };
		for (size_t i = 0; i < indices.size(); ++i) {
			indexBuffer_->SetIndexData(vertexStart + indices[i], indexStart + static_cast<uint32_t>(i));
		}

		this->DrawCall(textureIndex, 6, 4, directionalLightAddress);
	}

	void Draw::DrawAsymmetricPyramid(
		const uint32_t& textureIndex, const M::Vector4& textureColor,
		const M::Transform& uvTransform, const M::Vector3& scale,
		const M::Quaternion& rotate, const M::Vector3& translate,
		const G::PyramidMesh& mesh, const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

	  // 非対称ピラミッドのM::Transform
		M::Matrix4x4 worldMatrix = M::Matrix4x4::Scale(scale) *
			rotate.MakeRotateMatrixRh() *
			M::Matrix4x4::Translate(translate);

		TransformationData transformationData{};

		transformationData.wvp = worldMatrix * camera_->GetViewProjectionMatrix();
		transformationData.world = worldMatrix;

		transformationBuffer_->SetData(transformationData, constantBufferCount_);

		MaterialData materialData{};

		materialData.color = textureColor;

		M::Matrix4x4 uvTransformMatrix = M::Matrix4x4::Scale(uvTransform.scale);
		uvTransformMatrix *= M::Matrix4x4::RotateZ(uvTransform.rotate.z);
		uvTransformMatrix *= M::Matrix4x4::Translate(uvTransform.translate);
		materialData.uvTransformMatrix = uvTransformMatrix;

		materialData.isLightingEnable = isLighting;

		materialBuffer_->SetData(materialData, constantBufferCount_);

		// 描画関数内の書き込み処理
		uint32_t vStart = vertexBuffer_->GetDrewCount();

		for (const auto& vertex : mesh.renderVertices) {

			vertexBuffer_->SetVertexData(
				{ { vertex.position.x, vertex.position.y, vertex.position.z, 1.0f },
				 { 0.0f, 0.0f },
				 vertex.normal },
				vStart++);
		}

		// 描画実行 (mesh.indices.size() で数を確認)
		this->DrawCall(textureIndex, (uint32_t)mesh.renderVertices.size(),
			directionalLightAddress);
	}

	void Draw::RestoreBatchGeometryBindings() {
		// AssetModel::Draw はメッシュ固有 VB を IA に載せる。DrawSphere 等に戻す
		pCommandContextDirect_->GetCommandList()->IASetVertexBuffers(
		    0, 1, vertexBuffer_->PGetVertexBufferView());
		pCommandContextDirect_->GetCommandList()->IASetIndexBuffer(
		    indexBuffer_->PGetBufferView());
		pCommandContextDirect_->GetCommandList()->IASetPrimitiveTopology(
		    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Draw::DrawModel(AssetModel* model, const M::Transform& transform,
		const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		model->Draw(transform.MakeWorldMatrix(), camera_->GetViewMatrix(), directionalLightAddress,
			pCommandContextDirect_->GetCommandList(), pSrvAllocator_,
			camera_->GetPerspectiveFovMatrix(), isLighting, Math::Vector4::White(), constantBufferCount_, materialBuffer_.get(), transformationBuffer_.get());
		RestoreBatchGeometryBindings();
	}

	void Draw::DrawModel(AssetModel* model, const Math::Matrix4x4& worldMatrix,
		const Math::Vector4& color, const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		model->Draw(worldMatrix, camera_->GetViewMatrix(), directionalLightAddress,
			pCommandContextDirect_->GetCommandList(), pSrvAllocator_,
			camera_->GetPerspectiveFovMatrix(), isLighting, color, constantBufferCount_, materialBuffer_.get(), transformationBuffer_.get());
		RestoreBatchGeometryBindings();
	}

	void Draw::Finalize() {}

} // namespace Atrum