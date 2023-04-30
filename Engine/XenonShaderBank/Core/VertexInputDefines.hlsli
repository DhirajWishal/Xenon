// Copyright 2022-2023 Nexonous
// SPDX-License-Identifier: Apache-2.0

#ifndef VERTEX_INPUT_DEFINES_HLSLI
#define VERTEX_INPUT_DEFINES_HLSLI

#define XENON_VERTEX_INPUT_VERTEX_POSITION					[[vk::location(0)]]
#define XENON_VERTEX_INPUT_VERTEX_NORMAL					[[vk::location(1)]]
#define XENON_VERTEX_INPUT_VERTEX_TANGENT					[[vk::location(2)]]

#define XENON_VERTEX_INPUT_VERTEX_COLOR_0					[[vk::location(3)]]
#define XENON_VERTEX_INPUT_VERTEX_COLOR_1					[[vk::location(4)]]
#define XENON_VERTEX_INPUT_VERTEX_COLOR_2					[[vk::location(5)]]
#define XENON_VERTEX_INPUT_VERTEX_COLOR_3					[[vk::location(6)]]
#define XENON_VERTEX_INPUT_VERTEX_COLOR_4					[[vk::location(7)]]
#define XENON_VERTEX_INPUT_VERTEX_COLOR_5					[[vk::location(8)]]
#define XENON_VERTEX_INPUT_VERTEX_COLOR_6					[[vk::location(9)]]
#define XENON_VERTEX_INPUT_VERTEX_COLOR_7					[[vk::location(10)]]

#define XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_0		[[vk::location(11)]]
#define XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_1		[[vk::location(12)]]
#define XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_2		[[vk::location(13)]]
#define XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_3		[[vk::location(14)]]
#define XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_4		[[vk::location(15)]]
#define XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_5		[[vk::location(16)]]
#define XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_6		[[vk::location(17)]]
#define XENON_VERTEX_INPUT_VERTEX_TEXTURE_COORDINATE_7		[[vk::location(18)]]

#define XENON_VERTEX_INPUT_VERTEX_JOINT_INDICES_LOCATION	[[vk::location(19)]]
#define XENON_VERTEX_INPUT_VERTEX_JOINT_WEIGHT_LOCATION		[[vk::location(20)]]

#define XENON_VERTEX_INPUT_INSTANCE_POSITION				[[vk::location(21)]]
#define XENON_VERTEX_INPUT_INSTANCE_ROTATION				[[vk::location(22)]]
#define XENON_VERTEX_INPUT_INSTANCE_SCALE					[[vk::location(23)]]
#define XENON_VERTEX_INPUT_INSTANCE_ID						[[vk::location(24)]]

#endif // VERTEX_INPUT_DEFINES_HLSLI