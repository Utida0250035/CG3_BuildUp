#pragma once
#include <cstdint>

/// <summary>
/// 文字列をハッシュ化(FNV-1aハッシュ)
/// </summary>
/// <param name="str"> 文字列リテラル </param>
/// <returns> ハッシュ </returns>
constexpr uint64_t hash64_str(const char* str) {

	uint64_t hash = 14695981039346656037ull;
	const uint64_t fnv_prime = 1099511628211ull;

	while (*str) {

		hash ^= static_cast<uint64_t>(*str++);
		hash *= fnv_prime;

	}

	return hash;

}

/// <summary>
/// 文字列リテラルのハッシュ化ユーザー定義リテラル
/// </summary>
/// <param name="str"> 文字列リテラル </param>
/// <param name=""> 空のサイズ(コンパイルエラー回避用) </param>
/// <returns> ハッシュ </returns>
constexpr uint64_t operator"" _hash64(const char* str, size_t) {

	return hash64_str(str);

}