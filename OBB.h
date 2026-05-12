#pragma once

#include "Vector2.h"

typedef struct OrientedBoundingBox {
	// 中心
	Vector2 center;
	// 各軸の長さ(幅, 高さ)
	Vector2 size;
	// 各軸の長さの半分
	Vector2 halfSize;
	// 正規化された方向ベクトル(ローカル右、ローカル上)
	Vector2 axis[2];
} OBB;

/// <summary>
/// ワールド座標をOBBローカル座標に変換
/// </summary>
/// <param name="worldPos"> ワールド座標 </param>
/// <param name="obb"> OBB </param>
/// <returns></returns>
inline constexpr Vector2 ToLocal(const Vector2& worldPos, const OBB& obb) {

	Vector2 relation = worldPos - obb.center;
	return { VectorDot(relation, obb.axis[0]), VectorDot(relation, obb.axis[1]) };

}