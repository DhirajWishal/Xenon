Texture2D hdri : register(t0);
RWTexture3D<float4> cubemap : register(u1);

#define PI 3.141

float3 GetCubemapDirectionFromIndex(uint faceIndex, uint2 coord)
{
	uint width, height, depth;
	cubemap.GetDimensions(width, height, depth);

    float3 direction;
    switch (faceIndex)
    {
        case 0: // Positive X
            direction = float3(1, -coord.y / height * 2 + 1, coord.x / width * 2 - 1);
            break;
        case 1: // Negative X
            direction = float3(-1, -coord.y / height * 2 + 1, -coord.x / width * 2 + 1);
            break;
        case 2: // Positive Y
            direction = float3(coord.x / width * 2 - 1, 1, coord.y / height * 2 - 1);
            break;
        case 3: // Negative Y
            direction = float3(coord.x / width * 2 - 1, -1, -coord.y / height * 2 + 1);
            break;
        case 4: // Positive Z
            direction = float3(coord.x / width * 2 - 1, -coord.y / height * 2 + 1, 1);
            break;
        case 5: // Negative Z
            direction = float3(-coord.x / width * 2 + 1, -coord.y / height * 2 + 1, -1);
            break;
    }

    return normalize(direction);
}

float2 GetEquirectangularUV(float3 direction)
{
    float phi = atan2(direction.z, direction.x);
    float theta = acos(direction.y);
    return float2(phi / (2 * PI) + 0.5, theta / PI);
}

[numthreads(32, 32, 6)]
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    uint faceIndex = DispatchThreadID.z;
    uint2 faceCoord = DispatchThreadID.xy;

    float3 direction = GetCubemapDirectionFromIndex(faceIndex, faceCoord);
    float2 uv = GetEquirectangularUV(direction);

    cubemap[uint3(faceCoord, faceIndex)] = hdri[uv];
}