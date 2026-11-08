#include "Object3d.hlsli"

static const uint kLightModelLambert = 0;
static const uint kLightModelHalfLambert = 1;

struct DirectionalLight {
	// 16 * n[B]
	
	// 1
	float3 color;
	float padding0;
	
	// 1
	float32_t3 direction;
	float intensity;
	// 1
	uint lightingMode;
	float3 padding1;

	float3 rimColor;
	float rimIntensity;
	float rimPower;
	uint rimEnabled;
	float rimPadding0;
	float3 cameraWorldPosition;
	float cameraPadding0;

	float3 fogColor;
	float fogDensity;
	float fogHeightFalloff;
	float fogHeightOffset;
	float fogStartDistance;
	float fogMaxDistance;
	float fogInScatterIntensity;
	uint fogEnabled;
	float fogPadding0;
	
	// 7
	float4 padding2[7];
};

float3 ApplyVolumetricFog(float3 shadedRgb, float3 worldPos, float3 cameraPos, float3 lightDir, DirectionalLight light) {
	if (light.fogEnabled == 0u || light.fogDensity <= 0.0f) {
		return shadedRgb;
	}

	const float3 toPoint = worldPos - cameraPos;
	const float dist = length(toPoint);
	const float3 viewDir = toPoint / max(dist, 1.0e-4f);
	const float distRange = max(light.fogMaxDistance - light.fogStartDistance, 0.001f);
	const float distT = saturate((dist - light.fogStartDistance) / distRange);
	const float heightFactor = exp(-light.fogHeightFalloff * (worldPos.y - light.fogHeightOffset));
	const float opticalDepth = light.fogDensity * dist * distT * heightFactor;
	const float fogBlend = 1.0f - exp(-opticalDepth);
	const float scatter = light.fogInScatterIntensity *
		saturate(dot(normalize(-lightDir), viewDir)) * fogBlend;
	const float3 fogged = lerp(shadedRgb, light.fogColor, fogBlend);
	return fogged + light.fogColor * scatter;
}

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Material {
	// 16 * n[B]
	
	// 1
	float32_t4 color;
	
	// 4
	float32_t4x4 uvTransform;
	
	// 1
	int32_t isLightingEnable;
	int32_t isUseTexture;
	float2 padding1;
	
	// 10
	float4 padding2[10];
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);

SamplerState gSampler : register(s0);

#ifdef USE_DITHER_CLIP

float GetBayer4x4Threshold(uint2 screenPosition) {
	const uint x = screenPosition.x & 3u;
	const uint y = screenPosition.y & 3u;
	static const uint kMatrix[16] = {
		0u, 8u, 2u, 10u,
		12u, 4u, 14u, 6u,
		3u, 11u, 1u, 9u,
		15u, 7u, 13u, 5u,
	};
	return (float(kMatrix[y * 4u + x]) + 0.5f) / 16.0f;
}

#endif

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	
	float2 uvOffset = input.texCoord.xy - 0.5f;
	
	float32_t4 textureColor = float32_t4(1.0f, 1.0f, 1.0f, 1.0f);
	
	if (gMaterial.isUseTexture) {
	
		float2 transformedUV = mul(uvOffset, (float2x2) gMaterial.uvTransform);
		transformedUV += 0.5f + gMaterial.uvTransform._41_42;
		textureColor = gTexture.SampleLevel(gSampler, transformedUV, 0);
		
	}
	
#ifdef USE_CLIP

	clip(textureColor.a - 0.5f);

#endif

	float nDotL = dot(normalize(input.normal), -gDirectionalLight.direction);
	float cos = 0.0f;
	
	float3 materialRgb = gMaterial.color.rgb * input.instanceColor.rgb;
	float materialAlpha = gMaterial.color.a * input.instanceColor.a;

	if (gMaterial.isLightingEnable) {
	
		switch (gDirectionalLight.lightingMode) {
		
			case kLightModelLambert:
				cos = saturate(nDotL);
				output.color.rgb = materialRgb * textureColor.rgb * cos * gDirectionalLight.intensity * gDirectionalLight.color.rgb;
				output.color.a = materialAlpha * textureColor.a;

				break;
		
			case kLightModelHalfLambert:
				cos = pow(nDotL * 0.5f + 0.5f, 2.0f);
				output.color.rgb = materialRgb * textureColor.rgb * cos * gDirectionalLight.intensity * gDirectionalLight.color.rgb;
				output.color.a = materialAlpha * textureColor.a;

				break;
		
			default:
				output.color = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);
				break;
		
		}

		if (gDirectionalLight.rimEnabled != 0u && gDirectionalLight.rimIntensity > 0.0f) {
			const float3 normalW = normalize(input.normal);
			const float3 viewDir = normalize(gDirectionalLight.cameraWorldPosition - input.worldPosition);
			const float rimDot = 1.0f - saturate(dot(normalW, viewDir));
			const float rim = pow(rimDot, max(gDirectionalLight.rimPower, 0.001f)) *
				gDirectionalLight.rimIntensity;
			output.color.rgb += gDirectionalLight.rimColor * rim;
		}
		
	} else {
		
		output.color = float32_t4(materialRgb, materialAlpha) * textureColor;
		
	}

	output.color.rgb = ApplyVolumetricFog(
		output.color.rgb,
		input.worldPosition,
		gDirectionalLight.cameraWorldPosition,
		gDirectionalLight.direction,
		gDirectionalLight);

#ifdef USE_DITHER_CLIP
	{
		const float coverage = output.color.a;
		clip(coverage - GetBayer4x4Threshold(uint2(input.position.xy)));
		output.color.a = 1.0f;
	}
#endif
	
	return output;

}