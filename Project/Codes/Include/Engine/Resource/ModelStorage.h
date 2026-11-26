#pragma once

#include "Engine/Resource/AssetModel.h"

#include <unordered_map>
#include <memory>
#include <wrl/client.h>

namespace Atrum {

	class TextureStorage;
	class CommandContext;

	class ModelStorage final {

	private:

		// 3DモデルAsset用 Meshマップ
		std::unordered_map<uint64_t, std::weak_ptr<AssetMeshData>> assetMeshMap_{};

		// 3DモデルAsset用 Materialマップ
		std::unordered_map<uint64_t, std::weak_ptr<AssetMaterialData>> assetMaterialMap_{};

		// 3DモデルAsset用 Modelマップ
		std::unordered_map<uint64_t, std::weak_ptr<AssetModel>> assetModelMap_{};

		uint32_t defaultTextureSrvIndex_ = 1;

		// RetainModel で保持するモデル（解放はこのストレージの破棄時）
		std::vector<std::shared_ptr<AssetModel>> retainedModels_{};

		ModelStorage() = default;

	public:

		~ModelStorage() = default;

	private:

		CommandContext* pCommandContextDirect_ = nullptr;
		ID3D12Device* pDevice_ = nullptr;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>* pTemporaryResources_ = nullptr;
		TextureStorage* textureStorage_ = nullptr;

		std::vector<AssetMeshNode> LoadObjFile(const std::string& directoryPath, const std::string& fileName, std::vector<std::string>& useMaterialNames);

		std::vector<std::shared_ptr<AssetMaterialData>> LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);

	public:

		struct Token {
			explicit Token() = default;
		};

		explicit ModelStorage(Token) {}

		void Initialize(CommandContext* commandContextDirect, ID3D12Device* device, std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>* pTemporaryResources, uint32_t defaultTextureIndex, TextureStorage* textureStorage);

		std::shared_ptr<AssetModel> CreateModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName);

		std::shared_ptr<AssetModel> GetModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName);

		/// ゲーム中に生成・破棄を繰り返すエンティティ用。GPU が参照中の頂点バッファが
		/// 最後のエンティティ破棄で解放されないよう、ストレージが終了まで保持する。
		void RetainModel(const std::shared_ptr<AssetModel>& model);

		ModelStorage operator=(const ModelStorage& source) = delete;
		ModelStorage(const ModelStorage& source) = delete;

		void ReleaseLoadedModelResources();

	};

}