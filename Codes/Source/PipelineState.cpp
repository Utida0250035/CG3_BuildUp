#include "PipelineState.h"
#include "Log.h"
#include <cassert>

void PipelineState::SetupInputLayout() {

	inputElementDescriptions_[0].SemanticName = "POSITION";
	inputElementDescriptions_[0].SemanticIndex = 0;
	inputElementDescriptions_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescriptions_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescriptions_[1].SemanticName = "TEXCOORD";
	inputElementDescriptions_[1].SemanticIndex = 0;
	inputElementDescriptions_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescriptions_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescriptions_[2].SemanticName = "NORMAL";
	inputElementDescriptions_[2].SemanticIndex = 0;
	inputElementDescriptions_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescriptions_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescriptions_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescriptions_);

	LogFile::GetInstance()->Log("Finished SetUp InputLayout");

}

void PipelineState::SetupBlendState() {

	// BlendStateの設定

	// 全ての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	LogFile::GetInstance()->Log("Finished SetUp BlendState");

}

void PipelineState::SetupRasterizerState() {

	// RasterizerStateの設定

	// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;

	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	LogFile::GetInstance()->Log("Finished SetUp RasterizerState");

}

void PipelineState::SetupDepthStencilState() {

	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;

	// 書き込みする
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	// 比較関数をLessEqualとする(近ければ描画される)
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	LogFile::GetInstance()->Log("Finished SetUp DepthStencilState");

}

void PipelineState::Initialize(ID3D12RootSignature* rootSignature, ID3D12Device* device, IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob) {

	this->SetupInputLayout();
	this->SetupBlendState();
	this->SetupRasterizerState();
	this->SetupDepthStencilState();

	// ルートシグネチャを設定
	pipelineStateDesc_.pRootSignature = rootSignature;

	// InputLayout
	pipelineStateDesc_.InputLayout = inputLayoutDesc_;

	// VertexShader
	pipelineStateDesc_.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };

	// PixelShader
	pipelineStateDesc_.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };

	// Blendの設定
	pipelineStateDesc_.BlendState = blendDesc_;

	// Rasterizerの設定
	pipelineStateDesc_.RasterizerState = rasterizerDesc_;

	// DepthStencilの設定
	pipelineStateDesc_.DepthStencilState = depthStencilDesc_;
	pipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	pipelineStateDesc_.NumRenderTargets = 1;
	pipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するトポロジ(形状)のタイプ 三角形
	pipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// どのように画面に色を打ち込むかの設定
	pipelineStateDesc_.SampleDesc.Count = 1;
	pipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 実際に生成
	HRESULT hr = device->CreateGraphicsPipelineState(&pipelineStateDesc_, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));

	LogFile::GetInstance()->Log("Created PSO");

}