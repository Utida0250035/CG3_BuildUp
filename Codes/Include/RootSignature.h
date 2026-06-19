#pragma once

#include <wrl/client.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

class RootSignature {

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:

	// RootSignature
	ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	// RootSignatureの生成結果
	ComPtr<ID3DBlob> signatureBlob_ = nullptr;

	// RootSignatureのエラー結果
	ComPtr<ID3DBlob> errorBlob_ = nullptr;

public:

	void Initialize(ID3D12Device* device);

	/* ゲッター */

	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

};