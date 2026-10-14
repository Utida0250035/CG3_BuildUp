#pragma once

#include <vector>

namespace Atrum {

class CmpHitSphere;

class Entity;

class HitJudge {

private:
  std::vector<CmpHitSphere *> cmpHitSpheres_{};

  HitJudge() = default;
  ~HitJudge() = default;

public:
  std::vector<Entity *> IsHitOthers(CmpHitSphere *cmpHitSphere);

  void Register(CmpHitSphere *cmpHitSphere);
  void Remove(CmpHitSphere *CmpHitSphere);

  HitJudge operator=(const HitJudge &source) = delete;
  HitJudge(const HitJudge &source) = delete;

  static HitJudge *GetInstance() {
    static HitJudge instance;
    return &instance;
  }
};

} // namespace Atrum