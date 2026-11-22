#pragma once

namespace Atrum {

namespace Audio {
class AudioManager;
}

class CommandChangeScene;

/// フレームループ上で AudioManager を進める（ストリーミング補充・Waiting 処理）
void TickGameOrderFrameAudio(Audio::AudioManager *audio,
                             CommandChangeScene *changeScene);

} // namespace Atrum