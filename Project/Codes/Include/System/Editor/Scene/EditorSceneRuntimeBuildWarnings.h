#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include <string>
#include <vector>

namespace Atrum::EntityEditor {

/// プレビュー／ランタイム実体化時にスキップしたコンポーネントの警告
struct EditorSceneRuntimeBuildWarnings {
  std::vector<std::string> lines;

  void Clear() { lines.clear(); }

  void RecordSkippedComponentSlot(const std::string &entityName,
                                  const ComponentSlot &slot);

  /// プロジェクトパネルの status 表示用（長いときは先頭のみ）
  std::string FormatForImportStatusMessage() const;
};

} // namespace Atrum::EntityEditor