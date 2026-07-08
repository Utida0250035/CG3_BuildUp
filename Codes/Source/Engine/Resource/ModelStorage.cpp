#include "Debug/Log.h"
#include "Engine/CreateBufferResource.h"
#include "Engine/ModelStorage.h"
#include "Engine/TextureStorage.h"
#include "Hash/Hash64.h"
#include "Engine/CommandContext.h"

#include <DirectXTex/d3dx12.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <wrl/client.h>
#pragma comment(lib, "DirectXTex.lib")

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace fs = std::filesystem;

ModelStorage* ModelStorage::instance_ = nullptr;

void ModelStorage::Initialize(CommandContext* commandContextDirect, ID3D12Device* device, std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>* pTemporaryResources) {

	pCommandContextDirect_ = commandContextDirect;
	pDevice_ = device;
	pTemporaryResources_ = pTemporaryResources;

}


std::vector<AssetMeshNode> ModelStorage::LoadObjFile(const std::string& directoryPath, const std::string& fileName, std::vector<std::string>& useMaterialNames) {

	// 戻り値用 mtl部分は空
	std::vector<AssetMeshNode> assetMeshNodes{};

	// メッシュ生成用
	std::shared_ptr<AssetMeshData> assetMeshData = std::make_shared<AssetMeshData>();

	// 位置
	std::vector<Vector4> positions{};
	// 法線
	std::vector<Vector3> normals{};
	// テクスチャ座標
	std::vector<Vector2> texCoords{};

	// ファイル1行分
	std::string line;

	// ファイルのパス
	std::string filePath = directoryPath + "/" + fileName;

	// ファイルからの入力
	std::ifstream file(filePath);
	// 開けなかったらエラー
	assert(file.is_open());

	bool isMeshExist = false;
	bool isSkippedUsemtl = false;

	std::string meshName = "";

	while (std::getline(file, line)) {

		std::string identifier;
		std::stringstream s(line);

		// 先頭の識別子を読む
		s >> identifier;

		switch (hash64_str(identifier.c_str())) {
			case "o"_hash64:
			{

				if (isSkippedUsemtl) {
					// usemtlを飛ばしたあとなら

					// 飛ばしフラグを折る
					isSkippedUsemtl = false;

					// メッシュの既存フラグを折る
					isMeshExist = false;

				}

				if (isMeshExist) {

					break;

				}

				meshName.clear();

				s >> meshName;

				meshName = filePath + "?" + meshName;

				auto search = assetMeshMap_.find(hash64_str(meshName));

				if (search != assetMeshMap_.end()) {
					// メッシュが作成済テーブルに存在する場合

					// メッシュ作成完了 配列に保存
					assetMeshNodes.emplace_back(search->second.lock(), nullptr);

					useMaterialNames.emplace_back("");

					// 既存フラグを立てて次のusemtlまで処理を飛ばす
					isMeshExist = true;

					LogFile::GetInstance()->Log("GetMeshFromTable: " + meshName);

					break;

				}

				LogFile::GetInstance()->Log("LoadMesh: " + meshName);

				break;

			}

			case "v"_hash64:
			{

				if (isMeshExist) {

					break;

				}

				Vector4 position{};

				s >> position.x >> position.y >> position.z;
				position.w = 1.0f;

				positions.push_back(position);

				break;

			}

			case "vt"_hash64:
			{

				if (isMeshExist) {

					break;

				}

				Vector2 texCoord{};
				s >> texCoord.x >> texCoord.y;

				texCoords.push_back(texCoord);

				break;

			}

			case "vn"_hash64:
			{

				if (isMeshExist) {

					break;

				}

				Vector3 normal{ 0.0f, 1.0f, 0.0f };
				s >> normal.x >> normal.y >> normal.z;

				normals.push_back(normal);

				break;

			}

			case "f"_hash64:
			{

				if (isMeshExist) {

					break;

				}

				VertexData triangle[3]{};

				// 三角形の集合に限定 その他は対応しない

				for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {

					std::string vertexDefinition;
					s >> vertexDefinition;

					// 頂点の要素へのIndexは「位置/UV/法線」で格納されている
					// 分解してIndexを取得
					std::istringstream v(vertexDefinition);
					size_t elementIndices[3]{};

					for (size_t element = 0; element < 3; ++element) {

						std::string index;
						// /(スラッシュ)区切りでIndexを読んでいく
						std::getline(v, index, '/');
						elementIndices[element] = std::stoi(index);

					}

					Vector4 position = positions[elementIndices[0] - 1];
					position.z *= -1.0f;

					Vector2 texCoord = texCoords[elementIndices[1] - 1];
					texCoord.y = 1.0f - texCoord.y;

					Vector3 normal = normals[elementIndices[2] - 1];
					normal.z *= -1.0f;

					triangle[faceVertex] = { position, texCoord, normal };

				}

				assetMeshNodes.back().mesh->vertices.push_back(triangle[2]);
				assetMeshNodes.back().mesh->vertices.push_back(triangle[1]);
				assetMeshNodes.back().mesh->vertices.push_back(triangle[0]);

				break;

			}

			case "usemtl"_hash64:
			{

				if (isMeshExist) {

					isSkippedUsemtl = true;

					break;

				}

				std::string mtlName{};

				s >> mtlName;

				useMaterialNames.emplace_back(std::move(mtlName));

				// メッシュのポインタを配列に保存
				assetMeshNodes.emplace_back(assetMeshData, nullptr);

				// メッシュのポインタを作成済みテーブルに保存
				assetMeshMap_.emplace(hash64_str(meshName), assetMeshData);

				// 解放・再生成して次に備える
				assetMeshData.reset(new AssetMeshData());

				break;

			}

		}


	}

	file.close();

	size_t bufferSize = 0;

	HRESULT hr;

	for (auto& meshNode : assetMeshNodes) {

		auto& mesh = meshNode.mesh;

		if (mesh->vertexResource) {

			continue;

		}

		bufferSize = sizeof(VertexData) * mesh->vertices.size();

		mesh->vertexResource = CreateDefaultBuffer(bufferSize, pDevice_);
		ComPtr<ID3D12Resource> intermediateResource = CreateIntermediateResource(bufferSize, pDevice_);

		void* pData = nullptr;

		hr = intermediateResource->Map(0u, nullptr, &pData);

		if (SUCCEEDED(hr)) {

			std::memcpy(pData, mesh->vertices.data(), bufferSize);
			intermediateResource->Unmap(0u, nullptr);

		} else {

			assert(false && "LoadObjFile() failed");

		}

		pCommandContextDirect_->GetCommandList()->CopyBufferRegion(
			mesh->vertexResource.Get(), 0u,
			intermediateResource.Get(), 0u,
			static_cast<UINT64>(bufferSize)
		);

		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			mesh->vertexResource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		);

		pCommandContextDirect_->GetCommandList()->ResourceBarrier(1, &barrier);

		mesh->vertexBufferView.BufferLocation = mesh->vertexResource->GetGPUVirtualAddress();
		mesh->vertexBufferView.SizeInBytes = static_cast<UINT>(bufferSize);
		mesh->vertexBufferView.StrideInBytes = sizeof(VertexData);

		assetMeshMap_.emplace(hash64_str(filePath.c_str()), mesh);

		pTemporaryResources_->emplace_back(intermediateResource);

	}

	return assetMeshNodes;

}

std::vector<std::shared_ptr<AssetMaterialData>> ModelStorage::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName) {

	// 戻り値用
	std::vector<std::shared_ptr<AssetMaterialData>> assetMaterialData{};

	// マテリアルデータ生成用
	std::shared_ptr<AssetMaterialData> assetMaterial = nullptr;

	// 色読み込み用
	std::vector<Vector4> colors{};

	// ライティングフラグ読み込み用
	std::vector<bool> lightingEnableData{};

	// ファイルの1行読み込み
	std::string line;

	// ファイルのパス
	std::string filePath = directoryPath + "/" + fileName;

	// ファイルからの入力
	std::fstream file(filePath);
	// ファイルが開けなければエラー
	assert(file.is_open());

	std::string textureFilePath;

	bool isMaterialExist = false;

	bool isMaterialUseTexture = false;

	std::string mtlName = "";

	while (std::getline(file, line)) {

		std::string identifier;
		std::istringstream s(line);

		s >> identifier;

		switch (hash64_str(identifier)) {

			case "map_Kd"_hash64:
			{

				if (isMaterialExist) {

					break;

				}

				std::string textureFileName;
				s >> textureFileName;

				// 連結してファイルパスにする
				textureFilePath = directoryPath + "/" + textureFileName;

				// ファイルパスを基にテクスチャ取得
				assetMaterial->textureSrvIndex = TextureStorage::GetInstance()->GetTexture(textureFilePath);

				assetMaterialData.emplace_back(assetMaterial);

				assetMaterialMap_.emplace(hash64_str(mtlName), assetMaterial);

				isMaterialUseTexture = true;

#ifdef _DEBUG

				assetMaterial->textureFilePathDebug = textureFilePath;

#endif

				break;

			}

			case "Kd"_hash64:
			{

				if (isMaterialExist) {

					break;

				}

				Vector4 color{};

				s >> color.x >> color.y >> color.z;
				color.w = 1.0f;

				colors.emplace_back(std::move(color));

				break;

			}

			case "illum"_hash64:
			{

				if (isMaterialExist) {

					break;

				}

				UINT illum = 0u;

				s >> illum;

				if (illum > 0u) {

					lightingEnableData.emplace_back(true);

				} else {

					lightingEnableData.emplace_back(false);

				}

				break;

			}

			case "newmtl"_hash64:
			{

				if (assetMaterial) {

					if (isMaterialUseTexture) {

						isMaterialUseTexture = false;

					} else {
						// マテリアルの作成が完了していない(テクスチャが貼られていない)場合

						colors.push_back(Vec4White());


#ifdef _DEBUG
						assetMaterial->textureFilePathDebug = "./Assets/Images/white4x4.png";
#endif

						assetMaterialData.emplace_back(assetMaterial);

						assetMaterialMap_.emplace(hash64_str(mtlName), assetMaterial);

					}

					isMaterialExist = false;

				}

				assetMaterial.reset(new AssetMaterialData());

				mtlName.clear();

				s >> mtlName;

				mtlName = filePath + "?" + mtlName;

				auto search = assetMaterialMap_.find(hash64_str(mtlName));

				if (search != assetMaterialMap_.end()) {

					assert(search->second.lock());

					assetMaterialData.emplace_back(search->second.lock());

					// 色配列に空データを追加
					colors.emplace_back();

					// ライティングフラグ配列に空データを追加
					lightingEnableData.emplace_back(false);

					isMaterialExist = true;

					LogFile::GetInstance()->Log("GetMtlFromTable: " + mtlName);

				}

				LogFile::GetInstance()->Log("LoadMtl: " + mtlName);

				break;

			}

		}

	}

	assert(assetMaterialData.size() == colors.size());
	assert(assetMaterialData.size() == lightingEnableData.size());

	for (size_t i = 0; i < assetMaterialData.size(); ++i) {

		if (assetMaterialData[i]->materialResource) {

			continue;

		}

		// マテリアルリソースの生成
		assetMaterialData[i]->materialResource = CreateUploadBuffer(sizeof(MaterialData), pDevice_);
		// 書き込み用アドレスの確保
		assetMaterialData[i]->materialResource->Map(0u, nullptr, reinterpret_cast<void**>(&assetMaterialData[i]->materialData));

		assetMaterialData[i]->materialData->uvTransformMatrix = MakeIdentity4x4();

		assetMaterialData[i]->materialData->color = colors[i];

		assetMaterialData[i]->materialData->inLightingEnable = lightingEnableData[i];

	}

	return assetMaterialData;

}

std::shared_ptr<AssetModel> ModelStorage::CreateModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName) {

	assert(fs::exists(directoryPathObj + "/" + objFileName));

	assert(fs::exists(directoryPathMtl + "/" + mtlFileName));

	std::shared_ptr<AssetModel> assetModelData = std::make_shared<AssetModel>();

	std::vector<std::string> useMaterialNames{};

	// メッシュデータ
	assetModelData->meshNodes_ = LoadObjFile(directoryPathObj, objFileName, useMaterialNames);

	// マテリアルデータの寿命保証
	std::vector<std::shared_ptr<AssetMaterialData>> assetMaterialData = LoadMaterialTemplateFile(directoryPathMtl, mtlFileName);

	for (size_t i = 0; i < assetModelData->meshNodes_.size(); ++i) {

		auto search = assetMaterialMap_.find(hash64_str(directoryPathMtl + "/" + mtlFileName + "?" + useMaterialNames[i]));

		assert(search != assetMaterialMap_.end());

		assetModelData->meshNodes_[i].material = search->second.lock();

	}

	assetMaterialData.clear();

	// 座標変換リソース・データ
	assetModelData->transformationResource_ = CreateUploadBuffer(sizeof(TransformationData), pDevice_);
	assetModelData->transformationResource_->Map(0u, nullptr, reinterpret_cast<void**>(&assetModelData->transformationData_));

#ifdef _DEBUG

	assetModelData->mtlFilePathDebug_ = mtlFileName;
	assetModelData->objFilePathDebug_ = objFileName;

#endif

	// ファイル名(区切り連結)のハッシュ化
	uint64_t manageHash = hash64_str((objFileName + "|" + mtlFileName).c_str());

	// モデルテーブルへ追加
	assetModelMap_.emplace(manageHash, assetModelData);

	// モデルデータを参照元へ戻す
	return assetModelData;

}

std::shared_ptr<AssetModel> ModelStorage::GetModel(const std::string& directoryPathObj, const std::string& objFileName, const std::string& directoryPathMtl, const std::string& mtlFileName) {

	// キー検索
	auto search = assetModelMap_.find(hash64_str((directoryPathObj + "/" + objFileName + "|" + directoryPathMtl + "/" + mtlFileName).c_str()));

	if (search != assetModelMap_.end()) {
		// 該当要素がモデルテーブルに見つかった場合

		if (search->second.lock()) {
			// 値が空でなければ戻り値とする

			return search->second.lock();

		}

		// 値が空なら要素を消去する
		assetModelMap_.erase(search);

	}

	// 無ければ新しく作って戻り値とする
	return this->CreateModel(directoryPathObj, objFileName, directoryPathMtl, mtlFileName);

}