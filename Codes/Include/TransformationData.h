#pragma once
#include "Matrix3D.h"

struct TransformationData {
	Matrix4x4 wvp{};
	Matrix4x4 world{};

	// 4 * 16 + 4 * 16 = 128
	// (256 - 128) / 4
	// ConstantBuffer用の詰め物
	float padding[32]{};
};