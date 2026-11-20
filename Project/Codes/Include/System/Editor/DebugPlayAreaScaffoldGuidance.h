#pragma once

namespace Atrum::Editor::DebugPlayAreaScaffoldGuidance {

/// scaffold 直後にユーザーへ示す手順（Python scaffold と verify で文言を突合）
inline constexpr const char kPostGenerateStepsJa[] =
    "生成しました。次: プロジェクト一覧で開く →「エディタ内プレビュー」または"
    "「シーンを実行」(BuildUp)。起動シーン化は「起動シーンに設定」。F2 はテンプレ BuildUp。";

} // namespace Atrum::Editor::DebugPlayAreaScaffoldGuidance