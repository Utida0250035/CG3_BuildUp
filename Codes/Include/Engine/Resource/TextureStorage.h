#pragma once

#include "Texture.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <DirectXTex/DirectXTex.h>

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
	~TextureStorage() { textures_.clear(); };

	DirectX::ScratchImage LoadTexture(const std::string& filePath);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metaData);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureIntermediateResource(ID3D12Resource* textureResource);

	void UploadTextureData(ID3D12Resource* textureResource, const DirectX::ScratchImage& mipImages, ID3D12Resource* intermediateResource);

	void MakeShaderResourceView(Texture& texture, const DirectX::TexMetadata& metaData);

	static TextureStorage* instance_;

	CommandContext* pCommandContextDirect_ = nullptr;
	Fence* pFence_ = nullptr;
	SwapChain* pSwapChain_ = nullptr;
	DescriptorAllocator* pSrvAllocator_ = nullptr;
	ID3D12Device* pDevice_ = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>* pTemporaryResources_ = nullptr;

public:

	void Initialize(CommandContext* commandContextDirect, Fence* fence, SwapChain* swapChain, DescriptorAllocator* srvAllocator, ID3D12Device* device, std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>* pTemporaryResources);

	uint32_t GetTexture(const std::string& filePath);

	TextureStorage operator=(const TextureStorage& source) = delete;
	TextureStorage(const TextureStorage& source) = delete;

	static TextureStorage* GetInstance() {

		if (instance_ == nullptr) {

			instance_ = new TextureStorage();

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