#include "ForDebug/Log.h"
#include "Engine/Pipeline/RootSignature.h"
#include <cassert>

namespace Atrum {

	void RootSignature::Initialize(ID3D12Device* device) {

		// RootSignature作成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// RootParameter:
		// [0] PS Material (b0), [1] VS Transform (b0),
		// [2] PS テクスチャ SRV テーブル (t0),
		// [3] PS 平行光源 (b1), [4] VS パーティクルフレーム (b2),
		// [5] VS パーティクルインスタンス SRV テーブル (t1)
		D3D12_ROOT_PARAMETER rootParameters[6] = {};

		// CBVを使う
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		// PixelShaderで使う
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		// レジスタ番号0にバインド
		rootParameters[0].Descriptor.ShaderRegister = 0;

		// CBVを使う
		rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		// VertexShaderで使う
		rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		// レジスタ番号0を使う
		rootParameters[1].Descriptor.ShaderRegister = 0;

		// CBVを使う
		rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		// PixelShaderで使う
		rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		// レジスタ番号1を使う
		rootParameters[3].Descriptor.ShaderRegister = 1;

		// パーティクル用: フレーム定数（VP・View・カメラ位置）
		rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters[4].Descriptor.ShaderRegister = 2;


		// ルートパラメータ配列へのポインタ
		descriptionRootSignature.pParameters = rootParameters;
		// 配列の長さ
		descriptionRootSignature.NumParameters = _countof(rootParameters);


		// テクスチャ用 DescriptorRange（PS）
		D3D12_DESCRIPTOR_RANGE textureSrvRange[1] = {};
		textureSrvRange[0].BaseShaderRegister = 0;
		textureSrvRange[0].NumDescriptors = 1;
		textureSrvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		textureSrvRange[0].OffsetInDescriptorsFromTableStart =
		    D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		// DescriptorTableを使う
		rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		// PixelShaderで使う
		rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters[2].DescriptorTable.pDescriptorRanges = textureSrvRange;
		rootParameters[2].DescriptorTable.NumDescriptorRanges =
		    _countof(textureSrvRange);

		// パーティクル StructuredBuffer 用（VS）
		D3D12_DESCRIPTOR_RANGE particleInstanceSrvRange[1] = {};
		particleInstanceSrvRange[0].BaseShaderRegister = 1;
		particleInstanceSrvRange[0].NumDescriptors = 1;
		particleInstanceSrvRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		particleInstanceSrvRange[0].OffsetInDescriptorsFromTableStart =
		    D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters[5].DescriptorTable.pDescriptorRanges =
		    particleInstanceSrvRange;
		rootParameters[5].DescriptorTable.NumDescriptorRanges =
		    _countof(particleInstanceSrvRange);


		D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		// バイリニアフィルタ
		staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		// 0~1の範囲外をリピート
		staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		// 比較しない
		staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		// ありったけのMipMapを使う
		staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		// レジスタ番号0を使う
		staticSamplers[0].ShaderRegister = 0;
		// PixelShaderで使う
		staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		descriptionRootSignature.pStaticSamplers = staticSamplers;
		descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

		// シリアライズしてバイナリにする
		HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);

		if (FAILED(hr)) {

			Debug::LogFile::Instance()->Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));

			assert(false);

		}

		// バイナリを基に生成
		hr = device->CreateRootSignature(0, signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
		assert(SUCCEEDED(hr));

		Debug::LogFile::Instance()->Log("Created RootSignature");

	}

}