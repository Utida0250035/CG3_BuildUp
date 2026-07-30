#pragma once
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"
#include "Math/Transform.h"

namespace Atrum::Physics {

	class Handed {

	public:

		// LH (Game) -> RH (Physics) への変換
		static constexpr Math::Vector3 ToPhysicsTranslate(const Math::Vector3& translate) {
			return { translate.x, translate.y, -translate.z }; // Z反転
		}

		// RH (Physics) -> LH (Game) への変換
		static constexpr Math::Vector3 FromPhysicsTranslate(const Math::Vector3& translate) {
			return { translate.x, translate.y, -translate.z }; // Z反転
		}

		// クォータニオンの変換 (回転軸の反転・角度の調整)
		static constexpr Math::Quaternion ToPhysicsRotation(const Math::Quaternion& quaternion) {
			// 右手系に合わせるための鏡映変換
			return { quaternion.w, quaternion.x, quaternion.y, -quaternion.z };
		}

		static constexpr Math::Quaternion FromPhysicsRotation(const Math::Quaternion& quaternion) {
			return { quaternion.w, quaternion.x, quaternion.y, -quaternion.z };
		}

		static constexpr Math::TransformRH ToPhysics(const Math::TransformLH& transform) {

			return Math::TransformRH{
				.scale = transform.scale,
				.quaternion = ToPhysicsRotation(transform.quaternion),
				.translate = ToPhysicsTranslate(transform.translate)
			};

		}

		static constexpr Math::TransformLH FromPhysics(const Math::TransformRH& transform) {

			return Math::TransformLH{
				.scale = transform.scale,
				.quaternion = FromPhysicsRotation(transform.quaternion),
				.translate = FromPhysicsTranslate(transform.translate)
			};

		}

	};

}