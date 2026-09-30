#pragma once
// DirectInputのバージョン指定
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <cstdint>

#include <wrl/client.h>

namespace Atrum::Input {

	class DirectInput final {

	private:

		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

		~DirectInput() = default;
		DirectInput() = default;

		static DirectInput* instance_;

	public:

		DirectInput(const DirectInput& source) = delete;
		DirectInput operator=(const DirectInput& source) = delete;

		static DirectInput* GetInstance() {

			if (!instance_) {

				instance_ = new DirectInput();

			}

			return instance_;

		}

		static void Destroy() {

			if (instance_) {

				delete instance_;
				instance_ = nullptr;

			}

		}

	private:

		// 入力デバイス
		ComPtr<IDirectInput8> directInput_ = nullptr;
		// キーボード入力
		ComPtr<IDirectInputDevice8> keyboard_ = nullptr;

		// キー入力(今フレーム)
		BYTE keys_[256]{};
		// キー入力(前フレーム)
		BYTE preKeys_[256]{};

	public:

		/// <summary>
		/// 入力デバイスの初期化
		/// </summary>
		/// <param name="hInstance"> インスタンスハンドル </param>
		/// <param name="hwnd"> ウィンドウハンドル </param>
		void Initialize(HINSTANCE hInstance, HWND hwnd);

		/// <summary>
		/// 入力の更新
		/// </summary>
		void Update();

		/// <summary>
		/// キーの長押し検知
		/// </summary>
		/// <param name="keyIndex"> キー番号 </param>
		bool IsKeyPress(const uint8_t keyIndex);

		/// <summary>
		/// キーの押下検知
		/// </summary>
		/// <param name="keyIndex"> キー番号 </param>
		bool IsKeyTrigger(const uint8_t keyIndex);

		/// <summary>
		/// キーの離し検知
		/// </summary>
		/// <param name="keyIndex"> キー番号 </param>
		bool IsKeyRelease(const uint8_t keyIndex);

	};

}