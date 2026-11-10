#pragma once

#include <memory>
#include <vector>

namespace Atrum {

class Camera;
class CmpCamera;
class Entity;
class Scene;

/// シーンに属するカメラエンティティ（自動スポーン・primary 解決）
class SceneCameraRig final {
public:
  void EnsureAutoSpawnedCameras();
  void Clear();

  Camera *GetPrimaryCamera();
  const Camera *GetPrimaryCamera() const;

  CmpCamera *GetPrimaryCmpCamera();
  const CmpCamera *GetPrimaryCmpCamera() const;

  void PrepareActiveCameraBeforeDraw(const Scene &scene);

  const std::vector<std::unique_ptr<Entity>> &Entities() const { return entities_; }

private:
  std::vector<std::unique_ptr<Entity>> entities_;
  bool autoSpawnAttempted_ = false;

  CmpCamera *FindPrimaryCmpCamera() const;
};

} // namespace Atrum
