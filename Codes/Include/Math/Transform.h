#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace Atrum::Math {

	struct Transform {
		Vector3 scale{ 1.0f, 1.0f, 1.0f };
		Vector3 rotate{};
		Vector3 translate{};

		/// <summary>
		/// ワールド行列の作成
		/// </summary>
		/// <param name="transform"> Transform </param>
		/// <returns> ワールド行列 </returns>
		[[nodiscard]] Matrix4x4 MakeWorldMatrix() const {

			return Matrix4x4::World(translate, scale, rotate);

		}

	};

}