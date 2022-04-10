#include "Macros.hlsl"

inline float Sin01(half value)
{
	return sin(value) * 0.5 + 0.5;
}

inline float Cos01(half value)
{
	return cos(value) * 0.5 + 0.5;
}

inline float4x4 Inverse(float4x4 m)
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

inline half4x4 Identity()
{
    return half4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );
}

inline float4x4 Tranlsation(float3 value)
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        value.x, value.y, value.z, 1
        );
}

inline half4x4 RotationX(half rad)
{
    half c = cos(rad);
    half s = sin(rad);
    return half4x4(
        1, 0, 0, 0,
        0, c, s, 0,
        0, -s, c, 0,
        0, 0, 0, 1
        );
}

inline half4x4 RotationY(half rad)
{
    half c = cos(rad);
    half s = sin(rad);
    return half4x4(
        c, 0, -s, 0,
        0, 1, 0, 0,
        s, 0, c, 0,
        0, 0, 0, 1
        );
}

half4x4 RotationZ(half rad)
{
    half c = cos(rad);
    half s = sin(rad);
    return half4x4(
        c, s, 0, 0,
        -s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );
}

inline half4x4 RotationAxis(half3 axis, half rad)
{
    half x = axis.x;
    half y = axis.y;
    half z = axis.z;
    half c = cos(rad);
    half s = sin(rad);

    half ic = 1.0f - c;

    half4x4 m = 0;

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

inline float4x4 Scale(float3 value)
{
    return float4x4(
        value.x, 0, 0, 0,
        0, value.y, 0, 0,
        0, 0, value.z, 0,
        0, 0, 0, 1
        );
}

inline float3 DecomposeTranslation(float4x4 value)
{
    return float3(value[3][0], value[3][1], value[3][2]);
}

inline float3 DecomposeScale(float4x4 value)
{
    float xs = length(float3(value[0][0], value[0][1], value[0][2]));
    float ys = length(float3(value[1][0], value[1][1], value[1][2]));
    float zs = length(float3(value[2][0], value[2][1], value[2][2]));
    return float3(xs, ys, zs);
}

inline half3 UnpackNormalMap(half3 normalMap, half3 normal, half3 tangent, half3 binormal)
{
    //half3 unpackedNormalMap = (normalMap.x * tangent) + (normalMap.y * binormal) + (normalMap.z * normal);

    //half3 unpackedNormalMap = (normalMap.x * binormal) + (normalMap.y * tangent) + (normalMap.z * normal);
    //unpackedNormalMap = normalize(unpackedNormalMap);
    //return unpackedNormalMap;

    half3 nm = normalMap * 2.0f - 1.0f;
    half3x3 TBN = half3x3(tangent, binormal, normal);
    //TBN = transpose(TBN);
    return normalize(mul(nm, TBN));
}

inline half2 UVToTexPixelCoord(half width, half height, half2 uv)
{
    int iU = int(width * uv.x);
    int iH = int(height * uv.y);
    return half2(iU, iH);
}

inline half2 TexPixelCoordToUV(half width, half height, half2 pixelCoord)
{
    return half2(pixelCoord.x / width, pixelCoord.y / height);
}

inline half ToLinearDepth(half Near, half Far, half Depth)
{
    return 2.0f * Near / (Far + Near - Depth * (Far - Near));
}

inline half Brightness(half3 colorRGB)
{
    half a = max(colorRGB.r, colorRGB.g);
    half b = max(a, colorRGB.b);
    return b;
}

inline half Brightness(half4 colorRGBA)
{
    return Brightness(colorRGBA.rgb) * colorRGBA.a;
}

inline half Random(half2 xy)
{
    float2 noise = (frac(sin(dot(xy, half2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f));
    return frac(abs(noise.x + noise.y) * 0.5);
}

inline half3 RandomVector(half2 xy)
{
    half x0 = frac(sin(dot(xy, half2(15.8989f, 76.132f) * 1.0f)) * 46336.23745f);
    half y0 = frac(sin(dot(xy, half2(11.9899f, 62.223f) * 2.0f)) * 34748.34744f);
    half z0 = frac(sin(dot(xy, half2(13.3238f, 63.122f) * 3.0f)) * 59998.47362f);

    half x1 = (x0 + y0) * 0.5;
    half y1 = (y0 + z0) * 0.5;
    half z1 = (z0 + x0) * 0.5;

    return normalize(half3(x1, y1, z1));
}

// Screen -> NDC
inline half4 ToNDC(half2 uv, half depth)
{
    half4 NDC;
    NDC.xy = uv * 2.0f - 1.0f;
    NDC.y *= -1;
    NDC.z = depth;
    NDC.w = 1.0f;
    return NDC;
}

// Screen -> View
inline half3 ToViewSpace(half2 uv, half depth, half4x4 inverseProjectionMatrix)
{
    half4 NDC;
    NDC.xy = uv * 2.0f - 1.0f;
    NDC.y *= -1;
    NDC.z = depth;
    NDC.w = 1.0f;
    half4 view = mul(NDC, inverseProjectionMatrix);
    return view.xyz / view.w;
}

// NDC -> UV
inline half2 ToUV(half2 ndc_xy)
{
    return ndc_xy * half2(1.0f, -1.0f) * 0.5f + 0.5f;
}

inline float Saturate(float f)
{
    return saturate(f.x);
}

inline float2 Saturate(float2 f)
{
    return float2(saturate(f.x), saturate(f.y));
}

inline float3 Saturate(float3 f)
{

    return float3(saturate(f.x), saturate(f.y), saturate(f.y));
}

inline float4 Saturate(float4 f)
{
    return float4(saturate(f.x), saturate(f.y), saturate(f.y), saturate(f.y));
}

inline bool IsSaturated(float f)
{
    float v = Saturate(f);
    return v == f;
}

inline bool IsSaturated(float2 f)
{
    float2 v = Saturate(f);
    return f.x == v.x && f.y == v.y;
}

inline bool IsSaturated(float3 f)
{
    float3 v = Saturate(f);
    return f.x == v.x && f.y == v.y && f.z == v.z;
}

inline bool IsSaturated(float4 f)
{
    float4 v = Saturate(f);
    return f.x == v.x && f.y == v.y && f.z == v.z && f.w == v.w;
}

inline float4x4 NormalizeTransformationMatrix(float4x4 m)
{
    m[0].xyz = normalize(m[0].xyz);
    m[1].xyz = normalize(m[1].xyz);
    m[2].xyz = normalize(m[2].xyz);
    return m;
}

inline half4x4 NormalizeTransformationMatrix(half4x4 m)
{
    m[0].xyz = normalize(m[0].xyz);
    m[1].xyz = normalize(m[1].xyz);
    m[2].xyz = normalize(m[2].xyz);
    return m;
}