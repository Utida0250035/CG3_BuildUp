#include "Scene/GameOrderAudioPolicy.h"

#include "Audio/Audio.h"
#include "Scene/CommandChangeScene.h"

namespace Atrum {

void TickGameOrderFrameAudio(Audio::AudioManager *audio,
                             CommandChangeScene *changeScene) {
  if (audio == nullptr) {
    return;
  }
  audio->Update();
  // シーン遷移中の BGM ダック／再開は各シーン Update と CommandChangeScene が担当。
  (void)changeScene;
}

} // namespace Atrum