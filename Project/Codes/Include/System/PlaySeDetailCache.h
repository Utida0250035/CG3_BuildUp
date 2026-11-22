#pragma once

#include <cstddef>
#include <string>

namespace Atrum::PlaySeDetail {

void ClearSeIndexPathCache();

size_t GetOrLoadSeIndex(const std::string &path);

} // namespace Atrum::PlaySeDetail