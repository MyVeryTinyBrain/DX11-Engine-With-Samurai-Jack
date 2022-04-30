#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float2 UV : TEXCOORD;
};

#define FOG_TYPE_Distance	0
#define FOG_TYPE_Z			1

#define BLOOM_TYPE_ADD	0
#define BLOOM_TYPE_MIX	1

#define BLUR_TYPE_DEFAULT		0
#define BLUR_TYPE_INVDEPTH		1
#define BLUR_TYPE_DEPTH			2

struct SSAODesc
{
	bool	Enable;
	uint	NumSamples;
	uint	BlurNumSamples;
	float	Transparency;
	float	MinZ;
	float	Radius;
	float	Power;
	float	BlurPixelDistance;
};

struct SSRDesc
{
	bool	Enable;
	bool	BlurEnable;
	uint	BlurType;
	uint	NumSamples;
	uint	BlurNumSamples;
	float	Step;
	float	Thickness;
	float	Bias;
	float	BlurPixelDistance;
};

struct DOFDesc
{
	bool	Enable;
	uint	BlurNumSamples;
	float	MinZ;
	float	RangeZ;
	float	Power;
	float	BlurPixelDistance;
};

struct FogDesc
{
	bool	Enable;
	uint	Type;
	float	MinZ;
	float	RangeZ;
	float4	Color;
};

struct BloomDesc
{
	bool	Enable;
	uint	Type;
	uint	BlurNumSamples;
	float	Intensity;
	float	Threshold;
	float	BlurPixelDistance;
};

struct ChromaticAberrationDesc
{
	bool	Enable;
	float4	Blend;	
	float4	Offset;
	float4	Angle;	
};

struct BlurDesc
{
	bool	DepthBlur;
	uint	NumSamples;
	float	PixelDistance;
};

SSAODesc				_SSAODesc;
FogDesc					_FogDesc;
BloomDesc				_BloomDesc;
SSRDesc					_SSRDesc;
DOFDesc					_DOFDesc;
BlurDesc				_BlurDesc;
ChromaticAberrationDesc _ChromaticAberrationDesc;
float4					_TextureSize;
texture2D				_Albedo;
texture2D				_Normal;
texture2D				_Depth;
texture2D				_Light_Shadow;
texture2D				_Roughness_Metallic;
texture2D				_Emissive;
texture2D				_Occlusion_Reflection_ReflectionBlur_ReflectMask;
texture2D				_Result;
texture2D				_Sample; // For Temp Texture
texture2D				_Other; // For Temp Texture
SamplerState			linearSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
	ComparisonFunc = Never;
	MinLOD = 0;
	MaxLOD = FLT_MAX;
	Filter = MIN_MAG_MIP_LINEAR;
};
SamplerState			pointSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
	ComparisonFunc = Never;
	MinLOD = 0;
	MaxLOD = FLT_MAX;
	Filter = MIN_MAG_MIP_POINT;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	output.Screen = float4(In.Position, 1.0f);
	output.UV = In.UV;

	return output;
}

// SSAO =======================================================================================================

half SSAORayMarch(float2 uv)
{
	const static int NUM_KERNEL = 16;
	const static half3 kernels[NUM_KERNEL] =
	{
		normalize(half3(0.2024537f, 0.841204f, 0.9060141f)),	
		normalize(half3(0.4839356f, -0.8253108f, 0.1563844f)),	
		normalize(half3(-0.7578573f, -0.5583301f, 0.2347527f)),
		normalize(half3(-0.4192392f, 0.2084218f, 0.3672943f)),	
		normalize(half3(-0.8433938f, 0.1451271f, 0.2202872f)),	
		normalize(half3(0.0019193f, -0.8048455f, 0.0726584f)),	
		normalize(half3(0.7867433f, -0.141479f, 0.1567597f)),	
		normalize(half3(0.3677659f, 0.1086345f, 0.4466777f)),	
		normalize(half3(-0.4540417f, -0.252365f, 0.0694318f)),	
		normalize(half3(-0.4037157f, -0.8263387f, 0.4698132f)),
		normalize(half3(-0.6657394f, 0.6298575f, 0.6342437f)),	
		normalize(half3(-0.2200423f, 0.6282339f, 0.8275437f)),	
		normalize(half3(0.8775856f, 0.4617546f, 0.6427765f)),	
		normalize(half3(-0.0001783f, 0.2834622f, 0.8343929f)),	
		normalize(half3(-0.0483353f, -0.2527294f, 0.5924745f)),
		normalize(half3(0.4401554f, -0.4228428f, 0.3300118f)),	
	};

	float4x4 invProjMatrix = Inverse(_ProjectionMatrix);

	half3 normal = UnpackNormal(_Normal, pointSampler, uv);

	half depth = _Depth.Sample(pointSampler, uv, 0).r;
	half occlusionMask = _Occlusion_Reflection_ReflectionBlur_ReflectMask.Sample(pointSampler, uv, 0).r;

	[flatten] // Fast exit
	if (depth == 1.0f) return 1.0f;

	half3 vPosition = ToViewSpace(uv, depth, invProjMatrix);
	float3 worldPosition = mul(float4(vPosition, 1.0f), invProjMatrix).xyz;

	half3 fracWorldPosition;
	fracWorldPosition.x = frac(worldPosition.x * 100.0f);
	fracWorldPosition.y = frac(worldPosition.y * 100.0f);
	fracWorldPosition.z = frac(worldPosition.z * 100.0f);
	half fracWPSum = fracWorldPosition.x + fracWorldPosition.y + fracWorldPosition.z;
	half3 randomSeed = 0;
	randomSeed.x = dot(fracWorldPosition.xy, fracWorldPosition.yz) + fracWPSum;
	randomSeed.y = dot(fracWorldPosition.yz, fracWorldPosition.zx) + fracWPSum;
	randomSeed.z = dot(fracWorldPosition.zx, fracWorldPosition.xy) + fracWPSum;

	half3 rvec = RandomVector(randomSeed.xy);
	half3 tangent = normalize(rvec - normal * dot(rvec, normal));
	half3 bitangent = cross(normal, tangent);
	half3x3 tbn = half3x3(tangent, bitangent, normal);

	half occlusion = 0.0f;
	[loop]
	for (uint i = 0; i < _SSAODesc.NumSamples; ++i)
	{
		half3 dir = mul(kernels[i], tbn);
		dir = mul(half4(dir, 0.0f), _ViewMatrix).xyz;
		half3 vSamplePos = vPosition + dir * _SSAODesc.Radius * Random(uv);

		half4 offset = half4(vSamplePos, 1.0f);
		offset = mul(offset, _ProjectionMatrix);

		half2 sampleUV = half2(offset.xy / offset.w) * half2(1.0f, -1.0f);
		sampleUV = sampleUV * 0.5f + 0.5f;

		[branch]
		if (!IsSaturated(sampleUV))
			continue;

		// Must use Linear Sampler
		half sampleDepth = _Depth.SampleLevel(linearSampler, sampleUV, 0).r;
		half sampleVDepth = ToViewSpace(sampleUV, sampleDepth, invProjMatrix).z;

		//float rangeCheck = abs(vPosition.z - sampleVDepth) < _SSAODesc.Radius ? 1.0f : 0.0f;
		half rangeCheck = smoothstep(0.0, 1.0, _SSAODesc.Radius / abs(vPosition.z - sampleVDepth));

		half emptyDepthCheck = half(sampleDepth < 1.0f);

		occlusion += half(sampleVDepth <= vSamplePos.z - _SSAODesc.MinZ) * rangeCheck * emptyDepthCheck;
	}

	occlusion = 1.0f - (occlusion / _SSAODesc.NumSamples);
	occlusion = (_SSAODesc.NumSamples > 0) ? occlusion : 1.0f;
	occlusion = pow(abs(occlusion), _SSAODesc.Power);
	return lerp(1.0f, occlusion, occlusionMask);
}

float4 PS_MAIN_SSAO_WriteOcclusion(PS_IN In) : SV_TARGET
{
	half occlusion = SSAORayMarch(In.UV);
	return half4(occlusion, occlusion, occlusion, 1.0f);
}

// Middle Step ==================
// Horizontal Blur
// Vertical Blur
// ==============================

float4 PS_MAIN_SSAO_ApplyOcclusion(PS_IN In) : SV_TARGET
{
	half4 diffuse = _Albedo.Sample(pointSampler, In.UV);
	half occlusionMask = _Occlusion_Reflection_ReflectionBlur_ReflectMask.Sample(pointSampler, In.UV).r;
	half4 occlusion = 1.0f - _Sample.Sample(pointSampler, In.UV);
	occlusion = sin(PI / 2.0f * occlusion); // Adjust
	return half4(0, 0, 0, occlusion.r * diffuse.a * (1.0f - _SSAODesc.Transparency) * occlusionMask);
}

// SSR =======================================================================================================

half2 SSRBinarySearch(half3 vRayPos, half3 vDir)
{
	float4x4 invProjMatrix = Inverse(_ProjectionMatrix);

	vRayPos -= vDir * _SSRDesc.Step * 0.5f;

	half step = _SSRDesc.Step;
	half2 outputUV = half2(0.0f, 0.0f);

	for (int i = 0; i < 6; ++i)
	{
		half4 pSamplePos = mul(half4(vRayPos, 1.0f), _ProjectionMatrix);
		half3 sSamplePos = pSamplePos.xyz / pSamplePos.w;
		sSamplePos.xy = sSamplePos.xy * half2(1.0f, -1.0f) * 0.5f + 0.5f;

		half sampleDepth = _Depth.SampleLevel(pointSampler, sSamplePos.xy, 0).r;

		half3 vSamplePos = ToViewSpace(sSamplePos.xy, sampleDepth, invProjMatrix);

		half depthDiff = vRayPos.z - vSamplePos.z;

		if (depthDiff >= _SSRDesc.Bias && depthDiff <= _SSRDesc.Thickness)
		{
			outputUV = sSamplePos.xy;
			vRayPos -= vDir * step;
		}
		else
		{
			vRayPos += vDir * step;
		}

		step *= 0.5f;
	}

	return outputUV;
}

half2 SSRRayMarch(half2 uv)
{
	float4x4 invProjMatrix = Inverse(_ProjectionMatrix);

	half depth = _Depth.Sample(pointSampler, uv).r;

	[flatten] // Fast exit
	if (depth >= 1.0f) return half2(0.0f, 0.0f);

	half3 vPos = ToViewSpace(uv, depth, invProjMatrix);
	half3 vPixelDir = normalize(vPos);

	half3 normal = UnpackNormal(_Normal, pointSampler, uv);
	half3 vNormal = mul(half4(normal, 0.0f), _ViewMatrix).xyz;
	vNormal = normalize(vNormal);
	half3 vDir = reflect(vPixelDir, vNormal);
	vDir = normalize(vDir);

	half3 vRayPos = vPos;
	half step = _SSRDesc.Step;

	[loop]
	for (uint i = 0; i < _SSRDesc.NumSamples; ++i)
	{
		vRayPos += vDir * step;

		half4 pSamplePos = mul(half4(vRayPos, 1.0f), _ProjectionMatrix);
		half3 sSamplePos = pSamplePos.xyz / pSamplePos.w;
		sSamplePos.xy = sSamplePos.xy * half2(1.0f, -1.0f) * 0.5f + 0.5f;

		half sampleDepth = _Depth.SampleLevel(pointSampler, sSamplePos.xy, 0).r;

		half3 vSamplePos = ToViewSpace(sSamplePos.xy, sampleDepth, invProjMatrix);

		half depthDiff = vRayPos.z - vSamplePos.z;

		if (depthDiff >= _SSRDesc.Bias && depthDiff <= _SSRDesc.Thickness)
		{
			return SSRBinarySearch(vRayPos, vDir);
		}
	}

	return half2(0.0f, 0.0f);
}

inline half SSRSideFadeFunction(half a, half w, half h, half x)
{
	// a: downscale
	// w: range
	// h: step limit
	// x: variable
	half f0 = (x - w / 2);
	half f1 = 1.0f - (4.0f / w * w) * a * f0 * f0;
	half f2 = smoothstep(0, h, f1);
	return f2;
}

inline half SSRSideFadeFunction(half a, half w, half h, half2 uv)
{
	return SSRSideFadeFunction(a, w, h, uv.x) * SSRSideFadeFunction(a, w, h, uv.y);
}

half4 PS_MAIN_SSR_Write(PS_IN In) : SV_TARGET
{
	half4 color = half4(0,0,0,0);

	half2 uv = SSRRayMarch(In.UV);

	half sideFade = SSRSideFadeFunction(1.0f, 1, 0.3f, uv);

	half4 orrr = _Occlusion_Reflection_ReflectionBlur_ReflectMask.Sample(pointSampler, In.UV);
	half reflectMask = orrr.a;

	color.rgb = _Sample.Sample(pointSampler, uv, 0).rgb;
	color.a = reflectMask * sideFade;

	return color;
}

half4 PS_MAIN_SSR_Apply(PS_IN In) : SV_TARGET
{
	half4 reflectedColor = _Sample.Sample(pointSampler, In.UV);
	half4 reflectedBlurColor = _Other.Sample(pointSampler, In.UV);

	half4 orrr = _Occlusion_Reflection_ReflectionBlur_ReflectMask.Sample(pointSampler, In.UV);
	half reflection = orrr.g;
	half reflectionBlur = orrr.b;

	half4 color = lerp(reflectedColor, reflectedBlurColor, reflectionBlur);
	color.a *= reflection;

	return color;
}

// DOF =============================================================================================================

half4 PS_MAIN_DOF_WritePass0(PS_IN In) : SV_TARGET
{
	//// Horizontal Blur 

	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 1.0f);
	int size = _DOFDesc.BlurNumSamples;
	int numSamples = size * 2 + 1;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustX = _DOFDesc.BlurPixelDistance * i / numSamples;
		half2 sampleUV = In.UV + half2(adjustX, 0.0f) * deltaPixel;

		half4 sampleColor = _Sample.Sample(linearSampler, sampleUV);
		accumulation += sampleColor;
	}

	half4 horizontalBlur = accumulation / numSamples;
	horizontalBlur.a = 1.0f;

	//// 

	return horizontalBlur;
}

half4 PS_MAIN_DOF_WritePass1(PS_IN In) : SV_TARGET
{
	//// Verticla Blur 

	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 1.0f);
	int size = _DOFDesc.BlurNumSamples;
	int numSamples = size * 2 + 1;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustY = _DOFDesc.BlurPixelDistance * i / numSamples;
		half2 sampleUV = In.UV + half2(0.0f, adjustY) * deltaPixel;

		half4 sampleColor = _Sample.SampleLevel(pointSampler, sampleUV, 0);
		accumulation += sampleColor;
	}

	half4 blur = accumulation / numSamples;
	blur.a = 1.0f;

	//// Blend 

	half4 color = _Result.Sample(pointSampler, In.UV);

	half depth = _Depth.Sample(pointSampler, In.UV).r;
	half3 vPosition = ToViewSpace(In.UV, depth, Inverse(_ProjectionMatrix));

	half percent = smoothstep(_DOFDesc.MinZ, _DOFDesc.MinZ + _DOFDesc.RangeZ, vPosition.z);

	half4 lerped = lerp(color, blur, percent);
	return lerped;

	//// 
}

half4 PS_MAIN_DOF_Apply(PS_IN In) : SV_TARGET
{
	half4 color = _Sample.Sample(pointSampler, In.UV);
	return color;
}

// Fog =======================================================================================================

half4 PS_MAIN_Fog_Apply_Distance(PS_IN In) : SV_TARGET
{
	half depth = _Depth.Sample(pointSampler, In.UV).r;
	half3 vPosition = ToViewSpace(In.UV, depth, Inverse(_ProjectionMatrix));

	half d = length(vPosition.xyz - _ViewPosition.xyz);

	half percent = smoothstep(_FogDesc.MinZ, _FogDesc.MinZ + _FogDesc.RangeZ, d);

	half4 fogColor = _FogDesc.Color;
	fogColor.a *= percent;

	return fogColor;
}

half4 PS_MAIN_Fog_Apply_Z(PS_IN In) : SV_TARGET
{
	half depth = _Depth.Sample(pointSampler, In.UV).r;
	half3 vPosition = ToViewSpace(In.UV, depth, Inverse(_ProjectionMatrix));
	half vDepth = vPosition.z;

	half percent = smoothstep(_FogDesc.MinZ, _FogDesc.MinZ + _FogDesc.RangeZ, vDepth);

	half4 fogColor = _FogDesc.Color;
	fogColor.a *= percent;

	return fogColor;
}

// Bloom =====================================================================================================

half4 PS_MAIN_Bloom_Extract(PS_IN In) : SV_TARGET
{
	const static half4 BLACK = half4(0, 0, 0, 1);

	half4 sampleColor = _Sample.Sample(pointSampler, In.UV);
	half brightness = Brightness(sampleColor.rgb);
	//half brightness = saturate(length(sampleColor.rgb));

	//half percent = _BloomDesc.Threshold * brightness;
	//half percent = smoothstep(1.0f - _BloomDesc.Threshold, 1.0f, brightness);
	half percent = step(1.0f - _BloomDesc.Threshold, brightness);
	half4 brightedColor = normalize(sampleColor);

	return lerp(BLACK, brightedColor, percent);
}

// Middle Step ==================
// Horizontal Blur
// Vertical Blur
// ==============================

half4 PS_MAIN_Bloom_Apply_Add(PS_IN In) : SV_TARGET
{
	half4 sampleColor = _Sample.Sample(pointSampler, In.UV);

	half4 color;
	color.rgb = sampleColor.rgb;
	color.a = 1.0f;
	color *= _BloomDesc.Intensity;
	color = min(half4(1.0f, 1.0f, 1.0f, 1.0f), color);

	return color;
}

half4 PS_MAIN_Bloom_Apply_Mix(PS_IN In) : SV_TARGET
{
	half4 sampleColor = _Sample.Sample(pointSampler, In.UV);
	half brightness = Brightness(sampleColor.rgb);

	half4 color;
	color.rgb = normalize(sampleColor.rgb);
	color.a = brightness;
	color *= _BloomDesc.Intensity;
	color = min(half4(1.0f, 1.0f, 1.0f, 1.0f), color);

	return color;
}

// Chromatic Aberration ============================================================================================

half4 PS_MAIN_ChromaticAberration_Write(PS_IN In) : SV_TARGET
{
	half4 radAngle = _ChromaticAberrationDesc.Angle * Deg2Rad;

	half2 pixelDist = 1.0f / _TextureSize.xy;
	half2 r_dir = half2(cos(radAngle.r), sin(radAngle.r)) * pixelDist * _ChromaticAberrationDesc.Offset.r;
	half2 g_dir = half2(cos(radAngle.g), sin(radAngle.g)) * pixelDist * _ChromaticAberrationDesc.Offset.g;
	half2 b_dir = half2(cos(radAngle.b), sin(radAngle.b)) * pixelDist * _ChromaticAberrationDesc.Offset.b;

	half3 sampleColor;
	sampleColor.r = _Sample.Sample(pointSampler, In.UV + r_dir).r;
	sampleColor.g = _Sample.Sample(pointSampler, In.UV + g_dir).g;
	sampleColor.b = _Sample.Sample(pointSampler, In.UV + b_dir).b;

	half4 currentColor = _Sample.Sample(pointSampler, In.UV);

	half4 result;
	result.r = lerp(currentColor.r, sampleColor.r, _ChromaticAberrationDesc.Blend.r);
	result.g = lerp(currentColor.g, sampleColor.g, _ChromaticAberrationDesc.Blend.g);
	result.b = lerp(currentColor.b, sampleColor.b, _ChromaticAberrationDesc.Blend.b);
	result.a = currentColor.a;

	return result;
}

half4 PS_MAIN_ChromaticAberration_Apply(PS_IN In) : SV_TARGET
{
	return _Sample.Sample(pointSampler, In.UV);
}

// Blur ============================================================================================================

half4 PS_MAIN_HorizontalBlur(PS_IN In) : SV_TARGET
{
	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustX = _BlurDesc.PixelDistance * i / numSamples;
		half2 sampleUV = In.UV + half2(adjustX, 0.0f) * deltaPixel;

		half4 color = _Sample.SampleLevel(linearSampler, sampleUV, 0);
		accumulation += color;
	}

	half4 color = accumulation / numSamples;
	return color;
}

half4 PS_MAIN_VerticalBlur(PS_IN In) : SV_TARGET
{
	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustY = _BlurDesc.PixelDistance * i / numSamples;
		half2 sampleUV = In.UV + half2(0.0f, adjustY) * deltaPixel;

		half4 color = _Sample.SampleLevel(pointSampler, sampleUV, 0);
		accumulation += color;
	}

	half4 color = accumulation / numSamples;
	return color;
}

half4 PS_MAIN_HorizontalInvDepthBlur(PS_IN In) : SV_TARGET
{
	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	half depth = _Depth.Sample(pointSampler, In.UV).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustX = _BlurDesc.PixelDistance * (1 - depth) * i / numSamples;
		half2 sampleUV = In.UV + half2(adjustX, 0.0f) * deltaPixel;

		half4 color = _Sample.SampleLevel(linearSampler, sampleUV, 0);
		accumulation += color;
	}

	half4 color = accumulation / numSamples;
	return color;
}

half4 PS_MAIN_VerticalInvDepthBlur(PS_IN In) : SV_TARGET
{
	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	half depth = _Depth.Sample(pointSampler, In.UV).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustY = _BlurDesc.PixelDistance * (1 - depth) * i / numSamples;
		half2 sampleUV = In.UV + half2(0.0f, adjustY) * deltaPixel;

		half4 color = _Sample.SampleLevel(pointSampler, sampleUV, 0);
		accumulation += color;
	}

	half4 color = accumulation / numSamples;
	return color;
}

half4 PS_MAIN_HorizontalDepthBlur(PS_IN In) : SV_TARGET
{
	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	half depth = _Depth.Sample(pointSampler, In.UV).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustX = _BlurDesc.PixelDistance * depth * i / numSamples;
		half2 sampleUV = In.UV + half2(adjustX, 0.0f) * deltaPixel;

		half4 sampleColor = _Sample.SampleLevel(linearSampler, sampleUV, 0);
		accumulation += sampleColor;
	}

	half4 color = accumulation / numSamples;
	return color;
}

half4 PS_MAIN_VerticalDepthBlur(PS_IN In) : SV_TARGET
{
	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	half depth = _Depth.Sample(pointSampler, In.UV).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustY = _BlurDesc.PixelDistance * depth * i / numSamples;
		half2 sampleUV = In.UV + half2(0.0f, adjustY) * deltaPixel;

		half4 sampleColor = _Sample.SampleLevel(pointSampler, sampleUV, 0);
		accumulation += sampleColor;
	}

	half4 color = accumulation / numSamples;
	return color;
}

// Screen ==============================================================================================

half4 PS_MAIN_Screen_Default(PS_IN In) : SV_TARGET
{
	return _Sample.Sample(pointSampler, In.UV);
}

half4 PS_MAIN_Screen_Alphatest(PS_IN In) : SV_TARGET
{
	half4 color = _Sample.Sample(pointSampler, In.UV);
	if (color.a < 1.0f)
		discard;
	return color;
}

half4 PS_MAIN_Screen_AlphaToDark(PS_IN In) : SV_TARGET
{
	const static half4 DARK = half4(0.0f, 0.0f, 0.0f, 1.0f);
	half4 color = _Sample.Sample(pointSampler, In.UV);
	half4 result = color.a < 1.0f ? DARK : color;
	return result;
}

half4 PS_MAIN_Screen_LinearDepth(PS_IN In) : SV_TARGET
{
	const static half CONST_NEAR = 0.1f;
	const static half CONST_FAR = 50.0f;
	half4 color = _Sample.Sample(pointSampler, In.UV);
	half linearDepth = ToLinearDepth(CONST_NEAR, CONST_FAR, color.r);
	return half4(linearDepth, linearDepth, linearDepth, 1.0f);
}

// =====================================================================================================

RasterizerState RasterizerState0
{
	FillMode = Solid;
	Cullmode = None;
};

DepthStencilState DepthStencilState0
{
	DepthEnable = false;
	DepthFunc = always;
	DepthWriteMask = zero;
};

BlendState BlendState_None
{
	BlendEnable[0] = false;
};

BlendState BlendState_Mix
{
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState BlendState_Add
{
	BlendEnable[0] = true;
	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;
};

technique11 PostProcessing
{
	pass SSAO_WirteOcclusion
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_SSAO_WriteOcclusion();
	}
	pass SSAO_ApplyOcclusion
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_SSAO_ApplyOcclusion();
	}
	pass SSR_Write
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_SSR_Write();
	}
	pass SSR_Apply
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_SSR_Apply();
	}
	pass DOF_WritePass0
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_DOF_WritePass0();
	}
	pass DOF_WritePass1
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_DOF_WritePass1();
	}
	pass DOF_Apply
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_DOF_Apply();
	}
	pass Fog_Apply_Distance
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Fog_Apply_Distance();
	}
	pass Fog_Apply_Z
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Fog_Apply_Z();
	}
	pass Bloom_Extract
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Bloom_Extract();
	}
	pass Bloom_Apply_Add
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Bloom_Apply_Add();
	}
	pass Bloom_Apply_Mix
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Bloom_Apply_Mix();
	}
	pass ChromaticAberration_Write
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ChromaticAberration_Write();
	}
	pass ChromaticAberration_Apply
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ChromaticAberration_Apply();
	}
}
technique11 Blur
{
	pass HozizontalBlur
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_HorizontalBlur();
	}
	pass VerticalBlur
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_VerticalBlur();
	}
	pass HozizontalInvDepthBlur
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_HorizontalInvDepthBlur();
	}
	pass VerticalInvDepthBlur
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_VerticalInvDepthBlur();
	}
	pass HozizontalDepthBlur
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_HorizontalDepthBlur();
	}
	pass VerticalDepthBlur
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_VerticalDepthBlur();
	}
};
technique11 Screen
{
	pass Default
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Screen_Default();
	}
	pass Alphatest
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Screen_Alphatest();
	}
	pass Alphablend
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Screen_Default();
	}
	pass AlphaToDark
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Screen_AlphaToDark();
	}
	pass LinearDepth
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Mix, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Screen_LinearDepth();
	}
}