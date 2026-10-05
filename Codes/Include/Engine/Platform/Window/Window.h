#pragma once
#include <SDL.h>
#include <string>
#include <windows.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

namespace Atrum {

	class Window {

	private:

		// SDL2ウィンドウ実体
		SDL_Window* ptr_ = nullptr;

		static inline constexpr int32_t kDefaultClientWidth = 1280;

		static inline constexpr int32_t kDefaultClientHeight = 720;

		// クライアント領域の幅
		int32_t clientWidth_ = kDefaultClientWidth;

		// クライアント領域の高さ
		int32_t clientHeight_ = kDefaultClientHeight;

		// ウィンドウ矩形
		RECT wRc_{};

		// ウィンドウハンドル
		HWND hWnd_{};

		HINSTANCE hInstance_{};

		// ビューポート
		D3D12_VIEWPORT viewport_{};

		// シザー矩形
		D3D12_RECT scissorRect_{};

		void SizeInit(const int32_t clientWidth, const int32_t clientHeight);

		void UpdateMetrics();

	public:

		static LRESULT CALLBACK MySubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR);

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Window();

		/// <summary>
		/// ウィンドウの初期化
		/// </summary>
		/// <param name="windowLabel"></param>
		/// <param name="clientWidth"></param>
		/// <param name="clientHeight"></param>
		/// <returns></returns>
		void Initialize(const std::string& windowLabel, const int32_t& clientWidth = kDefaultClientWidth, const int32_t& clientHeight = kDefaultClientHeight);

		void Resize(const int32_t newWidth, const int32_t newHeight);

		/* ゲッター */

		/// <summary>
		/// 
		/// </summary>
		/// <returns> ウィンドウ実体のポインタ </returns>
		SDL_Window* GetWindow() { return ptr_; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns> クライアント領域の幅 </returns>
		int32_t GetClientWidth() const { return clientWidth_; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns> クライアント領域の高さ </returns>
		int32_t GetClientHeight() const { return clientHeight_; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns> ウィンドウ矩形 </returns>
		RECT GetWRc() const { return wRc_; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns> ウィンドウハンドル </returns>
		HWND GetHWnd() const { return hWnd_; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns> インスタンスハンドル </returns>
		HINSTANCE GetHInstance() const { return hInstance_; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns> ビューポート </returns>
		const D3D12_VIEWPORT& GetViewport() const { return viewport_; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns> シザー矩形 </returns>
		const D3D12_RECT& GetScissorRect() const { return scissorRect_; }

	};

}