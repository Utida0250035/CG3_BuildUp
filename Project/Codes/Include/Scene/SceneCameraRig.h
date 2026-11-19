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
  SceneCameraRig();
  ~SceneCameraRig();

  SceneCameraRig(const SceneCameraRig &) = delete;
  SceneCameraRig &operator=(const SceneCameraRig &) = delete;
  SceneCameraRig(SceneCameraRig &&) noexcept;
  SceneCameraRig &operator=(SceneCameraRig &&) noexcept;

  void EnsureAutoSpawnedCameras(const Scene *ownerScene = nullptr);
  void Clear();

  Camera *GetPrimaryCamera(const Scene *ownerScene = nullptr);
  const Camera *GetPrimaryCamera(const Scene *ownerScene = nullptr) const;

  CmpCamera *GetPrimaryCmpCamera(const Scene *ownerScene = nullptr);
  const CmpCamera *GetPrimaryCmpCamera(const Scene *ownerScene = nullptr) const;

  void PrepareActiveCameraBeforeDraw(const Scene &scene);

private:
  std::vector<std::unique_ptr<Entity>> entities_;
  bool autoSpawnAttempted_ = false;

  CmpCamera *FindPrimaryCmpCamera(const Scene *ownerScene);
  const CmpCamera *FindPrimaryCmpCamera(const Scene *ownerScene) const;
};

} // namespace Atrum
