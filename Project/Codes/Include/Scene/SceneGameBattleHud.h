#pragma once

#include <cstdint>

namespace Atrum {

class AtrumEngine;

/// <summary>
/// ゲームシーンの HP 表示（スプライト）
/// </summary>
void DrawSceneGameBattleHud(AtrumEngine *engine, uint32_t playerHealth,
                            uint32_t playerHealthMax, uint32_t bossHealth,
                            uint32_t bossHealthMax,
                            float bossHpBarReveal01 = 1.0f);

} // namespace Atrum
