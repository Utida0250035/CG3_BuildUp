#pragma once

#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

class PipelineState {

private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	/* InputLayout */

	// InputLayoutの設定
	D3D12_INPUT_ELEMENT_DESC inputElementDescriptions_[3]{};

	// inputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};


	/* 描画State */

	// BlendState
	D3D12_BLEND_DESC blendDesc_{};

	// RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	// DepthStencilState
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};


	/* PSO */

	// PSOの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc_{};

	// PSO
	ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	void SetupInputLayout();
	void SetupBlendState();
	void SetupRasterizerState();
	void SetupDepthStencilState();

public:
	
	void Initialize(ID3D12RootSignature* rootSignature, ID3D12Device* device, IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob);

	/* ゲッター */

	ID3D12PipelineState* GetPSO() const { return pipelineState_.Get(); }

};