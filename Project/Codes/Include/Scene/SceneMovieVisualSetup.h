#pragma once

#include <memory>

namespace Atrum {

class Camera;
class Entity;

/// ムービーカメラの先頭姿勢を適用したあと SkyDome を生成する
std::unique_ptr<Entity> CreateSkyDomeBoundToCamera(Camera *camera);

} // namespace Atrum