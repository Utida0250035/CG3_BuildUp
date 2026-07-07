#include "Input/DirectInput.h"
#include <cassert>

void DirectInput::Initialize(HINSTANCE hInstance, HWND hwnd) {

	/* 入力デバイス */
	
	HRESULT hr = DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_,
		nullptr
	);
	assert(SUCCEEDED(hr));


	/* キーボード入力 */

	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));

	// 標準キーボード形式
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	// 排他制御レベルのリセット
	hr = keyboard_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

}

void DirectInput::Update() {

	// キーボード入力の取得開始
	keyboard_->Acquire();

	// 前フレームのキー入力取得
	memcpy(preKeys_, keys_, sizeof(keys_));

	for (auto& key : keys_) {

		key = 0u;

	}

	// 全キー入力の取得
	keyboard_->GetDeviceState(sizeof(keys_), keys_);

}

bool DirectInput::IsKeyPress(const uint8_t keyIndex) {

	if (keys_[keyIndex]) {

		return true;

	}

	return false;

}

bool DirectInput::IsKeyTrigger(const uint8_t keyIndex) {

	if (keys_[keyIndex] && !preKeys_[keyIndex]) {

		return true;

	}

	return false;

}

bool DirectInput::IsKeyRelease(const uint8_t keyIndex) {

	if (!keys_[keyIndex] && preKeys_[keyIndex]) {

		return true;

	}

	return false;

}