// Copyright 2020 Google LLC

#include "Common.hlsli"

XENON_SETUP_ACCELERATION_STRUCTURE(rs);

XENON_SETUP_RENDER_TARGET_IMAGE(float4, image);

XENON_SETUP_CAMERA(UBO, cam);

// Max. number of recursion is passed via a specialization constant
// [[vk::constant_id(0)]] const int MAX_RECURSION = 0;

[shader("raygeneration")]
void main()
{
	uint3 LaunchID = DispatchRaysIndex();
	uint3 LaunchSize = DispatchRaysDimensions();

	const float2 pixelCenter = float2(LaunchID.xy) + float2(0.5, 0.5);
	const float2 inUV = pixelCenter/float2(LaunchSize.xy);
	float2 d = inUV * 2.0 - 1.0;
	float4 target = mul(cam.projInverse, float4(d.x, d.y, 1, 1));

	RayDesc rayDesc;
	rayDesc.Origin = mul(cam.viewInverse, float4(0,0,0,1)).xyz;
	rayDesc.Direction = mul(cam.viewInverse, float4(normalize(target.xyz), 0)).xyz;
	rayDesc.TMin = 0.001;
	rayDesc.TMax = 10000.0;

	float3 color = float3(0.0, 0.0, 0.0);

	for (int i = 0; i < 3 /*MAX_RECURSION*/; i++) {
		RayPayload rayPayload;
		TraceRay(rs, RAY_FLAG_FORCE_OPAQUE, 0xff, 0, 0, 0, rayDesc, rayPayload);
		float3 hitColor = rayPayload.color;

		if (rayPayload.distance < 0.0f) {
			color += hitColor;
			break;
		} else if (rayPayload.reflector == 1.0f) {
			const float3 hitPos = rayDesc.Origin + rayDesc.Direction * rayPayload.distance;
			rayDesc.Origin = hitPos + rayPayload.normal * 0.001f;
			rayDesc.Direction = reflect(rayDesc.Direction, rayPayload.normal);
		} else {
			color += hitColor;
			break;
		}

	}

	image[int2(LaunchID.xy)] = float4(color, 0.0);
}