#pragma once

#include <cstdint>

namespace Atrum {

class Entity;

enum class DrawCmpGroup : uint64_t {
  NONE = 0,
  MESH_BODY = 0,
  HIT_SPHERE_VISUAL = 10,
  ATTACK_TELEGRAPH = 15,
  BULLET_VISUAL = 20,
};

// 全ての描画パーツの基底
class DrawComponent {
private:
	Entity* owner_ = nullptr; // 所有者（Enemyなど）への参照

public:
	virtual ~DrawComponent() = default;

	virtual constexpr DrawCmpGroup DrawGroup() const { return DrawCmpGroup::NONE; }

	virtual void Initialize() {}

	virtual void Draw() {}

	virtual void Finalize() {}

	virtual void ResolveDependence() {}

protected:
	const Entity* GetOwner() const { return owner_; }
	Entity& RefOwner() { return *owner_; }

public:
	void SetOwner(Entity* owner) { owner_ = owner; }
};

} // namespace Atrum