#pragma once

namespace Atrum::Editor::DebugPlayAreaRuntimePreviewGuidance {

/// ランタイムプレビュー手順の正本（README・scaffold 出力・verify で突合）
inline constexpr const char kStepsJa[] =
    "ランタイムプレビュー: プロジェクト一覧で JSON を選択 →「シーンを実行」(BuildUp)。"
    "生成直後は「作成して BuildUp」。エディタ内のみなら「作成してエディタ内プレビュー」。"
    "F2=FeatureTest 正本、F6=combat_bounds_sandbox。";

/// プロジェクトパネルに常時表示する短い案内
inline constexpr const char kPanelHintJa[] =
    "BuildUp プレビューは「シーンを実行」または「作成して BuildUp」。";

} // namespace Atrum::Editor::DebugPlayAreaRuntimePreviewGuidance