#include "System/Editor/Scene/EditorSceneRuntimeBuildWarnings.h"

#include "System/Editor/Entity/EntityEditorComponentSlotKindResolve.h"

#include <algorithm>
#include <sstream>

namespace Atrum::EntityEditor {

namespace {

const char *SlotKindLabelJa(const ComponentSlotKind kind) {
  switch (kind) {
  case ComponentSlotKind::Draw:
    return "Draw";
  case ComponentSlotKind::Upd:
    return "Upd";
  case ComponentSlotKind::Data:
    return "Data";
  }
  return "?";
}

} // namespace

void EditorSceneRuntimeBuildWarnings::RecordSkippedComponentSlot(
    const std::string &entityName, const ComponentSlot &slot) {
  std::ostringstream oss;
  oss << "プレビュー: [" << entityName << "] "
      << slot.typeName << " (kind=" << SlotKindLabelJa(slot.kind)
      << ") をスキップ — ";
  if (!IsKnownEditorRuntimeComponentTypeName(slot.typeName)) {
    oss << "未対応のコンポーネント型です";
  } else if (const std::optional<ComponentSlotKind> expected =
                 TryDefaultKindForRegisteredType(slot.typeName);
             expected && *expected != slot.kind) {
    oss << "スロット種別の不一致（期待: "
        << SlotKindLabelJa(*expected) << "）";
  } else {
    oss << "追加に失敗しました（typeName と kind の組み合わせを確認）";
  }
  lines.push_back(oss.str());
}

std::string EditorSceneRuntimeBuildWarnings::FormatForImportStatusMessage()
    const {
  if (lines.empty()) {
    return {};
  }
  constexpr std::size_t kMaxLines = 4;
  std::ostringstream oss;
  const std::size_t count = std::min(lines.size(), kMaxLines);
  for (std::size_t i = 0; i < count; ++i) {
    if (i > 0) {
      oss << '\n';
    }
    oss << lines[i];
  }
  if (lines.size() > kMaxLines) {
    oss << "\n…他 " << (lines.size() - kMaxLines) << " 件";
  }
  return oss.str();
}

} // namespace Atrum::EntityEditor