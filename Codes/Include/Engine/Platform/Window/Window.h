#pragma once
#include "Debug/Log.h"
#include <SDL.h>
#include <string>
#include <windef.h>

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

namespace Atrum {

	class Window {

	private:

		// SDL2ウィンドウ実体
		SDL_Window* ptr_ = nullptr;

		// クライアント領域の幅
		int32_t clientWidth_ = 0;

		// クライアント領域の高さ
		int32_t clientHeight_ = 0;

		// ウィンドウ矩形
		RECT wrc_{};

		// ウィンドウハンドル
		HWND hwnd_{};


		// ビューポート
		D3D12_VIEWPORT viewport_{};

		// シザー矩形
		D3D12_RECT scissorRect_{};

		void UpdateMetrics();

	public:

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Window() {

			if (ptr_) {

				SDL_DestroyWindow(ptr_);

			}

			SDL_Quit();

			Debug::LogFile::GetInstance()->Log("SDL2: Quit");

		}

		/// <summary>
		/// ウィンドウの初期化
		/// </summary>
		/// <param name="windowLabel"></param>
		/// <param name="clientWidth"></param>
		/// <param name="clientHeight"></param>
		/// <returns></returns>
		void Initialize(const std::string& windowLabel, const int32_t& clientWidth, const int32_t& clientHeight);


		void Resize(const int32_t clientWidth, const int32_t clientHeight) {

			clientWidth_ = clientWidth;
			clientHeight_ = clientHeight;

			UpdateMetrics();

		}

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
		RECT GetWrc() const { return wrc_; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns> ウィンドウハンドル </returns>
		HWND GetHwnd() const { return hwnd_; }


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