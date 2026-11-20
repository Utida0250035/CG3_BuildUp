#pragma once

namespace Atrum {

class SceneBuildUp;

/// F1 で returnScene へフェード復帰（Update／遷移ブロック時の共通処理）
namespace SceneBuildUpF1ReturnRequest {

bool TryRequestReturnViaF1(SceneBuildUp &scene);

} // namespace SceneBuildUpF1ReturnRequest

} // namespace Atrum