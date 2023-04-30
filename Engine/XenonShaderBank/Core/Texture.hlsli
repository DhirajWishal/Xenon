// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#ifndef TEXTURE_HLSLI
#define TEXTURE_HLSLI

#include "Common.hlsli"

#define XENON_SETUP_TEXTURE(type, name, bindingIndex)																										\
	[[vk::combinedImageSampler]]																															\
	XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_MATERIAL, bindingIndex)																					\
	type XENON_NAME_CONCAT(name, Texture) : register(XENON_NAME_CONCAT(t, bindingIndex), XENON_DESCRIPTOR_SPACE(XENON_DESCRIPTOR_TYPE_MATERIAL));			\
																																							\
	[[vk::combinedImageSampler]]																															\
	XENON_SETUP_DESCRIPTOR(XENON_DESCRIPTOR_TYPE_MATERIAL, bindingIndex)																					\
	SamplerState XENON_NAME_CONCAT(name, Sampler) : register(XENON_NAME_CONCAT(s, bindingIndex), XENON_DESCRIPTOR_SPACE(XENON_DESCRIPTOR_TYPE_MATERIAL));						

#endif // TEXTURE_HLSLI