#include "Object3d.hlsli"

static const uint kLightModelLambert = 0;
static const uint kLightModelHalfLambert = 1;

struct DirectionalLight {
	float32_t4 color;
	float32_t3 direction;
	float intensity;
	uint lightingMode;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Material {
	float32_t4 color;
	int32_t isLightingEnable;
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	uint instanceID = InstanceID();
	
	float32_t4 textureColor = gTexture.SampleLevel(gSampler, input.texCoord, 0);
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