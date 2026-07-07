#include "Object3d.hlsli"

static const uint kLightModelLambert = 0;
static const uint kLightModelHalfLambert = 1;

struct DirectionalLight {
	// 16 * n[B]
	
	// 1
	float32_t4 color;
	// 1
	float32_t3 direction;
	float intensity;
	// 1
	uint lightingMode;
	float3 padding0;
	
	// 13
	float4 padding1[13];
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Material {
	// 16 * n[B]
	
	// 1
	float32_t4 color;
	// 4
	float32_t4x4 uvTransform;
	// 1
	int32_t isLightingEnable;
	float3 padding0;
	// 10
	float4 padding1[10];
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	float2 uvOffset = input.texCoord.xy - 0.5f;
	
	float2 transformedUV = mul(uvOffset, (float2x2)gMaterial.uvTransform);
	transformedUV += 0.5f + gMaterial.uvTransform._41_42;
	float32_t4 textureColor = gTexture.SampleLevel(gSampler, transformedUV, 0);
	float nDotL = dot(normalize(input.normal), -gDirectionalLight.direction);
	float cos = 0.0f;
	
	if (gMaterial.isLightingEnable) {
	
		switch (gDirectionalLight.lightingMode) {
		
			case kLightModelLambert:
				cos = saturate(nDotL);
				output.color = gMaterial.color * textureColor * cos * gDirectionalLight.intensity;
				break;
		
			case kLightModelHalfLambert:
				cos = pow(nDotL * 0.5f + 0.5f, 2.0f);
				output.color = gMaterial.color * textureColor * cos * gDirectionalLight.intensity;
				break;
		
			default:
				output.color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);
				break;
		
		}
		
	} else {
		
		output.color = gMaterial.color * textureColor;
		
	}
	
	return output;

}