#pragma once

#include <vector>

namespace Atrum {
class Scene;
}

namespace Atrum::SceneFlow {

struct ReleaseCarouselEntry;

/// Release ビルド専用: Space / パッド A で登録シーンを順番・ループ遷移
class SceneReleaseCarousel final {
public:
  static SceneReleaseCarousel &Instance();

  /// 決定入力で次シーンへ遷移したら true（呼び出し側は同一フレームのシーン Update を省略可）
  bool TryAdvanceOnSubmit();

  void SyncIndexFromActiveScene(Scene *sceneNow);

private:
  SceneReleaseCarousel();

  bool AdvanceToNextScene();
  int FindIndexForScene(Scene *scene) const;
  Scene *ResolveSceneForIndex(int index) const;

  std::vector<ReleaseCarouselEntry> entries_{};
  int activeIndex_ = 0;
};

} // namespace Atrum::SceneFlow
