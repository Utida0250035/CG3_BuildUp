#pragma once
#include "Math/Vector2.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <windows.h>
#include <cassert>

namespace Atrum::Input {

	enum class Key : uint8_t {
		A = SDL_SCANCODE_A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		_1 = SDL_SCANCODE_1, _2, _3, _4, _5, _6, _7, _8, _9, _0,
		NUM1 = SDL_SCANCODE_1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, NUM0,
		RETURN = SDL_SCANCODE_RETURN, ENTER = SDL_SCANCODE_RETURN, ESCAPE, BACKSPACE, TAB, SPACE,
		RIGHT = SDL_SCANCODE_RIGHT, LEFT, DOWN, UP,
		F1 = SDL_SCANCODE_F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		L_CTRL = SDL_SCANCODE_LCTRL, L_SHIFT, L_ALT,
		R_CTRL = SDL_SCANCODE_RCTRL, R_SHIFT, R_ALT,
	};

	enum class Mouse : uint8_t {
		Left,
		Middle,
		Right
	};

	class PlayInput final {

	private:

		/// <summary>
		/// デフォルトコンストラクタ
		/// </summary>
		PlayInput() = default;

		/// <summary>
		/// デフォルトデストラクタ
		/// </summary>
		~PlayInput() = default;


		// キー入力(今フレーム)
		bool keys_[256]{ false };
		// キー入力(前フレーム)
		bool preKeys_[256]{ false };

		// マウスカーソルの1フレーム変位
		Math::Vector2 cursorDelta_{};

		// マウスボタン
		bool mouseButtons_[SDL_BUTTON_X2 + 1]{ false };

		bool preMouseButtons_[SDL_BUTTON_X2 + 1]{ false };

		// 1フレームのマウスホイール量
		int32_t mouseWheel_ = 0;

		// マウスのクライアント座標
		Math::Vector2 cursorPos_{};

		static PlayInput* instance_;

	public:

		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns></returns>
		static PlayInput* GetInstance() {

			if (!instance_) {

				instance_ = new PlayInput();

			}

			return instance_;

		}

		static void Destroy() {

			if (instance_) {

				delete instance_;
				instance_ = nullptr;

			}

		}

		PlayInput(const PlayInput& source) = delete;
		PlayInput operator=(const PlayInput& source) = delete;

		/// <summary>
		/// フレーム終了時の処理
		/// </summary>
		void EndOfFrame();

		/* 変化量の加算 */

		void AddCursorDelta(const Math::Vector2 add) { cursorDelta_ += add; }

		void AddCursorDelta(const int32_t addX, const int32_t addY) {
			cursorDelta_.x += static_cast<float>(addX);
			cursorDelta_.y += static_cast<float>(addY);
		}

		void AddMouseWheel(const int32_t add) { mouseWheel_ += add; }

		/* 入力フラグ */

		bool IsKeyPress(const Key vKey) const { return keys_[static_cast<uint8_t>(vKey)]; }
		bool IsKeyTrigger(const Key vKey) const { return keys_[static_cast<uint8_t>(vKey)] && !preKeys_[static_cast<uint8_t>(vKey)]; }
		bool IsKeyRelease(const Key vKey) const { return !keys_[static_cast<uint8_t>(vKey)] && preKeys_[static_cast<uint8_t>(vKey)]; }

		bool IsMousePress(const Mouse vMouse) const { return mouseButtons_[static_cast<uint8_t>(vMouse)]; }
		bool IsMouseTrigger(const Mouse vMouse) const { return mouseButtons_[static_cast<uint8_t>(vMouse)] && !preMouseButtons_[static_cast<uint8_t>(vMouse)]; }
		bool IsMouseRelease(const Mouse vMouse) const { return !mouseButtons_[static_cast<uint8_t>(vMouse)] && preMouseButtons_[static_cast<uint8_t>(vMouse)]; }

		/* ゲッター */

		Math::Vector2 GetCursorPos() const { return cursorPos_; }
		Math::Vector2 GetCursorDelta() const { return cursorDelta_; }
		int32_t GetMouseWheel() const { return mouseWheel_; }

		/* セッター */

		void SetCursorPos(const int32_t x, const int32_t y) {
			cursorPos_.x = static_cast<float>(x);
			cursorPos_.y = static_cast<float>(y);
		}

		void SetKey(const uint8_t vKey, const bool isPush) {
			assert(vKey < 256);
			keys_[vKey] = isPush;
		}

		void SetMouseButton(const uint8_t vButton, const bool isPush) {
			assert(vButton <= SDL_BUTTON_X2);
			mouseButtons_[vButton] = isPush;
		}

	};

}