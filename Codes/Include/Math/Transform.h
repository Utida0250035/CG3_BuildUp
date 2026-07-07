#pragma once

#include "Matrix3D.h"
#include "Vector3.h"

struct Transform {
	Vector3 scale{ 1.0f, 1.0f, 1.0f };
	Vector3 rotate{};
	Vector3 translate{};
};

/// <summary>
/// ワールド行列の作成
/// </summary>
/// <param name="transform"> Transform </param>
/// <returns> ワールド行列 </returns>
inline Matrix4x4 MakeWorldMatrix(const Transform& transform) {

	return MakeWorldMatrix(transform.translate, transform.scale, transform.rotate);

}