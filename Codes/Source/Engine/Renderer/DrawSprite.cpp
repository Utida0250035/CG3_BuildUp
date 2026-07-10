#include "Engine/Renderer/DrawSprite.h"

#include "Engine/Command/CommandContext.h"
#include "Engine/Resource/DescriptorAllocator.h"
#include "Engine/Resource/IndexBuffer.h"
#include "Engine/Resource/VertexBuffer.h"
#include "Math/Transform.h"


DrawSprite* DrawSprite::instance_ = nullptr;

void DrawSprite::Initialize(ID3D12Device* device, CommandContext* commandContextDirect, DescriptorAllocator* srvAllocator, const uint32_t clientWidth, const uint32_t clientHeight) {

	pDevice_ = device;

	pCommandContextDirect_ = commandContextDirect;

	pSrvAllocator_ = srvAllocator;

	orthographicMatrix_ = MakeOrthographicMatrix(0.0f, 0.0f, cast::Float(clientWidth), cast::Float(clientHeight), 0.0f, 100.0f);

	CreateVertexBuffer();

	CreateIndexBuffer();

	CreateMaterialBuffer();

	CreateTransformationBuffer();

}

void DrawSprite::CreateVertexBuffer() {

	vertexBuffer_ = std::make_unique<VertexBuffer>();

	vertexBuffer_->CreateVertexBuffer(kVertexMaxDrawCount, pDevice_);

}

void DrawSprite::CreateIndexBuffer() {

	indexBuffer_ = std::make_unique<IndexBuffer>();

	indexBuffer_->CreateIndexBuffer(kVertexMaxDrawCount, pDevice_);

}

void DrawSprite::CreateMaterialBuffer() {

	materialBuffer_ = std::make_unique<MultiConstantBuffer<MaterialData>>();

	materialBuffer_->CreateBuffer(pDevice_, kMaxDrawCount);

}

void DrawSprite::CreateTransformationBuffer() {

	transformationBuffer_ = std::make_unique<MultiConstantBuffer<TransformationData>>();

	transformationBuffer_->CreateBuffer(pDevice_, kMaxDrawCount);

}

void DrawSprite::DrawSpriteCall(const uint32_t& textureIndex) {

	assert(constantBufferCount_ + 1 < kMaxDrawCount);

	// TransformMatrix (WVP) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr = transformationBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(TransformationData));
	// GPUに設定(rootParameter0)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformOffsetAddr);

	// Material (Color) のアドレス計算
	D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr = materialBuffer_->GetGpuVirtualAddress() + (constantBufferCount_ * sizeof(MaterialData));
	// GPUに設定(rootParameter1)
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialOffsetAddr);

	DescriptorAllocator::DescriptorHandle textureHandle{};
	textureHandle = pSrvAllocator_->GetHandle(textureIndex);

	// SRVのDescriptorTableの先頭を設定 2はrootParameter[2]
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

	// 描画(DrawCall) 6頂点インデックスで1つのインスタンス
	pCommandContextDirect_->GetCommandList()->DrawIndexedInstanced(6, 1, indexBuffer_->GetDrewCount(), vertexBuffer_->GetDrewCount(), 0);

	indexBuffer_->AddDrewCount(6);

	vertexBuffer_->AddDrewCount(4);

	constantBufferCount_++;

}

void DrawSprite::PrepareSprite() {

	// VertexBufferView(VBV)を設定
	pCommandContextDirect_->GetCommandList()->IASetVertexBuffers(0, 1, vertexBuffer_->PGetVertexBufferView());

	// IndexBufferView(IBV)を設定
	pCommandContextDirect_->GetCommandList()->IASetIndexBuffer(indexBuffer_->PGetBufferView());

	// トランスフォームの定数バッファの最初のアドレスを設定
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationBuffer_->GetGpuVirtualAddress());

	// マテリアルの定数バッファの最初のアドレスを設定
	pCommandContextDirect_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGpuVirtualAddress());

}

void DrawSprite::DrawSpriteRect(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Transform& rectTransform, const Vector2& rectSize) {

	// 三角形のTransform
	Matrix4x4 worldMatrix = MakeWorldMatrix(rectTransform);

	TransformationData TransformData{};

	TransformData.wvp = worldMatrix * orthographicMatrix_;
	TransformData.world = worldMatrix;

	transformationBuffer_->SetData(TransformData, constantBufferCount_);


	MaterialData MaterialData{};

	MaterialData.color = textureColor;
	MaterialData.inLightingEnable = false;

	Matrix4x4 uvTransformData = MakeScaleMatrix(uvTransform.scale);
	uvTransformData *= MakeZRotateMatrix(uvTransform.rotate.z);
	uvTransformData *= MakeTranslateMatrix(uvTransform.translate);
	MaterialData.uvTransformMatrix = uvTransformData;

	materialBuffer_->SetData(MaterialData, constantBufferCount_);


	/* 1枚目の三角形 */

	uint32_t vertexCount = vertexBuffer_->GetDrewCount();

	Vector2 halfSize = rectSize * 0.5f;

	uint32_t indexCount = indexBuffer_->GetDrewCount();

	VertexData VertexData{};

	// 法線の向きは全点共通
	VertexData.normal = Vector3{ 0.0f, 0.0f, -1.0f };

	// 左下
	VertexData.texCoord = { 0.0f, 1.0f };
	VertexData.position = { -halfSize.x, halfSize.y, 0.0f, 1.0f };

	vertexBuffer_->SetVertexData(VertexData, vertexCount++);

	// 左上
	VertexData.texCoord = { 0.0f, 0.0f };
	VertexData.position = { -halfSize.x, -halfSize.y, 0.0f, 1.0f };

	vertexBuffer_->SetVertexData(VertexData, vertexCount++);

	// 右下
	VertexData.texCoord = { 1.0f, 1.0f };
	VertexData.position = { halfSize.x, halfSize.y, 0.0f, 1.0f };

	vertexBuffer_->SetVertexData(VertexData, vertexCount++);

	// 右上
	VertexData.texCoord = { 1.0f, 0.0f };
	VertexData.position = { halfSize.x, -halfSize.y, 0.0f, 1.0f };

	vertexBuffer_->SetVertexData(VertexData, vertexCount++);

	// インスタンス内で頂点インデックスを0から数える
	indexBuffer_->SetIndexData(0, indexCount++);
	indexBuffer_->SetIndexData(1, indexCount++);
	indexBuffer_->SetIndexData(2, indexCount++);
	indexBuffer_->SetIndexData(1, indexCount++);
	indexBuffer_->SetIndexData(3, indexCount++);
	indexBuffer_->SetIndexData(2, indexCount++);

	// 描画
	this->DrawSpriteCall(textureIndex);

}

void DrawSprite::DrawSpriteLine(const uint32_t& textureIndex, const Vector4& textureColor, const Transform& uvTransform, const Vector2& start, const Vector2& end, const float& width, const float& posZ) {

	Vector2 difference = end - start;
	float length = VectorLength(difference);

	Vector2 rectPos = start + difference * 0.5f;

	Transform rectTransform{};
	rectTransform.translate = { rectPos.x, rectPos.y, posZ };
	rectTransform.scale = { 1.0f, 1.0f, 1.0f };
	rectTransform.rotate = { 0.0f, 0.0f, std::atan2(difference.y, difference.x) };

	DrawSpriteRect(textureIndex, textureColor, uvTransform, rectTransform, Vector2{ length, width });

}