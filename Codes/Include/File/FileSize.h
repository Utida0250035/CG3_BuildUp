#pragma once

#include <string>

namespace Atrum::File {

	size_t GetFileSize(const std::wstring& filePath);

	size_t GetFileSize(const std::string& filePath);

}