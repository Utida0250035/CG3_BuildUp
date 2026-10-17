#pragma once

#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Atrum::Visual {

	struct Particle {

		/* 空間・移動 */

		// 現在の座標
		Math::Vector3 position{};
		// 現在の速度
		Math::Vector3 velocity{};
		// 加速度
		Math::Vector3 acceleration{};
		// 速度減衰係数
		float damping = 0.0f;

		/* 見た目の補間用 */

		// 発生時の色(RGBA)
		Math::Vector4 startColor{};
		// 消滅時の色(RGBA)
		Math::Vector4 endColor{};
		// 現在の色(RGBA)
		Math::Vector4 currentColor{};

		// 発生時の大きさ
		float startScale = 0.0f;
		// 消滅時の大きさ
		float startScale = 0.0f;
		// 現在の大きさ
		float currentScale = 0.0f;

		/* 寿命データ */

		// 残り寿命
		float lifeTimer = 0.0f;

		// 寿命設定
		float lifeTime = 0.0f;

		/* 状態管理 */

		// 有効フラグ falseなら非アクティブ/プール再利用待ち
		bool isActive = false;

	};

}