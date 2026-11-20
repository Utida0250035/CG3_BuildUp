#pragma once

namespace Atrum {

class Scene;

/// launch_config 等から Release／非 Development 起動シーンを決める（未設定時は Title）
Scene *CreateInitialSceneForReleaseBoot();

} // namespace Atrum