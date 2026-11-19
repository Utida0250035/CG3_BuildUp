#include "Audio/StreamLoadPolicy.h"

#include "File/FileSize.h"

#include <algorithm>
#include <cctype>
#include <filesystem>

namespace Atrum::Audio {

namespace {

std::string LowerPathExtension(const std::string& filePath) {

	std::string extension = std::filesystem::path(filePath).extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(),
	               [](const unsigned char character) {
		               return static_cast<char>(std::tolower(character));
	               });
	return extension;

}

size_t EstimateDecodedPcmBytes(const size_t fileSizeOnDisk, const std::string& extension) {

	if (extension == ".wav" || extension == ".wave") {
		return fileSizeOnDisk;
	}

	if (extension == ".mp3" || extension == ".m4a" || extension == ".aac" ||
	    extension == ".wma" || extension == ".ogg" || extension == ".flac" ||
	    extension == ".mp4") {
		return fileSizeOnDisk * 12;
	}

	return fileSizeOnDisk * 8;

}

} // namespace

bool IsSuitableStreamingLoad(const std::string& filePath, size_t fileSizeOnDisk) {

	if (fileSizeOnDisk == 0) {
		fileSizeOnDisk = File::GetFileSize(filePath);
	}

	if (fileSizeOnDisk > kStreamingLoadThresholdBytes) {
		return true;
	}

	const std::string extension = LowerPathExtension(filePath);
	const size_t estimatedPcmBytes = EstimateDecodedPcmBytes(fileSizeOnDisk, extension);
	return estimatedPcmBytes > kStreamingLoadThresholdBytes;

}

} // namespace Atrum::Audio