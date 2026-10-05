#include <Windows.h>

#include "Engine/Platform/Window/Window.h"
#include <cassert>
#include <SDL_syswm.h>
#include <imgui/imgui_impl_dx12.h>

namespace Atrum {

	void Window::Initialize(const std::string& windowLabel, const int32_t& clientWidth, const int32_t& clientHeight) {

		// SDLの初期化
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {

			// エラーハンドリング
			assert(false);
			return;

		}

		// ウィンドウの生成
		ptr_ = SDL_CreateWindow(
			windowLabel.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			clientWidth,
			clientHeight,
			SDL_WINDOW_SHOWN
		);

		// DirectX連携のためにHWNDを取得
		SDL_SysWMinfo wmInfo{};
		SDL_VERSION(&wmInfo.version);

		SizeInit(clientWidth, clientHeight);

		if (SDL_GetWindowWMInfo(ptr_, &wmInfo)) {

			hWnd_ = wmInfo.info.win.window;

			hInstance_ = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd_, GWLP_HINSTANCE));

		}

	}

	void Window::UpdateMetrics() {

		// クライアント領域のサイズと同等にして画面全体を表示領域とする

		// ビューポートの設定
		viewport_.Width = static_cast<FLOAT>(clientWidth_);
		viewport_.Height = static_cast<float>(clientHeight_);
		viewport_.TopLeftX = 0.0f;
		viewport_.TopLeftY = 0.0f;
		viewport_.MinDepth = 0.0f;
		viewport_.MaxDepth = 1.0f;


		// シザー矩形の設定
		scissorRect_.left = 0;
		scissorRect_.right = clientWidth_;
		scissorRect_.top = 0;
		scissorRect_.bottom = clientHeight_;

	}

	void Window::SizeInit(const int32_t clientWidth, const int32_t clientHeight) {

		clientWidth_ = clientWidth;
		clientHeight_ = clientHeight;

		UpdateMetrics();

	}

	void Window::Resize(const int32_t newWidth, const int32_t newHeight) {

		SDL_SetWindowSize(ptr_, newWidth, newHeight);

	}

}