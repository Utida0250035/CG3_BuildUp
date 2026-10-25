#pragma once

#include "Math/Vector4.h"

#include <cstdint>
#include <initializer_list>

namespace Atrum {

class AtrumEngine;
class Camera;

namespace SceneScreenUi {

/// Draw 中ではなく EnterScene 等で呼ぶ（GetTexture は GPU コマンドを発行する）
void PreloadTitleUiTextures();
void PreloadResultUiTextures();
void PreloadGameIntroUiTextures();

void PrepareDraw(AtrumEngine *engine, Camera *camera);

void DrawTitleSceneUi(AtrumEngine *engine, Camera *camera);
void DrawResultSceneUi(AtrumEngine *engine, Camera *camera);
void DrawGameIntroHowToNextUi(AtrumEngine *engine, Camera *camera);

} // namespace SceneScreenUi
} // namespace Atrum
