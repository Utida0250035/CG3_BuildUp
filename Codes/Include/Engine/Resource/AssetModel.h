#pragma once
#include "./DescriptorAllocator.h"
#include "./MaterialData.h"
#include "./TransformationData.h"
#include "./VertexData.h"
#include "Math/Transform.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#ifdef _DEBUG
#include <string>
#endif

namespace Atrum {

/* Asset用 Mesh */
	struct AssetMeshData {

		// 頂点データ
		std::vector<VertexData> vertices;

		// 頂点リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;

		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

#ifdef _DEBUG

	// データ名
		std::string name;

#endif

	};

	/* Asset用 Material */
	struct AssetMaterialData {

		// srvディスクリプタヒープ上の番号
		uint32_t textureSrvIndex = 0u;

		// マテリアルリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;

		// マテリアルデータ
		MaterialData* materialData = nullptr;

#ifdef _DEBUG

	// テクスチャのファイルパス
		std::string textureFilePathDebug = "";

		// データ名
		std::string name;

#endif

	};

	struct AssetMeshNode {

		std::shared_ptr<AssetMeshData> mesh;
		std::shared_ptr<AssetMaterialData> material;

	};

	class AtrumEngine;
	class ModelStorage;

	/* Asset用 Model */
	class AssetModel {

	private:

		friend AtrumEngine;

		friend ModelStorage;

		// 座標変換リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> transformationResource_ = nullptr;
		// 座標変換データ
		TransformationData* transformationData_ = nullptr;

		// メッシュの塊の添え字検索
		std::unordered_map<uint64_t, size_t> nodeHashToIndexTable_{};
		// メッシュと対応マテリアルの塊
		std::vector<AssetMeshNode> meshNodes_{};

#ifdef _DEBUG

		std::string objFilePathDebug_ = "";
		std::string mtlFilePathDebug_ = "";

#endif

	public:

		void Draw(const Math::Transform& transform, Math::Matrix4x4 viewMatrix, const D3D12_GPU_VIRTUAL_ADDRESS& directionalLightAddr, ID3D12GraphicsCommandList* commandList, DescriptorAllocator* srvAllocator, const Math::Matrix4x4& perspectiveFovMatrix, const bool isLighting) {

			// 三角形のTransform
			Math::Matrix4x4 worldMatrix = transform.MakeWorldMatrix();

			transformationData_->world = worldMatrix;

			transformationData_->wvp = worldMatrix * viewMatrix * perspectiveFovMatrix;

			commandList->SetGraphicsRootConstantBufferView(1, transformationResource_->GetGPUVirtualAddress());

			commandList->SetGraphicsRootConstantBufferView(3, directionalLightAddr);


			DescriptorAllocator::DescriptorHandle textureHandle{};

			for (auto& meshNode : meshNodes_) {

				meshNode.material->materialData->inLightingEnable = isLighting;

				textureHandle = srvAllocator->GetHandle(meshNode.material->textureSrvIndex);

				// SRVのDescriptorTableの先頭を設定 rootParameter[2]
				commandList->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

				commandList->SetGraphicsRootConstantBufferView(0, meshNode.material->materialResource->GetGPUVirtualAddress());

				commandList->IASetVertexBuffers(0u, 1u, &meshNode.mesh->vertexBufferView);

				commandList->DrawInstanced(static_cast<UINT>(meshNode.mesh->vertices.size()), 1, 0, 0);

			}

		}

#ifdef _DEBUG

		std::string GetTexturePath() {

			std::string result{};

			for (const auto& meshNode : meshNodes_) {

				result += "\n" + meshNode.material->textureFilePathDebug;

			}

			return result;

		}

#endif

	};

}