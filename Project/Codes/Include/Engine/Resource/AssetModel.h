#pragma once
#include "DescriptorAllocator.h"
#include "MaterialData.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"
#include "TransformationData.h"
#include "ConstantBuffer.h"
#include "VertexData.h"

#include <memory>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#ifdef DEVELOPMENT
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

		void UnMap() {

			if (vertexResource) {

			  // vertexResource->Unmap(0, nullptr);

				vertices.clear();
			}
		}

#ifdef DEVELOPMENT

  // データ名
		std::string name;

#endif
	};

	/* Asset用 Material */
	struct AssetMaterialData {

	  // srvディスクリプタヒープ上の番号
		uint32_t textureSrvIndex = 0u;

		Math::Vector4 baseColor = Math::Vector4::White();

		bool isUseTexture = true;

		bool isLightingEnable = false;

#ifdef DEVELOPMENT

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

		// メッシュの塊の添え字検索
		std::unordered_map<uint64_t, size_t> nodeHashToIndexTable_{};
		// メッシュと対応マテリアルの塊
		std::vector<AssetMeshNode> meshNodes_{};

#ifdef DEVELOPMENT

		std::string objFilePathDebug_ = "";
		std::string mtlFilePathDebug_ = "";

#endif

	public:
		void Draw(const Math::Matrix4x4 worldMatrix, Math::Matrix4x4 viewMatrix,
			const D3D12_GPU_VIRTUAL_ADDRESS& directionalLightAddr,
			ID3D12GraphicsCommandList* commandList,
			DescriptorAllocator* srvAllocator,
			const Math::Matrix4x4& perspectiveFovMatrix, const bool isLighting,
			const Math::Vector4& color,
			uint32_t& ioConstantBufferCount,
			MultiConstantBuffer<MaterialData>* materialBuffer,
			MultiConstantBuffer<TransformationData>* transformationBuffer
			) {

			TransformationData transformationData{};

			transformationData.wvp =
				worldMatrix * viewMatrix * perspectiveFovMatrix;
			transformationData.world = worldMatrix;

			MaterialData materialData{};

			commandList->SetGraphicsRootConstantBufferView(3, directionalLightAddr);

			DescriptorAllocator::DescriptorHandle textureHandle{};

			D3D12_GPU_VIRTUAL_ADDRESS transformOffsetAddr{};
			
			D3D12_GPU_VIRTUAL_ADDRESS materialOffsetAddr{};

			for (auto& meshNode : meshNodes_) {

				transformationBuffer->SetData(transformationData, ioConstantBufferCount);

				// TransformMatrix (WVP) のアドレス計算
				transformOffsetAddr =
					transformationBuffer->GetGpuVirtualAddress() +
					(ioConstantBufferCount * sizeof(TransformationData));
				// GPUに設定(rootParameter0)
				commandList->SetGraphicsRootConstantBufferView(
					1, transformOffsetAddr);

				materialData.color = color* meshNode.material->baseColor;

				materialData.uvTransformMatrix = Math::Matrix4x4::Identity();

				materialData.isLightingEnable = isLighting;

				materialBuffer->SetData(materialData, ioConstantBufferCount);

				materialOffsetAddr =
					materialBuffer->GetGpuVirtualAddress() +
					(ioConstantBufferCount * sizeof(MaterialData));
				// GPUに設定(rootParameter1)
				commandList->SetGraphicsRootConstantBufferView(
					0, materialOffsetAddr);

				textureHandle =
					srvAllocator->GetHandle(meshNode.material->textureSrvIndex);

				// SRVのDescriptorTableの先頭を設定 rootParameter[2]
				commandList->SetGraphicsRootDescriptorTable(2, textureHandle.gpu);

				commandList->IASetPrimitiveTopology(
				    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandList->IASetVertexBuffers(0u, 1u, &meshNode.mesh->vertexBufferView);

				commandList->DrawInstanced(
					static_cast<UINT>(meshNode.mesh->vertices.size()), 1, 0, 0);

				++ioConstantBufferCount;
			}
		}

#ifdef DEVELOPMENT

		std::string GetTexturePath() {

			std::string result{};

			for (const auto& meshNode : meshNodes_) {

				result += "\n" + meshNode.material->textureFilePathDebug;
			}

			return result;
		}

#endif

		void ReleaseResources() {

			meshNodes_.clear();
		}
	};

} // namespace Atrum