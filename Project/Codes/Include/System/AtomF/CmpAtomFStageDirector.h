#pragma once

#include "System/Entity/ComponentUpd.h"

#include <cstddef>

namespace Atrum {

/// ステージ進行: 雑魚が全滅するたびに次の群れを出し、最後の群れの後にボスを出現させる。
/// ゲームの勝敗（ボス撃破で全滅→リザルト）は Lua の game_flow が見る。
class CmpAtomFStageDirector final : public ComponentUpd {
public:
  constexpr UpdCmpGroup UpdGroup() const override {
    return UpdCmpGroup::MOVEMENT;
  }

  void ResolveDependence() override;
  void Update() override;

private:
  class FrameDeltaTime *frameDeltaTime_ = nullptr;
  std::size_t nextWaveIndex_ = 0;
  float clearedElapsedSeconds_ = 0.0f;
  bool bossCalled_ = false;
};

} // namespace Atrum
