#pragma once

namespace Atrum {

namespace Audio {
class Manager;
}

class CommandChangeScene;

/// フレームループ上で AudioManager を進める（ストリーミング補充・Waiting 処理）
void TickGameOrderFrameAudio(Audio::Manager *audio,
                             CommandChangeScene *changeScene);

} // namespace Atrum