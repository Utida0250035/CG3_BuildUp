#pragma once
#include <cstdint>
#include <string>

namespace Atrum::Hash {

	/// <summary>
	/// 文字列をハッシュ化(FNV-1aハッシュ)
	/// </summary>
	/// <param name="str"> 文字列リテラル </param>
	/// <returns> ハッシュ </returns>
	constexpr uint64_t hash64_str(const char* str) {

		uint64_t hash = 14695981039346656037ull;
		const uint64_t fnv_prime = 1099511628211ull;

		while (*str) {

			hash ^= static_cast<uint64_t>(static_cast<uint8_t>(*str++));
			hash *= fnv_prime;

		}

		return hash;

	}

	/// <summary>
	/// 文字列をハッシュ化(FNV-1aハッシュ)
	/// </summary>
	/// <param name="string"> 文字列 </param>
	/// <returns> ハッシュ </returns>
	constexpr uint64_t hash64_str(const std::string& string) {

		return hash64_str(string.c_str());

	}

	/// <summary>
	/// 文字列をハッシュ化(FNV-1aハッシュ)
	/// </summary>
	/// <param name="wstr"> 文字列リテラル(wide) </param>
	/// <returns> ハッシュ </returns>
	constexpr uint64_t hash64_str(const wchar_t* wstr) {

		uint64_t hash = 14695981039346656037ull;
		const uint64_t fnv_prime = 1099511628211ull;

		while (*wstr) {

			wchar_t c = *wstr++;

			// wchar_t のサイズ(環境対応)分ループを回す
			for (size_t i = 0; i < sizeof(wchar_t); ++i) {
				// 1バイトずつ確実にハッシュに混ぜ込む
				hash ^= static_cast<uint64_t>(static_cast<uint8_t>((c >> (i * 8)) & 0xFF));
				hash *= fnv_prime;
			}

		}

		return hash;

	}

	/// <summary>
	/// 文字列をハッシュ化(FNV-1aハッシュ)
	/// </summary>
	/// <param name="wstr"> 文字列(wide) </param>
	/// <returns></returns>
	constexpr uint64_t hash64_str(const std::wstring& wstr) {

		return hash64_str(wstr.c_str());

	}

	/// <summary>
	/// 文字列リテラルのハッシュ化ユーザー定義リテラル
	/// </summary>
	/// <param name="str"> 文字列リテラル </param>
	/// <returns> ハッシュ </returns>
	constexpr uint64_t operator"" _hash64(const char* str, size_t) {

		return hash64_str(str);

	}

	/// <summary>
	/// 文字列リテラルのハッシュ化ユーザー定義リテラル
	/// </summary>
	/// <param name="str"> 文字列リテラル(wide) </param>
	/// <returns> ハッシュ </returns>
	constexpr uint64_t operator"" _hash64(const wchar_t* wstr, size_t) {

		return hash64_str(wstr);

	}

}