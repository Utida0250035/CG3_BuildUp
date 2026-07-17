#pragma once

namespace Atrum {

	class Entity;

	// 全てのパーツの基底
	class Component {

	protected:

		Entity* owner_ = nullptr; // 所有者（Enemyなど）への参照

	public:

		virtual ~Component() = default;

		virtual void Initialize() {}

		virtual void Execute() = 0;

		void SetOwner(Entity* owner) { owner_ = owner; }

	};

}