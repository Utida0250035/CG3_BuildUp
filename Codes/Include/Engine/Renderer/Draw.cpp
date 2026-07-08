#include "Cast/StaticCast.h"
#include "Engine/AssetModel.h"
#include "Engine/CommandContext.h"
#include "Engine/DescriptorAllocator.h" 
#include "Engine/RootSignature.h"
#include "Engine/PipelineState.h"
#include "Engine/Draw.h"
#include "Engine/IndexBuffer.h"
#include "Engine/SwapChain.h"
#include "Engine/VertexBuffer.h"
#include "Geometry/PyramidMesh.h"
#include "Math/Quaternion.h"

#include <numbers>

Draw* Draw::instance_ = nullptr;

void Draw::Initialize(ID3D12Device* device, CommandContext* commandContextDirect, DescriptorAllocator* srvAllocator) {

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

	transformationBuffer_ = std::make_unique<MultiConstantBuffer<TransformationData>>();

	transformationBuffer_->CreateBuffer(pDevice_, kMaxDrawCount);

}

void Draw::DrawTriangleCall(const uint32_t& textureIndex, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

	assert(constantBufferCount_ + 1 < kMaxDrawCount && "constantBufferCount over maxCount(Sphere)");

	// TransformMatrix (WVP) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformationBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(TransformationData));
	// GPUに設定(rootParameter0)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// Material (Color) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(MaterialData));
	// GPUに設定(rootParameter1)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	// GPUに設定(rootParameter3)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightAddress);

	DescriptorAllocator::DescriptorHandle textureHandle{};

	textureHandle = pSrvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 rootParameter[2]
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 3頂点で1つのインスタンス
	pCommandContextDirect_->GetCommandList()->DrawIndexedInstanced(3, 1, indexBuffer_->GetDrewCount(), vertexBuffer_->GetDrewCount(), 0);

	vertexBuffer_->AddDrewCount(3);
	indexBuffer_->AddDrewCount(3);

	constantBufferCount_++;

}

void Draw::DrawCall(const uint32_t& textureIndex, const uint32_t& indexDataCountInInstance, const uint32_t& vertexCountInInstance, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

	assert(constantBufferCount_ + 1 < kMaxDrawCount && "constantBufferCount over maxCount(Sphere)");

	// TransformMatrix (WVP) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformationBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(TransformationData));
	// GPUに設定(rootParameter0)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// Material (Color) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(MaterialData));

	// GPUに設定(rootParameter1)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	// GPUに設定(rootParameter3)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightAddress);

	DescriptorAllocator::DescriptorHandle textureHandle{};

	textureHandle = pSrvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 3頂点で1つのインスタンス
	pCommandContextDirect_->GetCommandList()->DrawIndexedInstanced(indexDataCountInInstance, 1, indexBuffer_->GetDrewCount(), vertexBuffer_->GetDrewCount(), 0);

	indexBuffer_->AddDrewCount(indexDataCountInInstance);

	vertexBuffer_->AddDrewCount(vertexCountInInstance);

	constantBufferCount_++;

}


void Draw::DrawCall(const uint32_t& textureIndex, const uint32_t& vertexCountInInstance, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

	assert(constantBufferCount_ + 1 < kMaxDrawCount && "constantBufferCount over maxCount(Sphere)");

	// TransformMatrix (WVP) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformationBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(TransformationData));
	// GPUに設定(rootParameter0)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// Material (Color) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(MaterialData));

	// GPUに設定(rootParameter1)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	// GPUに設定(rootParameter3)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightAddress);

	DescriptorAllocator::DescriptorHandle textureHandle{};

	textureHandle = pSrvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 3頂点で1つのインスタンス
	pCommandContextDirect_->GetCommandList()->DrawInstanced(vertexCountInInstance, 1, vertexBuffer_->GetDrewCount(), 0);

	vertexBuffer_->AddDrewCount(vertexCountInInstance);

	constantBufferCount_++;

}

void Draw::DrawTriangle(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& triangleTransform, const std::array<VertexData, 3>& vertexData, const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

	// 三角形のTransform
	Matrix4x4 triangleWorldMatrix = MakeWorldMatrix(triangleTransform);


	TransformationData transformationData{};

	transformationData.wvp = triangleWorldMatrix * viewMatrix_ * kPerspectiveFovMatrix;
	transformationData.world = triangleWorldMatrix;

	transformationBuffer_->SetData(transformationData, constantBufferCount_);


	MaterialData materialData{};

	materialData.color = textureColor;

	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix *= MakeZRotateMatrix(uvTransform.rotate.z);
	uvTransformMatrix *= MakeTranslateMatrix(uvTransform.translate);
	materialData.uvTransformMatrix = uvTransformMatrix;

	materialData.inLightingEnable = isLighting;

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

void Draw::DrawSphere(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& sphereTransform, const float radius, const uint32_t subdivision, const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

	// 球のTransform
	Matrix4x4 sphereWorldMatrix = MakeWorldMatrix(sphereTransform);

	TransformationData transformationData{};

	transformationData.wvp = sphereWorldMatrix * viewMatrix_ * kPerspectiveFovMatrix;
	transformationData.world = sphereWorldMatrix;

	transformationBuffer_->SetData(transformationData, constantBufferCount_);


	MaterialData materialData{};

	materialData.color = textureColor;

	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix *= MakeZRotateMatrix(uvTransform.rotate.z);
	uvTransformMatrix *= MakeTranslateMatrix(uvTransform.translate);
	materialData.uvTransformMatrix = uvTransformMatrix;

	materialData.inLightingEnable = isLighting;

	materialBuffer_->SetData(materialData, constantBufferCount_);


	const float kLonEvery = std::numbers::pi_v<float> *2.0f / cast::Float(subdivision);
	const float kLatEvery = std::numbers::pi_v<float> / cast::Float(subdivision);

	VertexData pointA{}, pointB{}, pointC{}, pointD{};

	float lat = 0.0f;
	float lon = 0.0f;

	uint32_t vertexDataCount = vertexBuffer_->GetDrewCount();
	uint32_t indexDataCount = indexBuffer_->GetDrewCount();
	uint32_t boardCountInSphere = 0;
	uint32_t baseIndex = 0;

	for (uint32_t latIndex = 0; latIndex < subdivision; ++latIndex) {

		lat = -(std::numbers::pi_v<float> *0.5f) + kLatEvery * cast::Float(latIndex);

		for (uint32_t lonIndex = 0; lonIndex < subdivision; ++lonIndex) {

			lon = static_cast<float>(lonIndex) * kLonEvery;

			pointA.position = Vector4{ cos(lat) * cos(lon), sin(lat), cos(lat) * sin(lon), 0.0f } * radius;
			pointA.position.w = 1.0f;
			pointA.texCoord = Vector2{ cast::Float(lonIndex) / cast::Float(subdivision), 1.0f - cast::Float(latIndex) / cast::Float(subdivision) };
			pointA.normal = VectorNormalize(Vector3{ pointA.position.x, pointA.position.y, pointA.position.z });

			pointB.position = Vector4{ cos(lat + kLatEvery) * cos(lon), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon), 0.0f } * radius;
			pointB.position.w = 1.0f;
			pointB.texCoord = Vector2{ cast::Float(lonIndex) / cast::Float(subdivision), 1.0f - cast::Float(latIndex + 1) / cast::Float(subdivision) };
			pointB.normal = VectorNormalize(Vector3{ pointB.position.x, pointB.position.y, pointB.position.z });

			pointC.position = Vector4{ cos(lat) * cos(lon + kLonEvery), sin(lat), cos(lat) * sin(lon + kLonEvery) , 0.0f } * radius;
			pointC.position.w = 1.0f;
			pointC.texCoord = Vector2{ cast::Float(lonIndex + 1) / cast::Float(subdivision), 1.0f - cast::Float(latIndex) / cast::Float(subdivision) };
			pointC.normal = VectorNormalize(Vector3{ pointC.position.x, pointC.position.y, pointC.position.z });

			pointD.position = Vector4{ cos(lat + kLatEvery) * cos(lon + kLonEvery), sin(lat + kLatEvery), cos(lat + kLatEvery) * sin(lon + kLonEvery), 0.0f } * radius;
			pointD.position.w = 1.0f;
			pointD.texCoord = Vector2{ cast::Float(lonIndex + 1) / cast::Float(subdivision), 1.0f - cast::Float(latIndex + 1) / cast::Float(subdivision) };
			pointD.normal = VectorNormalize(Vector3{ pointD.position.x, pointD.position.y, pointD.position.z });


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

	this->DrawCall(textureIndex, indexDataCount - indexBuffer_->GetDrewCount(), vertexDataCount - vertexBuffer_->GetDrewCount(), directionalLightAddress);

}

void Draw::DrawAsymmetricPyramid(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Vector3& scale, const Quaternion& rotate, const Vector3& translate, const PyramidMesh& mesh, const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

	// 非対称ピラミッドのTransform
	Matrix4x4 worldMatrix = MakeScaleMatrix(scale) * rotate.create_rotate_matrix() * MakeTranslateMatrix(translate);

	TransformationData transformationData{};

	transformationData.wvp = worldMatrix * viewMatrix_ * kPerspectiveFovMatrix;
	transformationData.world = worldMatrix;

	transformationBuffer_->SetData(transformationData, constantBufferCount_);


	MaterialData materialData{};

	materialData.color = textureColor;

	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix *= MakeZRotateMatrix(uvTransform.rotate.z);
	uvTransformMatrix *= MakeTranslateMatrix(uvTransform.translate);
	materialData.uvTransformMatrix = uvTransformMatrix;

	materialData.inLightingEnable = isLighting;

	materialBuffer_->SetData(materialData, constantBufferCount_);


	// 描画関数内の書き込み処理
	uint32_t vStart = vertexBuffer_->GetDrewCount();

	for (const auto& vertex : mesh.renderVertices) {

		vertexBuffer_->SetVertexData(
			{
				{vertex.position.x, vertex.position.y, vertex.position.z, 1.0f},
				{0.0f, 0.0f},
				vertex.normal
			},
			vStart++
		);

	}

	// 描画実行 (mesh.indices.size() で数を確認)
	this->DrawCall(textureIndex, (uint32_t)mesh.renderVertices.size(), directionalLightAddress);

}

void Draw::DrawModel(AssetModel* model, const Transform& transform, const bool isLighting, D3D12_GPU_VIRTUAL_ADDRESS directionalLightAddress) {

	model->Draw(transform, viewMatrix_, directionalLightAddress, pCommandContextDirect_->GetCommandList(), pSrvAllocator_, kPerspectiveFovMatrix, isLighting);

}