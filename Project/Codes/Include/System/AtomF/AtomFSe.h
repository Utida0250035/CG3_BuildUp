#pragma once

#include "System/PlaySe.h"

namespace Atrum::AtomF {

/// atomF の戦闘 SE（既存アセットを役割名で包む）。ファイルが無ければ無音。
inline void PlayBossBulletWarnSe() {
  PlaySeCandidates({"./Assets/Audios/seBossBulletWarn.mp3",
                    "./Resources/seBossBulletWarn.mp3"});
}
inline void PlayBossMeleeWarnSe() {
  PlaySeCandidates({"./Assets/Audios/seBossMeleeWarn.mp3",
                    "./Resources/seBossMeleeWarn.mp3"});
}
inline void PlayBossShotSe() {
  PlaySeCandidates(
      {"./Assets/Audios/seBossShot.mp3", "./Resources/seBossShot.mp3"});
}
inline void PlayBossHitSe() {
  PlaySeCandidates(
      {"./Assets/Audios/seBossHit.mp3", "./Resources/seBossHit.mp3"});
}
inline void PlayBossHpBarGlowSe() {
  PlaySeCandidates({"./Assets/Audios/seBossHpBarGlow.mp3",
                    "./Resources/seBossHpBarGlow.mp3"});
}
inline void PlayBossDeathBoomSe() {
  PlaySeCandidates({"./Assets/Audios/seBossDeathBoom.mp3",
                    "./Resources/seBossDeathBoom.mp3"});
}
inline void PlayAtomFEnemyShotSe() {
  PlaySeCandidates(
      {"./Assets/Audios/seEnemyShot.mp3", "./Resources/seEnemyShot.mp3"});
}

} // namespace Atrum::AtomF
