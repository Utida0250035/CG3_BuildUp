#pragma once

#include "System/Entity/ComponentData.h"
#include <string>

namespace Atrum {

/// ランタイム配布向けテクスチャ参照（パスのみ保持・エディタプレビュー用ロードは別経路）
class CmpRuntimeTexturePath : public ComponentData {
private:
  std::string texturePath_{};

public:
  const std::string &TexturePath() const { return texturePath_; }
  void SetTexturePath(const std::string &path) { texturePath_ = path; }
};

} // namespace Atrum