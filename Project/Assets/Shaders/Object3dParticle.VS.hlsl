#include "Object3d.hlsli"

// フレームごとのカメラ情報（パーティクル VS のみ使用）
cbuffer ParticleFrame : register(b2) {
	float32_t4x4 gViewProjection;
	float32_t4x4 gView;
	float4 gCameraWorld;
	uint gInstanceBufferBase;
	uint3 gParticleFramePaddingUi;
	float4 gParticleFramePadding[6];
};

struct ParticleInstance {
	float32_t4x4 world;
	float32_t4 color;
	float radius;
	float32_t3 padding3;
};

StructuredBuffer<ParticleInstance> gParticleInstances : register(t1);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texCoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
	ParticleInstance inst = gParticleInstances[gInstanceBufferBase + instanceId];

	VertexShaderOutput output;

	float32_t4 localPos = float32_t4(input.position.xyz * inst.radius, input.position.w);
	float32_t4 worldPos = mul(localPos, inst.world);

	output.position = mul(worldPos, gViewProjection);

	output.texCoord = input.texCoord;

	float3x3 worldRot = (float3x3)inst.world;
	output.normal = normalize(mul(input.normal, worldRot));

	output.worldPosition = worldPos.xyz;

	output.instanceColor = inst.color;

	return output;
}
