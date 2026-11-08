#include "Engine/Alias/CoreAlias.h"

#include "Cast/StaticCast.h"
#include "Engine/Command/CommandContext.h"
#include "Engine/Device/SwapChain.h"
#include "Engine/Pipeline/PipelineState.h"
#include "Engine/Pipeline/RootSignature.h"
#include "Engine/Renderer/Draw.h"
#include "Engine/Renderer/Line3DGeometry.h"
#include "Engine/Resource/AssetModel.h"
#include "Engine/Resource/CreateBufferResource.h"
#include "Engine/Resource/DescriptorAllocator.h"
#include "Engine/Resource/IndexBuffer.h"
#include "Engine/Resource/ParticleGpuData.h"
#include "Engine/Resource/VertexBuffer.h"
#include "Engine/Renderer/BillboardMatrix.h"
#include "Geometry/PyramidMesh.h"
#include "Camera/Camera.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <functional>
#include <numbers>
#include <vector>
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

	std::unique_ptr<Draw> Draw::instance_ = nullptr;

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

		CreateParticleGpuResources();

		CreateParticleInstanceSrv();
	}

	void Draw::RegisterObject3dAndParticlePipelines(
	    const std::array<ID3D12PipelineState *,
	                     static_cast<size_t>(BlendMode::MODE_COUNT)>
	        &object3dPipelines,
	    const std::array<ID3D12PipelineState *,
	                     static_cast<size_t>(BlendMode::MODE_COUNT)>
	        &particlePipelines) {
		object3dPipelineByBlend_ = object3dPipelines;
		particlePipelineByBlend_ = particlePipelines;
	}

	void Draw::CreateParticleGpuResources() {
		particleFrameBuffer_ =
		    std::make_unique<SingleConstantBuffer<ParticleFrameGpuData>>();
		particleFrameBuffer_->CreateBuffer(pDevice_);

		particleInstanceCapacity_ = kMaxParticleInstances;
		particleInstanceBufferResource_ = CreateUploadBuffer(
		    sizeof(ParticleInstanceGpuData) * particleInstanceCapacity_,
		    pDevice_);
		particleInstanceBufferResource_->Map(
		    0, nullptr, reinterpret_cast<void **>(&particleInstanceData_));
		std::memset(particleInstanceData_, 0,
		            sizeof(ParticleInstanceGpuData) *
		                particleInstanceCapacity_);
	}

	void Draw::CreateParticleInstanceSrv() {
		particleInstanceSrv_ = pSrvAllocator_->Allocate();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Shader4ComponentMapping =
		    D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements =
		    static_cast<uint32_t>(particleInstanceCapacity_);
		srvDesc.Buffer.StructureByteStride =
		    sizeof(ParticleInstanceGpuData);
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		pDevice_->CreateShaderResourceView(particleInstanceBufferResource_.Get(),
		                                   &srvDesc, particleInstanceSrv_.cpu);
	}

	void Draw::WriteSphereMeshToBatch(const uint32_t subdivision,
	                                  const float /*radius*/,
	                                  uint32_t vertexWriteStart,
	                                  uint32_t indexWriteStart,
	                                  uint32_t &outVertexCount,
	                                  uint32_t &outIndexCount) {
		namespace M = Math;

		const float kLonEvery =
		    std::numbers::pi_v<float> * 2.0f / Cast::Float(subdivision);
		const float kLatEvery =
		    std::numbers::pi_v<float> / Cast::Float(subdivision);

		VertexData pointA{}, pointB{}, pointC{}, pointD{};

		uint32_t vertexDataCount = vertexWriteStart;
		uint32_t indexDataCount = indexWriteStart;
		uint32_t boardCountInSphere = 0;
		uint32_t baseIndex = 0;

		for (uint32_t latIndex = 0; latIndex < subdivision; ++latIndex) {
			const float lat = -(std::numbers::pi_v<float> * 0.5f) +
			                  kLatEvery * Cast::Float(latIndex);

			for (uint32_t lonIndex = 0; lonIndex < subdivision; ++lonIndex) {
				const float lon = static_cast<float>(lonIndex) * kLonEvery;

				pointA.position =
				    M::Vector4{cos(lat) * cos(lon), sin(lat), cos(lat) * sin(lon),
				               0.0f};
				pointA.position.w = 1.0f;
				pointA.texCoord =
				    M::Vector2{Cast::Float(lonIndex) / Cast::Float(subdivision),
				               1.0f -
				                   Cast::Float(latIndex) / Cast::Float(subdivision)};
				pointA.normal = {pointA.position.x, pointA.position.y,
				                 pointA.position.z};
				pointA.normal.Normalize();

				pointB.position =
				    M::Vector4{cos(lat + kLatEvery) * cos(lon),
				               sin(lat + kLatEvery),
				               cos(lat + kLatEvery) * sin(lon), 0.0f};
				pointB.position.w = 1.0f;
				pointB.texCoord =
				    M::Vector2{Cast::Float(lonIndex) / Cast::Float(subdivision),
				               1.0f - Cast::Float(latIndex + 1) /
				                          Cast::Float(subdivision)};
				pointB.normal = {pointB.position.x, pointB.position.y,
				                 pointB.position.z};
				pointB.normal.Normalize();

				pointC.position = M::Vector4{cos(lat) * cos(lon + kLonEvery), sin(lat),
				                             cos(lat) * sin(lon + kLonEvery), 0.0f};
				pointC.position.w = 1.0f;
				pointC.texCoord = M::Vector2{
				    Cast::Float(lonIndex + 1) / Cast::Float(subdivision),
				    1.0f - Cast::Float(latIndex) / Cast::Float(subdivision)};
				pointC.normal = {pointC.position.x, pointC.position.y,
				                 pointC.position.z};
				pointC.normal.Normalize();

				pointD.position =
				    M::Vector4{cos(lat + kLatEvery) * cos(lon + kLonEvery),
				               sin(lat + kLatEvery),
				               cos(lat + kLatEvery) * sin(lon + kLonEvery), 0.0f};
				pointD.position.w = 1.0f;
				pointD.texCoord = M::Vector2{
				    Cast::Float(lonIndex + 1) / Cast::Float(subdivision),
				    1.0f - Cast::Float(latIndex + 1) / Cast::Float(subdivision)};
				pointD.normal = {pointD.position.x, pointD.position.y,
				                 pointD.position.z};
				pointD.normal.Normalize();

				vertexBuffer_->SetVertexData(pointA, vertexDataCount++);
				vertexBuffer_->SetVertexData(pointB, vertexDataCount++);
				vertexBuffer_->SetVertexData(pointC, vertexDataCount++);
				vertexBuffer_->SetVertexData(pointD, vertexDataCount++);

				baseIndex = boardCountInSphere * 4;

				indexBuffer_->SetIndexData(baseIndex, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 1, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 2, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 1, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 3, indexDataCount++);
				indexBuffer_->SetIndexData(baseIndex + 2, indexDataCount++);

				++boardCountInSphere;
			}
		}

		outVertexCount = vertexDataCount - vertexWriteStart;
		outIndexCount = indexDataCount - indexWriteStart;
	}

	void Draw::WriteBillboardQuadToBatch(uint32_t vertexWriteStart,
	                                     uint32_t indexWriteStart) {
		namespace M = Math;

		const M::Vector3 faceNormal{0.0f, 0.0f, 1.0f};

		const std::array<M::Vector4, 4> localPositions{
		    M::Vector4{-1.0f, -1.0f, 0.0f, 1.0f},
		    M::Vector4{1.0f, -1.0f, 0.0f, 1.0f},
		    M::Vector4{-1.0f, 1.0f, 0.0f, 1.0f},
		    M::Vector4{1.0f, 1.0f, 0.0f, 1.0f},
		};

		const std::array<M::Vector2, 4> texCoords{
		    M::Vector2{0.0f, 1.0f},
		    M::Vector2{1.0f, 1.0f},
		    M::Vector2{0.0f, 0.0f},
		    M::Vector2{1.0f, 0.0f},
		};

		for (size_t i = 0; i < localPositions.size(); ++i) {
			VertexData vertex{};
			vertex.position = localPositions[i];
			vertex.texCoord = texCoords[i];
			vertex.normal = faceNormal;
			vertexBuffer_->SetVertexData(
			    vertex, vertexWriteStart + static_cast<uint32_t>(i));
		}

		const std::array<uint32_t, 6> indices{0, 2, 1, 2, 3, 1};
		for (size_t i = 0; i < indices.size(); ++i) {
			indexBuffer_->SetIndexData(indices[i],
			                           indexWriteStart +
			                               static_cast<uint32_t>(i));
		}
	}

	void Draw::DrawParticleIndexedInstanced(
	    uint32_t textureIndex, uint32_t indexCountPerInstance,
	    uint32_t vertexCountPerInstance, uint32_t startVertexLocation,
	    uint32_t startIndexLocation, int32_t baseVertexLocation,
	    uint32_t instanceCount, uint32_t instanceBufferBase,
	    D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress,
	    const MaterialData *materialOverride) {

		(void)startVertexLocation;

		if (camera_ == nullptr || instanceCount == 0) {
			return;
		}

		if (constantBufferCount_ + 1 >= kMaxDrawCount) {
			return;
		}

		const size_t blendIndex = static_cast<size_t>(activeBlendMode_);
		ID3D12PipelineState *particlePso =
		    particlePipelineByBlend_[blendIndex];
		ID3D12PipelineState *objectPso =
		    object3dPipelineByBlend_[blendIndex];
		if (particlePso == nullptr || objectPso == nullptr) {
			assert(false && "DrawParticleIndexedInstanced: PSO が未登録です");
			return;
		}

		camera_->UpdateMatrix();

		ParticleFrameGpuData frame{};
		frame.viewProjection = camera_->GetViewProjectionMatrix();
		frame.view = camera_->GetViewMatrix();
		const M::Matrix4x4 invView = frame.view.Inversed();
		frame.cameraWorld =
		    M::Vector4{invView.m[3][0], invView.m[3][1], invView.m[3][2], 1.0f};
		frame.instanceBufferBase = instanceBufferBase;
		particleFrameBuffer_->SetData(frame);

		MaterialData materialData{};
		if (materialOverride != nullptr) {
			materialData = *materialOverride;
		} else {
			materialData.color = M::Vector4::White();
			materialData.uvTransformMatrix = M::Matrix4x4::Identity();
			materialData.isLightingEnable = false;
			materialData.isUseTexture = true;
		}
		materialBuffer_->SetData(materialData, constantBufferCount_);

		D3D12_GPU_VIRTUAL_ADDRESS materialAddr =
		    materialBuffer_->GetGpuVirtualAddress() +
		    (constantBufferCount_ * sizeof(MaterialData));

		auto *cmd = pCommandContextDirect_->GetCommandList();

		cmd->SetGraphicsRootConstantBufferView(0, materialAddr);
		cmd->SetGraphicsRootConstantBufferView(
		    4, particleFrameBuffer_->GetGpuVirtualAddress());
		cmd->SetGraphicsRootDescriptorTable(5, particleInstanceSrv_.gpu);

		DescriptorAllocator::DescriptorHandle textureHandle =
		    pSrvAllocator_->GetHandle(textureIndex);
		cmd->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

		cmd->SetGraphicsRootConstantBufferView(3, directionalLightAddress);

		cmd->SetPipelineState(particlePso);
		cmd->IASetVertexBuffers(0, 1, vertexBuffer_->PGetVertexBufferView());
		cmd->IASetIndexBuffer(indexBuffer_->PGetBufferView());
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		(void)vertexCountPerInstance;

		cmd->DrawIndexedInstanced(indexCountPerInstance, instanceCount,
		                          startIndexLocation, baseVertexLocation, 0);

		cmd->SetPipelineState(objectPso);
		cmd->SetGraphicsRootConstantBufferView(
		    0, materialBuffer_->GetGpuVirtualAddress());
		cmd->SetGraphicsRootConstantBufferView(
		    1, transformationBuffer_->GetGpuVirtualAddress());
		cmd->SetGraphicsRootConstantBufferView(
		    3, directionalLightAddress);

		constantBufferCount_++;
	}

	void Draw::DrawParticleNonIndexedInstanced(
	    uint32_t textureIndex, const D3D12_VERTEX_BUFFER_VIEW *meshVbv,
	    uint32_t vertexCountPerInstance, uint32_t instanceCount,
	    uint32_t instanceBufferBase,
	    D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress,
	    const MaterialData *materialOverride) {

		if (camera_ == nullptr || meshVbv == nullptr || instanceCount == 0) {
			return;
		}

		if (constantBufferCount_ + 1 >= kMaxDrawCount) {
			return;
		}

		const size_t blendIndex = static_cast<size_t>(activeBlendMode_);
		ID3D12PipelineState *particlePso =
		    particlePipelineByBlend_[blendIndex];
		ID3D12PipelineState *objectPso =
		    object3dPipelineByBlend_[blendIndex];
		if (particlePso == nullptr || objectPso == nullptr) {
			assert(false && "DrawParticleNonIndexedInstanced: PSO が未登録です");
			return;
		}

		camera_->UpdateMatrix();

		ParticleFrameGpuData frame{};
		frame.viewProjection = camera_->GetViewProjectionMatrix();
		frame.view = camera_->GetViewMatrix();
		const M::Matrix4x4 invView = frame.view.Inversed();
		frame.cameraWorld =
		    M::Vector4{invView.m[3][0], invView.m[3][1], invView.m[3][2], 1.0f};
		frame.instanceBufferBase = instanceBufferBase;
		particleFrameBuffer_->SetData(frame);

		MaterialData materialData{};
		if (materialOverride != nullptr) {
			materialData = *materialOverride;
		} else {
			materialData.color = M::Vector4::White();
			materialData.uvTransformMatrix = M::Matrix4x4::Identity();
			materialData.isLightingEnable = false;
			materialData.isUseTexture = true;
		}
		materialBuffer_->SetData(materialData, constantBufferCount_);

		D3D12_GPU_VIRTUAL_ADDRESS materialAddr =
		    materialBuffer_->GetGpuVirtualAddress() +
		    (constantBufferCount_ * sizeof(MaterialData));

		auto *cmd = pCommandContextDirect_->GetCommandList();

		cmd->SetGraphicsRootConstantBufferView(0, materialAddr);
		cmd->SetGraphicsRootConstantBufferView(
		    4, particleFrameBuffer_->GetGpuVirtualAddress());
		cmd->SetGraphicsRootDescriptorTable(5, particleInstanceSrv_.gpu);

		DescriptorAllocator::DescriptorHandle textureHandle =
		    pSrvAllocator_->GetHandle(textureIndex);
		cmd->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

		cmd->SetGraphicsRootConstantBufferView(3, directionalLightAddress);

		cmd->SetPipelineState(particlePso);
		cmd->IASetVertexBuffers(0, 1, meshVbv);

		D3D12_INDEX_BUFFER_VIEW nullIndexView{};
		nullIndexView.BufferLocation = 0;
		nullIndexView.SizeInBytes = 0;
		nullIndexView.Format = DXGI_FORMAT_UNKNOWN;
		cmd->IASetIndexBuffer(&nullIndexView);

		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cmd->DrawInstanced(vertexCountPerInstance, instanceCount, 0, 0);

		cmd->SetPipelineState(objectPso);
		cmd->SetGraphicsRootConstantBufferView(
		    0, materialBuffer_->GetGpuVirtualAddress());
		cmd->SetGraphicsRootConstantBufferView(
		    1, transformationBuffer_->GetGpuVirtualAddress());
		cmd->SetGraphicsRootConstantBufferView(
		    3, directionalLightAddress);
		cmd->IASetVertexBuffers(0, 1, vertexBuffer_->PGetVertexBufferView());
		cmd->IASetIndexBuffer(indexBuffer_->PGetBufferView());

		constantBufferCount_++;
	}

	void Draw::DrawSphereParticlesInstanced(
	    uint32_t textureIndex, const Math::Transform &uvTransform,
	    const std::vector<ParticleInstanceGpuData> &instances,
	    uint32_t subdivision, bool isLighting,
	    D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (camera_ == nullptr || instances.empty()) {
			return;
		}

		(void)isLighting;
		(void)uvTransform;

		uint32_t geomVertexStart = 0;
		uint32_t geomIndexStart = 0;
		uint32_t sphereVertexCount = 0;
		uint32_t sphereIndexCount = 0;
		bool geometryWritten = false;

		const size_t total = instances.size();
		for (size_t offset = 0; offset < total;
		     offset += kMaxParticleInstances) {
			const uint32_t batchCount = static_cast<uint32_t>(std::min<size_t>(
			    kMaxParticleInstances, total - offset));

			std::memcpy(
			    particleInstanceData_,
			    instances.data() + offset,
			    sizeof(ParticleInstanceGpuData) * batchCount);

			if (!geometryWritten) {
				geomVertexStart = vertexBuffer_->GetDrewCount();
				geomIndexStart = indexBuffer_->GetDrewCount();
				WriteSphereMeshToBatch(
				    subdivision, 1.0f, geomVertexStart, geomIndexStart,
				    sphereVertexCount, sphereIndexCount);
				vertexBuffer_->AddDrewCount(sphereVertexCount);
				indexBuffer_->AddDrewCount(sphereIndexCount);
				geometryWritten = true;
			}

			DrawParticleIndexedInstanced(
			    textureIndex, sphereIndexCount, sphereVertexCount,
			    geomVertexStart, geomIndexStart,
			    static_cast<int32_t>(geomVertexStart), batchCount, 0,
			    directionalLightAddress, nullptr);
		}
	}

	void Draw::DrawBillboardParticlesInstanced(
	    uint32_t textureIndex, const Math::Transform &uvTransform,
	    const std::vector<ParticleInstanceGpuData> &instances,
	    bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (camera_ == nullptr || instances.empty()) {
			return;
		}

		(void)isLighting;
		(void)uvTransform;

		constexpr uint32_t kQuadVertexCount = 4;
		constexpr uint32_t kQuadIndexCount = 6;

		uint32_t geomVertexStart = 0;
		uint32_t geomIndexStart = 0;
		bool geometryWritten = false;

		camera_->UpdateMatrix();

		const size_t total = instances.size();
		for (size_t offset = 0; offset < total;
		     offset += kMaxParticleInstances) {
			const uint32_t batchCount = static_cast<uint32_t>(std::min<size_t>(
			    kMaxParticleInstances, total - offset));

			std::memcpy(
			    particleInstanceData_,
			    instances.data() + offset,
			    sizeof(ParticleInstanceGpuData) * batchCount);

			if (!geometryWritten) {
				geomVertexStart = vertexBuffer_->GetDrewCount();
				geomIndexStart = indexBuffer_->GetDrewCount();
				WriteBillboardQuadToBatch(geomVertexStart, geomIndexStart);
				vertexBuffer_->AddDrewCount(kQuadVertexCount);
				indexBuffer_->AddDrewCount(kQuadIndexCount);
				geometryWritten = true;
			}

			DrawParticleIndexedInstanced(
			    textureIndex, kQuadIndexCount, kQuadVertexCount, geomVertexStart,
			    geomIndexStart, static_cast<int32_t>(geomVertexStart), batchCount,
			    0, directionalLightAddress, nullptr);
		}
	}

	void Draw::DrawModelParticlesInstanced(
	    AssetModel *model, const std::vector<ParticleInstanceGpuData> &instances,
	    bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (camera_ == nullptr || model == nullptr || instances.empty()) {
			return;
		}

		const auto &nodes = model->GetMeshNodes();
		if (nodes.empty()) {
			return;
		}

		const size_t total = instances.size();
		for (size_t offset = 0; offset < total;
		     offset += kMaxParticleInstances) {
			const uint32_t batchCount = static_cast<uint32_t>(std::min<size_t>(
			    kMaxParticleInstances, total - offset));

			std::memcpy(
			    particleInstanceData_,
			    instances.data() + offset,
			    sizeof(ParticleInstanceGpuData) * batchCount);

			for (const auto &node : nodes) {
				if (!node.mesh || !node.material) {
					continue;
				}

				MaterialData meshMaterial{};
				meshMaterial.color = node.material->baseColor;
				meshMaterial.uvTransformMatrix = M::Matrix4x4::Identity();
				meshMaterial.isLightingEnable = isLighting ? 1 : 0;
				meshMaterial.isUseTexture = node.material->isUseTexture ? 1 : 0;

				DrawParticleNonIndexedInstanced(
				    node.material->textureSrvIndex,
				    &node.mesh->vertexBufferView,
				    static_cast<uint32_t>(node.mesh->vertices.size()),
				    batchCount, 0, directionalLightAddress, &meshMaterial);
			}
		}
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
		const M::Matrix4x4& worldMatrix,
		const std::array<VertexData, 3>& vertexData,
		const bool isLighting,
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

	void Draw::DrawBillboard(const uint32_t& textureIndex,
		const Math::Vector4& textureColor,
		const Math::Transform& uvTransform,
		const Math::Matrix4x4& worldMatrix,
		const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (camera_ == nullptr) {
			assert(false && "DrawBillboard: SetCamera を呼んでから描画してください");
			return;
		}

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

		// ローカル +Z が板の表。頂点はローカル座標のまま VS の world/WVP に任せる
		const M::Vector3 faceNormal{worldMatrix.m[2][0], worldMatrix.m[2][1],
		                            worldMatrix.m[2][2]};
		M::Vector3 normalizedFaceNormal = faceNormal;
		normalizedFaceNormal.Normalize();

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
			vertex.position = localPositions[i];
			vertex.texCoord = texCoords[i];
			vertex.normal = normalizedFaceNormal;
			vertexBuffer_->SetVertexData(vertex, vertexStart + static_cast<uint32_t>(i));
		}

		const uint32_t indexStart = indexBuffer_->GetDrewCount();
		// DrawCall の BaseVertexLocation が vertexStart になるため、インデックスは
		// バッチ内相対（DrawSphere と同じ）にする。絶対頂点番号を書くと二重加算で壊れる。
		const std::array<uint32_t, 6> indices{ 0, 2, 1, 2, 3, 1 };
		for (size_t i = 0; i < indices.size(); ++i) {
			indexBuffer_->SetIndexData(indices[i], indexStart + static_cast<uint32_t>(i));
		}

		this->DrawCall(textureIndex, 6, 4, directionalLightAddress);
	}

	void Draw::DrawLine3D(const uint32_t& textureIndex,
		const Math::Vector4& textureColor,
		const Math::Transform& uvTransform,
		const Math::Vector3& worldStart, const Math::Vector3& worldEnd,
		const float width, const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (camera_ == nullptr) {
			assert(false && "DrawLine3D: SetCamera を呼んでから描画してください");
			return;
		}

		std::array<M::Vector3, 4> worldPositions{};
		M::Vector3 faceNormal{};
		if (!Renderer::BuildCameraFacingLineQuad(
				worldStart, worldEnd, width, camera_->GetTranslate(),
				worldPositions, faceNormal)) {
			return;
		}

		const M::Matrix4x4 worldMatrix = M::Matrix4x4::Identity();
		const M::Matrix4x4 viewProjection = camera_->GetViewProjectionMatrix();

		TransformationData transformationData{};
		transformationData.wvp = worldMatrix * viewProjection;
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

		const std::array<M::Vector2, 4> texCoords{
			M::Vector2{0.0f, 1.0f},
			M::Vector2{1.0f, 1.0f},
			M::Vector2{0.0f, 0.0f},
			M::Vector2{1.0f, 0.0f},
		};

		const uint32_t vertexStart = vertexBuffer_->GetDrewCount();
		for (size_t i = 0; i < worldPositions.size(); ++i) {
			const M::Vector3& p = worldPositions[i];
			VertexData vertex{};
			vertex.position = M::Vector4{p.x, p.y, p.z, 1.0f};
			vertex.texCoord = texCoords[i];
			vertex.normal = faceNormal;
			vertexBuffer_->SetVertexData(vertex, vertexStart + static_cast<uint32_t>(i));
		}

		const uint32_t indexStart = indexBuffer_->GetDrewCount();
		const std::array<uint32_t, 6> indices{0, 2, 1, 2, 3, 1};
		for (size_t i = 0; i < indices.size(); ++i) {
			indexBuffer_->SetIndexData(indices[i], indexStart + static_cast<uint32_t>(i));
		}

		this->DrawCall(textureIndex, 6, 4, directionalLightAddress);
	}

	void Draw::DrawLine3DOnFloorPlane(
		const uint32_t& textureIndex, const Math::Vector4& textureColor,
		const Math::Transform& uvTransform, const Math::Vector3& worldStart,
		const Math::Vector3& worldEnd, const float lineWidthPixels,
		const int viewportWidth, const int viewportHeight, const float floorY,
		const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		if (camera_ == nullptr) {
			assert(false &&
			       "DrawLine3DOnFloorPlane: SetCamera を呼んでから描画してください");
			return;
		}

		const M::Matrix4x4 viewProjection =
			camera_->GetViewMatrix() * camera_->GetPerspectiveFovMatrix();

		const auto drawSegment = [&](const M::Vector3 &drawStart,
		                             const M::Vector3 &drawEnd) {
			std::array<M::Vector3, 4> worldPositions{};
			M::Vector3 faceNormal{};
			const bool builtScreenStable =
			    Renderer::BuildScreenStableFloorLineQuad(
			        drawStart, drawEnd, lineWidthPixels, viewportWidth,
			        viewportHeight, camera_->GetTranslate(),
			        camera_->GetViewMatrix(),
			        camera_->GetPerspectiveFovMatrix(), floorY, worldPositions,
			        faceNormal);
			if (!builtScreenStable) {
				const M::Vector3 midPoint = (drawStart + drawEnd) * 0.5f;
				const float metersPerPixel =
				    Renderer::EstimateMetersPerPixelAtWorldPoint(
				        midPoint, camera_->GetTranslate(),
				        camera_->GetPerspectiveFovMatrix(), viewportHeight);
				const float fallbackWorldWidth =
				    std::max(metersPerPixel * lineWidthPixels, 1.0e-3f);
				if (!Renderer::BuildPlanarLineQuadOnFloor(
				        drawStart, drawEnd, fallbackWorldWidth, worldPositions,
				        faceNormal)) {
					return;
				}
			}

			const M::Matrix4x4 worldMatrix = M::Matrix4x4::Identity();

			TransformationData transformationData{};
			transformationData.wvp =
			    worldMatrix * camera_->GetViewProjectionMatrix();
			transformationData.world = worldMatrix;
			transformationBuffer_->SetData(transformationData,
			                               constantBufferCount_);

			MaterialData materialData{};
			materialData.color = textureColor;
			M::Matrix4x4 uvTransformMatrix = M::Matrix4x4::Scale(uvTransform.scale);
			uvTransformMatrix *= M::Matrix4x4::RotateZ(uvTransform.rotate.z);
			uvTransformMatrix *= M::Matrix4x4::Translate(uvTransform.translate);
			materialData.uvTransformMatrix = uvTransformMatrix;
			materialData.isLightingEnable = isLighting;
			materialData.isUseTexture = true;
			materialBuffer_->SetData(materialData, constantBufferCount_);

			const std::array<M::Vector2, 4> texCoords{
			    M::Vector2{0.0f, 1.0f},
			    M::Vector2{1.0f, 1.0f},
			    M::Vector2{0.0f, 0.0f},
			    M::Vector2{1.0f, 0.0f},
			};

			const uint32_t vertexStart = vertexBuffer_->GetDrewCount();
			for (size_t i = 0; i < worldPositions.size(); ++i) {
				const M::Vector3 &p = worldPositions[i];
				VertexData vertex{};
				vertex.position = M::Vector4{p.x, p.y, p.z, 1.0f};
				vertex.texCoord = texCoords[i];
				vertex.normal = faceNormal;
				vertexBuffer_->SetVertexData(
				    vertex, vertexStart + static_cast<uint32_t>(i));
			}

			const uint32_t indexStart = indexBuffer_->GetDrewCount();
			const std::array<uint32_t, 6> indices{0, 2, 1, 2, 3, 1};
			for (size_t i = 0; i < indices.size(); ++i) {
				indexBuffer_->SetIndexData(
				    indices[i], indexStart + static_cast<uint32_t>(i));
			}

			this->DrawCall(textureIndex, 6, 4, directionalLightAddress);
		};

		Renderer::ForEachVisibleLineSubsegment(worldStart, worldEnd,
		                                       viewProjection, drawSegment);
	}

	void Draw::DrawDashedLine3D(const uint32_t& textureIndex,
		const Math::Vector4& textureColor,
		const Math::Transform& uvTransform,
		const Math::Vector3& worldStart, const Math::Vector3& worldEnd,
		const float width, const float dashLength, const float gapLength,
		const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

		const Math::Vector3 delta = worldEnd - worldStart;
		const float totalLength = delta.Length();
		if (totalLength < 1.0e-4f) {
			return;
		}

		const float clampedDash = std::max(dashLength, 1.0e-4f);
		const float clampedGap = std::max(gapLength, 0.0f);
		const Math::Vector3 direction = delta / totalLength;

		float traveled = 0.0f;
		while (traveled < totalLength) {
			const float segmentEndDistance =
				std::min(traveled + clampedDash, totalLength);
			const Math::Vector3 segmentStart =
				worldStart + direction * traveled;
			const Math::Vector3 segmentEnd =
				worldStart + direction * segmentEndDistance;

			this->DrawLine3D(textureIndex, textureColor, uvTransform,
				segmentStart, segmentEnd, width, isLighting,
				directionalLightAddress);

			traveled = segmentEndDistance + clampedGap;
		}
	}

	void Draw::DrawAsymmetricPyramid(
		const uint32_t& textureIndex, const M::Vector4& textureColor,
		const M::Transform& uvTransform, const M::Matrix4x4& worldMatrix,
		const G::PyramidMesh& mesh, const bool isLighting,
		D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

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