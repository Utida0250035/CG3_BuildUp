#pragma once

#include "Engine/Resource/LightData.h"

#include <cstdint>
#include <memory>
#include <string>

namespace Atrum {

class AtrumEngine;
class AssetModel;
class FrameDeltaTime;
class CommandContext;
class ModelStorage;
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
ModelStorage *EngineModelStorage();
CommandContext *EngineCommandContextDirect();

uint32_t EngineGetTexture(const std::string &filePath);
std::shared_ptr<AssetModel> EngineGetModel(const std::string &directoryPathObj,
                                         const std::string &objFileName,
                                         const std::string &directoryPathMtl,
                                         const std::string &mtlFileName);
void EngineSetDirectionalLightData(const DirectionalLightData &data);

/// ウィンドウのクライアントサイズ（StageView アスペクト等）。取得できないとき false。
bool EngineTryGetClientViewportSize(int32_t &outWidth, int32_t &outHeight);

} // namespace Atrum
