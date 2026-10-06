#include "Object3d.hlsli"

struct TransformationMatrix {
	// 16 * n[B]
	
	// 4
	float32_t4x4 WVP;
	// 4
	float32_t4x4 world;
	
	// 8
	float4 padding[8];
};

ConstantBuffer<TransformationMatrix> gTransformMatrix : register(b0);

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t2 texCoord : TEXCOORD0;
	float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input) {
	
	VertexShaderOutput output;
	
	output.position = mul(input.position, gTransformMatrix.WVP);
	
	output.texCoord = input.texCoord;
	
	output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformMatrix.world));
	
	return output;
}