#include "Debug/Log.h"
#include "Engine/Command/CommandContext.h"
#include "Engine/Command/Fence.h"
#include "Engine/Device/SwapChain.h"
#include "Engine/Resource/CreateBufferResource.h"
#include "Engine/Resource/DescriptorAllocator.h"
#include "Engine/Resource/TextureStorage.h"
#include "Hash/Hash64.h"
#include "String/ConvertString.h"

#include <filesystem>

TextureStorage* TextureStorage::instance_ = nullptr;

void TextureStorage::Initialize(CommandContext* commandContextDirect, Fence* fence, SwapChain* swapChain, DescriptorAllocator* srvAllocator, ID3D12Device* device, std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>* pTemporaryResources) {

	pCommandContextDirect_ = commandContextDirect;
	pFence_ = fence;
	pSwapChain_ = swapChain;
	pSrvAllocator_ = srvAllocator;
	pDevice_ = device;
	pTemporaryResources_ = pTemporaryResources;

}

DirectX::ScratchImage TextureStorage::LoadTexture(const std::string& filePath) {

	std::wstring filePathBuffer = StringToWString(filePath);

	if (!std::filesystem::exists(filePath)) {
		// ファイルが見つからなければ失敗
		std::wstring errorMessage = L"NotFoundFile:\n" + filePathBuffer;
		LogFile::GetInstance()->Log(WStringToString(errorMessage));
		MessageBoxW(nullptr, errorMessage.c_str(), L"Not Found Texture", MB_OK | MB_ICONERROR);

		assert(false && "textureFile not found");

	}

	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	[[maybe_unused]] HRESULT hr = DirectX::LoadFromWICFile(filePathBuffer.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミップマップ付きのデータを返す
	return mipImages;

}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureStorage::CreateTextureResource(const DirectX::TexMetadata& metaData) {

	/* metaDataを基にResourceの設定 */
	D3D12_RESOURCE_DESC resourceDesc{};
	// Textureの幅
	resourceDesc.Width = UINT(metaData.width);
	// Textureの高さ
	resourceDesc.Height = UINT(metaData.height);
	// mipMapの数
	resourceDesc.MipLevels = UINT16(metaData.mipLevels);
	// 奥行き or 配列Textureの要素数
	resourceDesc.DepthOrArraySize = UINT16(metaData.arraySize);
	// TextureのFormat
	resourceDesc.Format = metaData.format;
	// サンプリングカウント 1固定
	resourceDesc.SampleDesc.Count = 1;
	// Textureの次元数 普段(画像)は2次元
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);

	/* 利用するHeapの設定 */
	D3D12_HEAP_PROPERTIES heapProperties{};
	// VRAM上に作成
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	/* Resourceの生成 */
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	[[maybe_unused]] HRESULT hr = pDevice_->CreateCommittedResource(
		// Heapの設定
		&heapProperties,
		// Heapの特殊な設定 特に無し
		D3D12_HEAP_FLAG_NONE,
		// Resourceの設定
		&resourceDesc,
		// 初回のResourceState データ転送される設定
		D3D12_RESOURCE_STATE_COPY_DEST,
		// Clear最適値 使わないのでnullptr
		nullptr,
		// 作成するResourceポインタへのポインタ
		IID_PPV_ARGS(&resource)
	);

	assert(SUCCEEDED(hr));

	return resource;

}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureStorage::CreateTextureIntermediateResource(ID3D12Resource* textureResource) {

	// テクスチャの設定を取得
	D3D12_RESOURCE_DESC texDesc = textureResource->GetDesc();
	uint64_t intermediateSize = 0;

	/*GPU上のレイアウトに基づき、必要な総書き込みサイズを計算*/
	// GetCopyableFootPrints()は、テクスチャの各サブリソースが
	// UPLOADバッファ上でどこに配置されるべきか(オフセットやピッチ)を計算する
	pDevice_->GetCopyableFootprints(
		// テクスチャの情報をもとにする
		&texDesc,
		// 最初のサブリソースインデックス
		0,
		// 全ミップレベル分を計算
		texDesc.MipLevels,
		// バッファ上の開始オフセット
		0,
		// レイアウト詳細 不要
		nullptr,
		// 行数 不要
		nullptr,
		// 行サイズ 不要
		nullptr,
		// 合計の必要バイト数の受け取り
		&intermediateSize
	);

	// 受け取ったサイズのUPLOADヒープのリソースを生成して参照元へ
	return CreateIntermediateResource(intermediateSize, pDevice_);

}

void TextureStorage::UploadTextureData(ID3D12Resource* textureResource, const DirectX::ScratchImage& mipImages, ID3D12Resource* intermediateResource) {

	UINT subresourceCount = static_cast<UINT>(mipImages.GetImageCount());

	/* レイアウト情報の取得 */
	// GPU上のメモリ配置(アライメント)に合わせたコピー情報を取得する
	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts(subresourceCount);
	std::vector<UINT> numRows(subresourceCount);
	std::vector<UINT64> rowSizeInBytes(subresourceCount);
	UINT64 totalBytes = 0;

	D3D12_RESOURCE_DESC desc = textureResource->GetDesc();
	pDevice_->GetCopyableFootprints(
		&desc,
		0,
		subresourceCount,
		0,
		layouts.data(),
		numRows.data(),
		rowSizeInBytes.data(),
		&totalBytes
	);

	/* CPUから中間リソース(UPLOAD)への書き込み(Map > memcpy > UnMap) */
	uint8_t* pDest = nullptr;
	intermediateResource->Map(0, nullptr, reinterpret_cast<void**>(&pDest));

	for (UINT i = 0; i < subresourceCount; ++i) {

		// 各ミップレベルで配列、深さを取得
		const DirectX::Image* image = mipImages.GetImage(i, 0, 0);

		uint8_t* pDestSubresource = pDest + layouts[i].Offset;
		const uint8_t* pSrcSubresource = image->pixels;

		for (UINT y = 0; y < numRows[i]; ++y) {
			// 行ごとにコピー(GPUのアライメント/ピッチに合わせるため)

			std::memcpy(
				// 書き込み先(アライメント考慮済み)
				pDestSubresource + (y * layouts[i].Footprint.RowPitch),
				// 読み込み元
				pSrcSubresource + (y * image->rowPitch),
				// 1行の有効なバイト数
				rowSizeInBytes[i]
			);

		}

	}

	intermediateResource->Unmap(0, nullptr);

	/* 中間リソースからテクスチャへのコピー命令を積む */
	for (UINT i = 0; i < subresourceCount; ++i) {

		D3D12_TEXTURE_COPY_LOCATION destinationLocation{};
		destinationLocation.pResource = textureResource;
		destinationLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destinationLocation.SubresourceIndex = i;

		D3D12_TEXTURE_COPY_LOCATION sourceLocation{};
		sourceLocation.pResource = intermediateResource;
		sourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		sourceLocation.PlacedFootprint = layouts[i];

		pCommandContextDirect_->GetCommandList()->CopyTextureRegion(&destinationLocation, 0, 0, 0, &sourceLocation, nullptr);

	}

	/* バリアを張って利用可能な状態にする */
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textureResource;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	pCommandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

}

void TextureStorage::MakeShaderResourceView(Texture& texture, const DirectX::TexMetadata& metaData) {

	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	// 2Dテクスチャ
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metaData.mipLevels);

	// SRVを作成するDescriptionHeapの場所を決める

	DescriptorAllocator::DescriptorHandle handle{};

	handle = pSrvAllocator_->Allocate();

	texture.srvHandleCPU = handle.cpu;
	texture.srvHandleGPU = handle.gpu;
	texture.srvIndex = handle.index;

	// SRVの作成
	pDevice_->CreateShaderResourceView(texture.resource.Get(), &srvDesc, texture.srvHandleCPU);

}

uint32_t TextureStorage::GetTexture(const std::string& filePath) {

	auto search = textureIndexTable_.find(hash64_str(filePath.c_str()));

	if (search != textureIndexTable_.end()) {

		return search->second;

	}

	Texture texture;

	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = this->LoadTexture(filePath);
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();

	texture.resource = this->CreateTextureResource(metaData);

	assert(texture.resource);

	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = this->CreateTextureIntermediateResource(texture.resource.Get());

	// 中間リソースを用いた転送
	this->UploadTextureData(texture.resource.Get(), mipImages, intermediateResource.Get());

	// コマンドリストの内容を確定させる
	[[maybe_unused]] HRESULT hr = pCommandContextDirect_->GetCommandList()->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストを実行させる
	ID3D12CommandList* commandLists[] = { pCommandContextDirect_->GetCommandList()};
	pCommandContextDirect_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	// 次のフレームの準備
	// 現在のフレームが「いつ終わるか」を Signal
	// 「次に使う予定のアロケータ」が解放されているかを確認して Wait する

	// 現在のフレームに完了番号を割り振って Signal
	pFence_->Signal(pCommandContextDirect_->GetCommandQueue(), pSwapChain_->GetBackBufferIndex());

	// 次のフレームのインデックスを取得する
	pSwapChain_->UpdateBackBufferIndex();

	// これから使うアロケータが前回の実行を終えているか確認
	pFence_->WaitForNextBuffer(pSwapChain_->GetBackBufferIndex());

	// 次のフレーム用のアロケータとリストをリセット
	hr = pCommandContextDirect_->GetCommandAllocator(pSwapChain_->GetBackBufferIndex())->Reset();
	assert(SUCCEEDED(hr));
	hr = pCommandContextDirect_->GetCommandList()->Reset(pCommandContextDirect_->GetCommandAllocator(pSwapChain_->GetBackBufferIndex()), nullptr);
	assert(SUCCEEDED(hr));

	// 実際にShaderResourceViewを作る
	this->MakeShaderResourceView(texture, metaData);

	// 中間リソースを一時保存
	pTemporaryResources_->emplace_back(intermediateResource);

	// ファイル名からのハッシュとSRV番号を格納
	textureIndexTable_.emplace(hash64_str(filePath.c_str()), texture.srvIndex);

	// 配列に所有権を移動
	textures_.emplace_back(texture);

	LogFile::GetInstance()->Log("Got Texture: " + filePath);

	// 作成したテクスチャの番号を返す
	return textures_.back().srvIndex;

}