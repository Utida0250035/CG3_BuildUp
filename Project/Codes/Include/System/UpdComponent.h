#pragma once

#include <cstdint>

namespace Atrum {

class Entity;

enum class UpdCmpGroup : uint64_t { NONE_GROUPING, INPUT = 16, MOVEMENT = 32, PHYSICS = 48, HIT_JUDGE = 64, HIT_RESPONSE = 80, ANIMATION = 96, PRE_RENDER = 112 };

// 全ての更新パーツの基底
class UpdComponent {
private:
	Entity* owner_ = nullptr; // 所有者（Enemyなど）への参照

public:
	virtual ~UpdComponent() = default;

	virtual constexpr UpdCmpGroup UpdGroup() const = 0;

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void Finalize() {}

	virtual void ResolveDependence() {}

	const Entity* GetOwner() const { return owner_; }
	Entity& RefOwner() { return *owner_; }
	void SetOwner(Entity* owner) { owner_ = owner; }
};

} // namespace Atrum