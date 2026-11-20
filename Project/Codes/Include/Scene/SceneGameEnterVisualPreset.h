#pragma once

/// EnterScene 直後に本編見た目（game_scene_lighting.json）を適用し CB を同期する。
/// DrawScene 毎の再適用は行わない（ランタイム調整と game_scene_lighting.json 反映の保持）。
void ApplySceneGameVisualPresetOnEnter();