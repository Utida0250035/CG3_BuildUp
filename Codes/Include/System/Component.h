#pragma once

namespace Atrum {

	class Entity;

	// 全てのパーツの基底
	class Component {

	private:

		Entity* owner_ = nullptr; // 所有者（Enemyなど）への参照

	public:

		virtual ~Component() = default;

		virtual void Initialize() {}

		virtual void Execute() {};

		virtual void Finalize() {};

	protected:

		const Entity* GetOwner() const { return owner_; }

	public:

		void SetOwner(Entity* owner) { owner_ = owner; }

	};

}