#pragma once

#include <cstdint>
#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <wrl/client.h>

class RenderDevice {

private:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	bool isInitialized_ = false;

	// DXGI(DirectX Graphics Infrastructure)オブジェクト生成インターフェース
	ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	// 使用するアダプタ用
	ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	// デバイス
	ComPtr<ID3D12Device> device_ = nullptr;

	/// <summary>
	/// DxgiFactoryの生成
	/// </summary>
	void CreateDxgiFactory();

	/// <summary>
	/// 使用するアダプタの選択
	/// </summary>
	void SelectAdapter();

	/// <summary>
	/// デバイスの生成
	/// </summary>
	void CreateDevice();

public:

	// コンストラクタ デフォルト
	RenderDevice() = default;

	// デストラクタ デフォルト
	~RenderDevice() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/* ゲッター */

	/// <summary>
	/// ゲッター Device
	/// </summary>
	/// <returns> Device </returns>
	ID3D12Device* GetDevice() { return device_.Get(); }

	/// <summary>
	/// ゲッター DxgiFactory
	/// </summary>
	/// <returns> DxgiFactory </returns>
	IDXGIFactory7* GetDxgiFactory() { return dxgiFactory_.Get(); }

};