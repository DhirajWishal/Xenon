// Copyright 2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#ifndef TEXTURE_HLSLI
#define TEXTURE_HLSLI

#define XENON_NAME_CONCAT(first, second)	first##second

#define XENON_SETUP_TEXTURE(type, name, binding)															\
	type XENON_NAME_CONCAT(name, Texture) : register(XENON_NAME_CONCAT(t, binding), space1);				\
	SamplerState XENON_NAME_CONCAT(name, Sampler) : register(XENON_NAME_CONCAT(s, binding), space1);						

#endif // TEXTURE_HLSLI