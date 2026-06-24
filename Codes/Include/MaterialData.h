#pragma once
#include "Matrix3D.h"
#include "Vector4.h"
#include <cstdint>

struct MaterialData {
	Vector4 color{};
	Matrix4x4 uvTransformMatrix{};
	int32_t inLightingEnable = false;
	// ConstantBuffer用の詰め物
	float padding[43]{};
};