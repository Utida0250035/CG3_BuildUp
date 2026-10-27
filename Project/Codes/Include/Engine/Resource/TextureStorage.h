#pragma once

#include "Texture.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <DirectXTex/DirectXTex.h>

namespace Atrum {

	class Fence;
	class SwapChain;
	class DescriptorAllocator;
	class CommandContext;

	class TextureStorage final {

	private:

		// Textureのsrv番号テーブル
		std::unordered_map<uint64_t, uint32_t> textureIndexTable_{};

		// Texture
		std::vector<Texture> textures_{};

		TextureStorage() = default;

	public:

		~TextureStorage() { textures_.clear(); };

	private:

		DirectX::ScratchImage LoadTexture(const std::string& filePath);

		Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metaData);

		Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureIntermediateResource(ID3D12Resource* textureResource);

		void UploadTextureData(ID3D12Resource* textureResource, const DirectX::ScratchImage& mipImages, ID3D12Resource* intermediateResource);

		void MakeShaderResourceView(Texture& texture, const DirectX::TexMetadata& metaData);

		static std::unique_ptr<TextureStorage> instance_;

		CommandContext* pCommandContextDirect_ = nullptr;
		Fence* pFence_ = nullptr;
		SwapChain* pSwapChain_ = nullptr;
		DescriptorAllocator* pSrvAllocator_ = nullptr;
		ID3D12Device* pDevice_ = nullptr;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>* pTemporaryResources_ = nullptr;

		struct Token {
			explicit Token() = default;
		};

	public:

		explicit TextureStorage(Token) {}

		void Initialize(CommandContext* commandContextDirect, Fence* fence, SwapChain* swapChain, DescriptorAllocator* srvAllocator, ID3D12Device* device, std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>* pTemporaryResources);

		uint32_t GetTexture(const std::string& filePath);
		uint32_t GetTextureWidth(const uint32_t textureIndex);
		uint32_t GetTextureHeight(const uint32_t textureIndex);

		TextureStorage operator=(const TextureStorage& source) = delete;
		TextureStorage(const TextureStorage& source) = delete;

		static TextureStorage* Instance() {

			if (!instance_) {

				instance_ = std::make_unique<TextureStorage>(Token{});

			}

			return instance_.get();

		}

		static void Destroy() {

			if (instance_) {
				instance_.reset();
			}

		}

	};

}