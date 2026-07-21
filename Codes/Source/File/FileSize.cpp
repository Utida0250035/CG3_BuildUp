#include "File/FileSize.h"
#include "String/ConvertString.h"

#include <filesystem>
#include <cassert>

namespace Atrum::File {

	size_t GetFileSize(const std::wstring& filePath) {

		std::error_code ec;

		auto size = std::filesystem::file_size(filePath, ec);

		if (ec) {

			assert(false && "Failed to read file size");

			return 0;

		}

		return static_cast<size_t>(size);

	}

	size_t GetFileSize(const std::string& filePath) {

		return GetFileSize(StringToWString(filePath));

	}

}