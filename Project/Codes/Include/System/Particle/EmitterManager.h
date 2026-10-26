#pragma once

#include "Math/Vector3.h"
#include "System/Particle/ParticleEmitter.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Atrum {
class Camera;
} // namespace Atrum

namespace Atrum::Particle::Emitter {

/// <summary>
/// Visual::Particle::Emitter の生成・更新・描画を一括管理する
/// </summary>
class EmitterManager {
public:
  /// 同名エミッタを複数登録できる 1 エントリ
  struct RegisteredEmitter {
    uint64_t nameHash = 0;
    std::unique_ptr<Atrum::Visual::Particle::Emitter> emitter;
  };

private:
  static EmitterManager *instance_;

  EmitterManager() = default;
  ~EmitterManager() = default;

  std::vector<RegisteredEmitter> emitters_{};

  Camera *camera_ = nullptr;

public:
  EmitterManager(const EmitterManager &) = delete;
  EmitterManager &operator=(const EmitterManager &) = delete;

  static EmitterManager *Instance();
  static void Destroy();

  /// <summary>
  /// 名前を付けてエミッタを配列末尾に追加する（同名可）
  /// </summary>
  void Register(const std::string &name,
                std::unique_ptr<Atrum::Visual::Particle::Emitter> &&emitter);

  /// <summary>
  /// 新規エミッタを生成して Register し、指定位置で 1 回 Emit する
  /// </summary>
  Atrum::Visual::Particle::Emitter *
  AddOneShotEmitter(const std::string &emitterName, const char *jsonFilePath,
                    const Math::Vector3 &worldPosition);

  /// 最初に見つかった同名エミッタ（後方互換）
  Atrum::Visual::Particle::Emitter *Find(const std::string &name);
  Atrum::Visual::Particle::Emitter *FindByNameHash(uint64_t nameHash);

  void SetCamera(Camera *camera);

  /// 同名の全エミッタで Emit する
  void Emit(const std::string &name);
  void EmitAll();

  void Update();
  void Draw();

  /// <summary>
  /// アクティブなパーティクルが1つもないエミッタを取り除く
  /// </summary>
  void RemoveInactive();

  void Clear();

  const std::vector<RegisteredEmitter> &GetEmitters() const { return emitters_; }
};

} // namespace Atrum::Particle::Emitter
