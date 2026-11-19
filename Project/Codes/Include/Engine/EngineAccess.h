#pragma once

#include "Engine/Resource/LightData.h"

#include <cstdint>
#include <memory>
#include <string>

namespace Atrum {

class AtrumEngine;
class AssetModel;
class FrameDeltaTime;
class TextureStorage;

namespace Input {
class PlayInput;
}

namespace Audio {
class AudioManager;
}

namespace Physics {
class PhysicsManager;
}

/// <summary>
/// 実行中の AtrumEngine シングルトンを返す（メンバ呼び出しには AtrumEngine.h が必要）
/// </summary>
AtrumEngine *EngineInstance();

FrameDeltaTime *EngineFrameDeltaTime();
Input::PlayInput *EnginePlayInput();
Audio::AudioManager *EngineAudioManager();
Physics::PhysicsManager *EnginePhysicsManager();
TextureStorage *EngineTextureStorage();

uint32_t EngineGetTexture(const std::string &filePath);
std::shared_ptr<AssetModel> EngineGetModel(const std::string &directoryPathObj,
                                         const std::string &objFileName,
                                         const std::string &directoryPathMtl,
                                         const std::string &mtlFileName);
void EngineSetDirectionalLightData(const DirectionalLightData &data);

} // namespace Atrum
