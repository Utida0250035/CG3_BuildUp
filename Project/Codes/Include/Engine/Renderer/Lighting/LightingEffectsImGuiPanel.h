#pragma once

namespace Atrum::Visual {

class VolumetricFog;
class GodRays;
class DirectionalShadow;

/// Lighting::ImGui とエンジン配線の共通 UI（フォグ／ゴッドレイ／影）。
void DrawLightingEffectsImGuiPanel(VolumetricFog &volumetricFog, GodRays &godRays,
                                 DirectionalShadow &directionalShadow);

} // namespace Atrum::Visual