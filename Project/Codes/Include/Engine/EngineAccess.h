#pragma once

namespace Atrum {

class AtrumEngine;
class FrameDeltaTime;
class TextureStorage;

namespace Input {
class PlayInput;
}

namespace Audio {
class AudioManager;
}

/// <summary>
/// 実行中の AtrumEngine シングルトンを返す（メンバ呼び出しには AtrumEngine.h が必要）
/// </summary>
AtrumEngine *EngineInstance();

FrameDeltaTime *EngineFrameDeltaTime();
Input::PlayInput *EnginePlayInput();
Audio::AudioManager *EngineAudioManager();
TextureStorage *EngineTextureStorage();

} // namespace Atrum
