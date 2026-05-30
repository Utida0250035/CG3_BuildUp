#include "ConvertString.h"
#include <Windows.h>
#include <cassert>

std::wstring StringToWString(const std::string& str) {

	std::wstring result;

	// 文字数を計測
	int needSize = MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.length()),
		nullptr,
		0
	);

	assert(needSize >= 0);

	// サイズ各頬
	result.resize(needSize);

	// 変換
	MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		static_cast<int>(str.length()),
		result.data(),
		static_cast<int>(result.size())
	);

	return result;

}

std::string WStringToString(const std::wstring& str) {

	std::string result;

	// 文字数を計測
	int needSize = WideCharToMultiByte(
		CP_ACP,
		0,
		str.c_str(),
		static_cast<int>(str.length()),
		nullptr,
		0,
		nullptr,
		nullptr
	);

	assert(needSize >= 0);

	// サイズ確保
	result.resize(needSize);

	// 変換
	WideCharToMultiByte(
		CP_ACP,
		0,
		str.c_str(),
		static_cast<int>(str.length()),
		result.data(),
		static_cast<int>(result.size()),
		nullptr,
		nullptr
	);

	return result;

}