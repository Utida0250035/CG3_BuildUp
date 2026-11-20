#pragma once

namespace Atrum {

class SceneResult;

/// リザルトで Submit 後にフェードでタイトルへ（タイトル BGM を deferred 登録）
struct SceneResultSubmitTransition final {
  static bool TryBeginOnSubmit(SceneResult &scene);
};

} // namespace Atrum