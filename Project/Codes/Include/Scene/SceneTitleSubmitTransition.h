#pragma once

namespace Atrum {

class SceneTitle;

/// タイトルで Submit が押された直後にフェード遷移（イントロムービー）を開始する
struct SceneTitleSubmitTransition final {
  static bool TryBeginOnSubmit(SceneTitle &scene);
};

} // namespace Atrum