// Copyright 2022-2023 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "../Core/Common.hlsli"

RWTexture2D<float4> resultImage : register(u0);

/**
 * Setup an input image and it's sampler.
 *
 * @param name The image and sampler name. The iamge name will be <name>Image, and sampler will be <name>Sampler.
 * @param binding The image and sampler binding.
 */
#define SETUP_INPUT_IMAGE(name, binding)													\
	[[vk::combinedImageSampler]]															\
	Texture2D XENON_NAME_CONCAT(name, Image) : register(XENON_NAME_CONCAT(t, binding));		\
	[[vk::combinedImageSampler]]															\
	SamplerState XENON_NAME_CONCAT(name, Sampler) : register(XENON_NAME_CONCAT(s, binding))

SETUP_INPUT_IMAGE(positiveXColor, 1);
SETUP_INPUT_IMAGE(negativeXColor, 2);
SETUP_INPUT_IMAGE(positiveYColor, 3);
SETUP_INPUT_IMAGE(negativeYColor, 4);
SETUP_INPUT_IMAGE(positiveZColor, 5);
SETUP_INPUT_IMAGE(negativeZColor, 6);

SETUP_INPUT_IMAGE(positiveXNormal, 7);
SETUP_INPUT_IMAGE(negativeXNormal, 8);
SETUP_INPUT_IMAGE(positiveYNormal, 9);
SETUP_INPUT_IMAGE(negativeYNormal, 10);
SETUP_INPUT_IMAGE(positiveZNormal, 11);
SETUP_INPUT_IMAGE(negativeZNormal, 12);

[numthreads(8, 8, 1)]
void main(uint2 ThreadID : SV_DispatchThreadID)
{
	int2 coordinate = int2(ThreadID.xy);
	resultImage[coordinate] = negativeZColorImage[coordinate];
}