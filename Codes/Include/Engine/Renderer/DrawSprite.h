#pragma once

#include "Cast/StaticCast.h"
#include "Engine/Resource/ConstantBuffer.h"
#include "Engine/Resource/MaterialData.h"
#include "Engine/Resource/TransformationData.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"

#include <memory>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

class IndexBuffer;
class VertexBuffer;
class CommandContext;
class DescriptorAllocator;

class AtrumEngine;

class DrawSprite final {

private:

	friend AtrumEngine;

	/* Sprite用 Vertex */

	std::unique_ptr<VertexBuffer> vertexBuffer_ = nullptr;


	/* Sprite用 頂点インデックス */

	std::unique_ptr<IndexBuffer> indexBuffer_ = nullptr;


	/* Sprite用 Material */

	// MaterialResource
	std::unique_ptr<MultiConstantBuffer<MaterialData>> materialBuffer_ = nullptr;


	/* Sprite用 Transform */

	std::unique_ptr<MultiConstantBuffer<TransformationData>> transformationBuffer_ = nullptr;

	/* Sprite用 constantBufferCount */

	uint32_t constantBufferCount_ = 0;

	/// <summary>
	/// 初期化処理 Sprite用VertexBufferの生成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// 初期化処理 Sprite用IndexBufferの生成
	/// </summary>
	void CreateIndexBuffer();

	/// <summary>
	/// 初期化処理 Sprite用MaterialBufferの生成
	/// </summary>
	void CreateMaterialBuffer();

	/// <summary>
	/// 初期化処理 Sprite用TransformBufferの生成
	/// </summary>
	void CreateTransformationBuffer();


	/// <summary>
	/// Spriteの描画呼び出し
	/// </summary>
	void DrawSpriteCall(const uint32_t& textureIndex);

	ID3D12Device* pDevice_ = nullptr;
	CommandContext* pCommandContextDirect_ = nullptr;
	DescriptorAllocator* pSrvAllocator_ = nullptr;

	inline static constexpr uint32_t kMaxDrawCount = 1024;

	// スプライト版 最大頂点数
	inline static constexpr uint32_t kVertexMaxDrawCount = 4096;

	DrawSprite() = default;
	~DrawSprite() = default;

	static DrawSprite* instance_;

	// 正射影
	Matrix4x4 orthographicMatrix_{};

public:

	void Initialize(ID3D12Device* device, CommandContext* commandContextDirect, DescriptorAllocator* srvAllocator, const uint32_t clientWidth, const uint32_t clientHeight);

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


	DrawSprite operator=(const DrawSprite& source) = delete;
	DrawSprite(const DrawSprite& source) = delete;

	static DrawSprite* GetInstance() {

		if (!instance_) {

			instance_ = new DrawSprite();

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