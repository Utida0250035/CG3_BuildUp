#include "System/PlaySeDetailCache.h"

#include "Audio/Audio.h"
#include "Engine/AtrumEngine.h"

#include <filesystem>
#include <unordered_map>

namespace Atrum::PlaySeDetail {

namespace {

std::unordered_map<std::string, size_t> &SeIndexPathCache() {

	static std::unordered_map<std::string, size_t> cache;
	return cache;

}

void PruneMissingPathsFromCache() {

	auto &cache = SeIndexPathCache();
	for (auto iterator = cache.begin(); iterator != cache.end();) {
		if (!std::filesystem::exists(iterator->first)) {
			iterator = cache.erase(iterator);
		} else {
			++iterator;
		}
	}

}

} // namespace

void ClearSeIndexPathCache() {

	SeIndexPathCache().clear();

}

size_t GetOrLoadSeIndex(const std::string &path) {

	if (!std::filesystem::exists(path)) {
		SeIndexPathCache().erase(path);
		return static_cast<size_t>(-1);
	}

	PruneMissingPathsFromCache();

	Audio::AudioManager *audio = ::Atrum::EngineAudioManager();

	const size_t index = audio->Load(path);
	if (index == static_cast<size_t>(-1)) {
		SeIndexPathCache().erase(path);
		return static_cast<size_t>(-1);
	}

	SeIndexPathCache()[path] = index;
	return index;

}

} // namespace Atrum::PlaySeDetail