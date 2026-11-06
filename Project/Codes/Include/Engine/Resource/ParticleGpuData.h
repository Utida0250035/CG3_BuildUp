#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector4.h"
#include <cstdint>

namespace Atrum {

// GPU 定数バッファ用（シェーダの cbuffer ParticleFrame とレイアウト一致、256 バイト）
struct ParticleFrameGpuData {
	Math::Matrix4x4 viewProjection{};
	Math::Matrix4x4 view{};
	Math::Vector4 cameraWorld{}; // xyz: カメラ位置, w は未使用
	uint32_t instanceBufferBase = 0;
	uint32_t particleFramePaddingUi[3]{};
	float padding[24]{}; // 合計 256 バイトになるよう詰める
};

static_assert(sizeof(ParticleFrameGpuData) == 256);

// StructuredBuffer 用（シェーダの ParticleInstance と要素サイズ一致）
struct ParticleInstanceGpuData {
	Math::Matrix4x4 world{};
	Math::Vector4 color{};
	float radius = 1.0f;
	float padding3[3]{};
};

static_assert(sizeof(ParticleInstanceGpuData) == 96);
static_assert(sizeof(ParticleInstanceGpuData) % 16 == 0);

} // namespace Atrum
