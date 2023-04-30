// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Texture.hlsli"
#include "SceneCommon.hlsli"

XENON_SETUP_TEXTURE(Texture2D, baseColor, 0)
XENON_SETUP_TEXTURE(Texture2D, shadowMap, 1)

#define ambient 0.1

float textureProj(float4 shadowCoord, float2 off)
{
	float shadow = 1.0;
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 )
	{
		float dist = shadowMapTexture.Sample( shadowMapSampler, shadowCoord.xy + off ).r;
		if ( shadowCoord.w > 0.0 && dist < shadowCoord.z )
		{
			shadow = ambient;
		}
	}
	return shadow;
}

float filterPCF(float4 sc)
{
	int2 texDim;
	shadowMapTexture.GetDimensions(texDim.x, texDim.y);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);

	float shadowFactor = 0.0;
	int count = 0;
	int range = 1;

	for (int x = -range; x <= range; x++)
	{
		for (int y = -range; y <= range; y++)
		{
			shadowFactor += textureProj(sc, float2(dx*x, dy*y));
			count++;
		}

	}
	return shadowFactor / count;
}

float4 main(VSOutput input) : SV_TARGET
{
	float shadow = filterPCF(input.shadowCoordinate / input.shadowCoordinate.w);

	float3 N = normalize(input.normal);
	float3 L = normalize(input.lightVector);
	float3 V = normalize(input.viewVector);
	float3 R = normalize(-reflect(L, N));
	float3 diffuse = max(dot(N, L), ambient) * (baseColorTexture.Sample(baseColorSampler, input.textureCoordinates) * input.lightColor);

	return float4(diffuse * shadow, 1.0);
}