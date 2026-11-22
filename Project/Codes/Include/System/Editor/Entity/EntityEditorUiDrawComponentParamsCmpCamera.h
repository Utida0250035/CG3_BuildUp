#pragma once

namespace Atrum::EntityEditor {

struct ComponentSlot;

/// CmpCamera のインスペクタ（entity_camera_components.md 準拠の投影パラメータ）
bool TryDrawCmpCameraComponentParams(ComponentSlot &slot, bool &outChanged);

} // namespace Atrum::EntityEditor