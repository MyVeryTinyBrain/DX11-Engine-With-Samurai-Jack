#include "Macros.hlsl"

float2 UVToPixelCoord(float width, float height, float2 uv)
{
	int iU = int(width * uv.x);
	int iH = int(height * uv.y);
	return float2(iU, iH);
}

float2 PixelCoordToUV(float width, float height, float2 pixelCoord)
{
	return float2(pixelCoord.x / width, pixelCoord.y / height);
}

float Sin01(float value)
{
	return sin(value) * 0.5 + 0.5;
}

float Cos01(float value)
{
	return cos(value) * 0.5 + 0.5;
}

float4x4 Inverse(float4x4 m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

float4x4 Identity()
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );
}

float4x4 Tranlsation(float3 value)
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        value.x, value.y, value.z, 1
        );
}

float4x4 RotationX(float rad)
{
    float c = cos(rad);
    float s = sin(rad);
    return float4x4(
        1, 0, 0, 0,
        0, c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1
        );
}

float4x4 RotationY(float rad)
{
    float c = cos(rad);
    float s = sin(rad);
    return float4x4(
        c, 0, -s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1
        );
}

float4x4 RotationZ(float rad)
{
    float c = cos(rad);
    float s = sin(rad);
    return float4x4(
        c, s, 0, 0,
        -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );
}

float4x4 RotationAxis(float3 axis, float rad)
{
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float c = cos(rad);
    float s = sin(rad);

    float ic = 1.0f - c;

    float4x4 m = 0;

    m[0][0] = c + x * x * ic;
    m[0][1] = x * y * ic - z * s;
    m[0][2] = x * z * ic + y * s;
    m[1][0] = y * x * ic + z * s;
    m[1][1] = c + y * y * ic;
    m[1][2] = y * z * ic - x * s;
    m[2][0] = z * x * ic - y * s;
    m[2][1] = z * y * ic + x * s;
    m[2][2] = c + z * z * ic;

    m[0][3] = m[1][3] = m[2][3] = 0;
    m[3][3] = 1;

    return transpose(m);
}

float4x4 Scale(float3 value)
{
    return float4x4(
        value.x, 0, 0, 0,
        0, value.y, 0, 0,
        0, 0, value.z, 0,
        0, 0, 0, 1
        );
}

float3 DecomposeTranslation(float4x4 value)
{
    return float3(value[3][0], value[3][1], value[3][2]);
}

float3 DecomposeScale(float4x4 value)
{
    float xs = length(float3(value[0][0], value[0][1], value[0][2]));
    float ys = length(float3(value[1][0], value[1][1], value[1][2]));
    float zs = length(float3(value[2][0], value[2][1], value[2][2]));
    return float3(xs, ys, zs);
}

float3 UnpackNormalMap(float3 normalMap, float3 normal, float3 tangent, float3 binormal)
{
    float3 unpackedNormalMap = (normalMap.x * tangent) + (normalMap.y * binormal) + (normalMap.z * normal);
    unpackedNormalMap = normalize(unpackedNormalMap);
    return unpackedNormalMap;
}

float4 PackNormal(float3 normal)
{
    normal = normalize(normal);
    float3 normalizedNormal = normal * 0.5f + 0.5f;
    return float4(normalizedNormal, 1.0f);
}

float3 UnpackNormal(float4 packedNormal)
{
    return packedNormal.xyz * 2.0f - 1.0f;
}

float4 PackWorldPosition(float3 worldPosition)
{
    float3 normalizedWorldPosition = normalize(worldPosition) * 0.5f + 0.5f;
    float worldPositionLength = length(worldPosition) / WORLD_POSITION_PACK_SCALE;
    return float4(normalizedWorldPosition, worldPositionLength);
}

float3 UnpackWorldPosition(float4 packedWorldPosition)
{
    float3 unpackedWorldPosition = packedWorldPosition.xyz * 2.0f - 1.0f;
    unpackedWorldPosition *= packedWorldPosition.w * WORLD_POSITION_PACK_SCALE;
    return unpackedWorldPosition;
}

float ToLinearDepth(float Near, float Far, float Depth)
{
    return 2.0f * Near / (Far + Near - Depth * (Far - Near));
}

float Brightness(float3 colorRGB)
{
    float a = max(colorRGB.r, colorRGB.g);
    float b = max(a, colorRGB.b);
    return b;
}

float Brightness(float4 colorRGBA)
{
    return Brightness(colorRGBA.rgb) * colorRGBA.a;
}

float Random(float2 xy)
{
    float2 noise = (frac(sin(dot(xy, float2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f));
    return frac(abs(noise.x + noise.y) * 0.5);
}

float3 RandomVector(float2 xy)
{
    float x0 = frac(sin(dot(xy, float2(15.8989f, 76.132f) * 1.0f)) * 46336.23745f);
    float y0 = frac(sin(dot(xy, float2(11.9899f, 62.223f) * 2.0f)) * 34748.34744f);
    float z0 = frac(sin(dot(xy, float2(13.3238f, 63.122f) * 3.0f)) * 59998.47362f);

    float x1 = (x0 + y0) * 0.5;
    float y1 = (y0 + z0) * 0.5;
    float z1 = (z0 + x0) * 0.5;

    return normalize(float3(x1, y1, z1));
}

// Screen -> NDC
float4 ToNDC(float2 uv, float depth)
{
    float4 NDC;
    NDC.xy = uv * 2.0f - 1.0f;
    NDC.y *= -1;
    NDC.z = depth;
    NDC.w = 1.0f;
    return NDC;
}

// Screen -> View
float3 ToViewSpace(float2 uv, float depth, float4x4 inverseProjectionMatrix)
{
    float4 NDC;
    NDC.xy = uv * 2.0f - 1.0f;
    NDC.y *= -1;
    NDC.z = depth;
    NDC.w = 1.0f;
    float4 view = mul(NDC, inverseProjectionMatrix);
    return view.xyz / view.w;
}
