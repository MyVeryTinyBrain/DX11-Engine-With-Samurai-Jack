#include "Engine.hlsl"

struct VS_IN
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
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

struct LinearDOFDesc
{
	bool	Enable;
	uint	BlurNumSamples;
	float	MinZ;
	float	RangeZ;
	float	Power;
	float	BlurPixelDistance;
};

struct BlurDesc
{
	bool	DepthBlur;
	uint	NumSamples;
	float	PixelDistance;
};

SSAODesc		_SSAODesc;
FogDesc			_FogDesc;
BloomDesc		_BloomDesc;
SSRDesc			_SSRDesc;
LinearDOFDesc	_LinearDOFDesc;
BlurDesc		_BlurDesc;
float4			_TextureSize;
texture2D		_Diffuse;
texture2D		_Normal;
texture2D		_WorldPosition;
texture2D		_Depth_Light_Occlusion_Shadow;
texture2D		_Reflection_ReflectMask;
texture2D		_Result;
texture2D		_Sample; // For Temp Texture
SamplerState	linearSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
	ComparisonFunc = Never;
	MinLOD = 0;
	MaxLOD = FLT_MAX;
	Filter = MIN_MAG_MIP_LINEAR;
};
SamplerState	pointSampler
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

	output.position = float4(In.position, 1.0f);
	output.uv = In.uv;

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

	half3 normal = _Normal.Sample(pointSampler, uv, 0).xyz;

	half4 depthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, uv, 0);
	float depth = depthLightOcclusionShadow.r;
	half occlusionMask = depthLightOcclusionShadow.z;

	half3 vPosition = ToViewSpace(uv, depth, Inverse(_ProjectionMatrix));
	float3 worldPosition = mul(float4(vPosition, 1.0f), Inverse(_ViewMatrix)).xyz;

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
		half3 vSamplePos = vPosition + dir * _SSAODesc.Radius * Random(randomSeed.xy);

		half4 offset = half4(vSamplePos, 1.0f);
		offset = mul(offset, _ProjectionMatrix);

		half2 sampleUV = half2(offset.xy / offset.w) * half2(1.0f, -1.0f);
		sampleUV = sampleUV * 0.5f + 0.5f;

		[branch]
		if (!IsSaturated(sampleUV))
			continue;

		half sampleDepth = _Depth_Light_Occlusion_Shadow.SampleLevel(linearSampler, sampleUV, 0).r;
		half sampleVDepth = ToViewSpace(sampleUV, sampleDepth, Inverse(_ProjectionMatrix)).z;

		//float rangeCheck = abs(vPosition.z - sampleVDepth) < _SSAODesc.Radius ? 1.0f : 0.0f;
		half rangeCheck = smoothstep(0.0, 1.0, _SSAODesc.Radius / abs(vPosition.z - sampleVDepth));

		half emptyDepthCheck = half(sampleDepth < 1.0f);

		occlusion += half(sampleVDepth <= vSamplePos.z - _SSAODesc.MinZ) * rangeCheck * emptyDepthCheck;
	}

	occlusion = 1.0f - (occlusion / _SSAODesc.NumSamples);
	occlusion = (_SSAODesc.NumSamples > 0) ? occlusion : 1.0f;
	return pow(abs(occlusion), _SSAODesc.Power) * occlusionMask;
}

float4 PS_MAIN_SSAO_WriteOcclusion(PS_IN In) : SV_TARGET
{
	half occlusion = SSAORayMarch(In.uv);
	return half4(occlusion, occlusion, occlusion, 1.0f);
}

// Middle Step ==================
// Horizontal Blur
// Vertical Blur
// ==============================

float4 PS_MAIN_SSAO_ApplyOcclusion(PS_IN In) : SV_TARGET
{
	half4 diffuse = _Diffuse.Sample(pointSampler, In.uv);
	half occlusionMask = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv).b;
	half4 occlusion = 1.0f - _Sample.Sample(pointSampler, In.uv);
	return half4(0, 0, 0, occlusion.r * diffuse.a * (1.0f - _SSAODesc.Transparency) * occlusionMask);
}

// SSR =======================================================================================================

half2 SSRBinarySearch(half3 vRayPos, half3 vDir)
{
	vRayPos -= vDir * _SSRDesc.Step * 0.5f;

	half step = _SSRDesc.Step;
	half2 outputUV = half2(0.0f, 0.0f);

	for (int i = 0; i < 6; ++i)
	{
		half4 pSamplePos = mul(half4(vRayPos, 1.0f), _ProjectionMatrix);
		half3 sSamplePos = pSamplePos.xyz / pSamplePos.w;
		sSamplePos.xy = sSamplePos.xy * half2(1.0f, -1.0f) * 0.5f + 0.5f;

		half sampleDepth = _Depth_Light_Occlusion_Shadow.SampleLevel(pointSampler, sSamplePos.xy, 0).r;

		half3 vSamplePos = ToViewSpace(sSamplePos.xy, sampleDepth, Inverse(_ProjectionMatrix));

		half depthDiff = vRayPos.z - vSamplePos.z;

		if (depthDiff >= 0.0f)
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

half2 SSRRayMarch_Screen(half2 uv)
{
	half depth = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, uv).r;

	[flatten]
	if (depth >= 1.0f)
		return half2(0.0f, 0.0f);

	half3 vPos = ToViewSpace(uv, depth, Inverse(_ProjectionMatrix));
	half3 vPixelDir = normalize(vPos);

	half3 normal = _Normal.Sample(pointSampler, uv).rgb;
	half3 vNormal = mul(float4(normal, 0.0f), _ViewMatrix).xyz;
	vNormal = normalize(vNormal);
	float3 vDir = reflect(vPixelDir, vNormal);
	vDir = normalize(vDir);

	float4 pPos = mul(float4(vPos, 1.0f), _ProjectionMatrix);
	pPos.xyz /= pPos.w;
	float2 sPos = pPos.xy;

	float4 pDir = mul(float4(vDir, 0.0f), _ProjectionMatrix);
	pDir.xyz /= pDir.w;
	float2 sDir = pDir.xy;

	return 0.0f;
}

half2 SSRRayMarch_View(half2 uv)
{
	half depth = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, uv).r;

	[flatten]
	if (depth >= 1.0f)
		return half2(0.0f, 0.0f);

	half3 vPos = ToViewSpace(uv, depth, Inverse(_ProjectionMatrix));
	half3 vPixelDir = normalize(vPos);

	half3 normal = _Normal.Sample(pointSampler, uv).rgb;
	half3 vNormal = mul(half4(normal, 0.0f), _ViewMatrix).xyz;
	vNormal = normalize(vNormal);
	half3 vDir = reflect(vPixelDir, vNormal);
	vDir = normalize(vDir);

	half3 vRayPos = vPos;

	[loop]
	for (uint i = 0; i < _SSRDesc.NumSamples; ++i)
	{
		vRayPos += vDir * _SSRDesc.Step;

		half4 pSamplePos = mul(half4(vRayPos, 1.0f), _ProjectionMatrix);
		half3 sSamplePos = pSamplePos.xyz / pSamplePos.w;
		sSamplePos.xy = sSamplePos.xy * half2(1.0f, -1.0f) * 0.5f + 0.5f;

		half sampleDepth = _Depth_Light_Occlusion_Shadow.SampleLevel(pointSampler, sSamplePos.xy, 0).r;

		half3 vSamplePos = ToViewSpace(sSamplePos.xy, sampleDepth, Inverse(_ProjectionMatrix));

		half depthDiff = vRayPos.z - vSamplePos.z;

		if (depthDiff >= _SSRDesc.Bias && depthDiff <= _SSRDesc.Thickness)
		{
			return SSRBinarySearch(vRayPos, vDir);
		}
	}

	return half2(0.0f, 0.0f);
}

inline half SSRFadeFunction(half a, half w, half h, half x)
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

inline half SSRFadeFunction(half a, half w, half h, half2 uv)
{
	return SSRFadeFunction(a, w, h, uv.x) * SSRFadeFunction(a, w, h, uv.y);
}

half4 PS_MAIN_SSR_Write(PS_IN In) : SV_TARGET
{
	half4 color = half4(0,0,0,0);

	half2 uv = SSRRayMarch_View(In.uv);

	half fade = SSRFadeFunction(1.0f, 1, 0.3f, uv);
	
	half4 reflectionReflectMask = _Reflection_ReflectMask.Sample(pointSampler, uv);
	half reflectMask = reflectionReflectMask.g;

	color.rgb = _Sample.Sample(linearSampler, uv, 0).rgb;
	color.a = reflectMask * fade;

	return color;
}

half4 PS_MAIN_SSR_Apply(PS_IN In) : SV_TARGET
{
	half4 color = _Sample.Sample(linearSampler, In.uv);

	half4 reflectionReflectMask = _Reflection_ReflectMask.Sample(pointSampler, In.uv);
	half reflection = reflectionReflectMask.r;
	color.a *= reflection;

	return color;
}

// Fog =======================================================================================================

half4 PS_MAIN_Fog_Apply_Distance(PS_IN In) : SV_TARGET
{
	half4 depthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv);
	half depth = depthLightOcclusionShadow.r;
	half3 vPosition = ToViewSpace(In.uv, depth, Inverse(_ProjectionMatrix));

	half d = length(vPosition.xyz - _ViewPosition.xyz);

	half percent = smoothstep(_FogDesc.MinZ, _FogDesc.MinZ + _FogDesc.RangeZ, d);

	half4 fogColor = _FogDesc.Color;
	fogColor.a *= percent;

	return fogColor;
}

half4 PS_MAIN_Fog_Apply_Z(PS_IN In) : SV_TARGET
{
	half4 depthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv);
	half depth = depthLightOcclusionShadow.r;
	half3 vPosition = ToViewSpace(In.uv, depth, Inverse(_ProjectionMatrix));
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

	half4 sampleColor = _Sample.Sample(pointSampler, In.uv);
	half brightness = Brightness(sampleColor.rgb);

	half percent = smoothstep(1.0f - _BloomDesc.Threshold, 1.0f, brightness);
	half4 brightedColor = normalize(sampleColor);

	return lerp(BLACK, brightedColor, percent);
}

// Middle Step ==================
// Horizontal Blur
// Vertical Blur
// ==============================

half4 PS_MAIN_Bloom_Apply_Add(PS_IN In) : SV_TARGET
{
	half4 sampleColor = _Sample.Sample(pointSampler, In.uv);

	half4 color;
	color.rgb = sampleColor.rgb;
	color.a = 1.0f;
	color *= _BloomDesc.Intensity;
	color = min(half4(1.0f, 1.0f, 1.0f, 1.0f), color);

	return color;
}

half4 PS_MAIN_Bloom_Apply_Mix(PS_IN In) : SV_TARGET
{
	half4 sampleColor = _Sample.Sample(pointSampler, In.uv);
	half brightness = Brightness(sampleColor.rgb);

	half4 color;
	color.rgb = normalize(sampleColor.rgb);
	color.a = brightness;
	color *= _BloomDesc.Intensity;
	color = min(half4(1.0f, 1.0f, 1.0f, 1.0f), color);

	return color;
}

// Linear DOF ======================================================================================================

half4 PS_MAIN_LinearDOF_WritePass0(PS_IN In) : SV_TARGET
{
	//// Horizontal Blur 

	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 1.0f);
	int size = _LinearDOFDesc.BlurNumSamples;
	int numSamples = size * 2 + 1;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustX = _LinearDOFDesc.BlurPixelDistance * i / numSamples;
		half2 sampleUV = In.uv + half2(adjustX, 0.0f) * deltaPixel;

		half4 sampleColor = _Sample.Sample(pointSampler, sampleUV);
		accumulation += sampleColor;
	}

	half4 horizontalBlur = accumulation / numSamples;
	horizontalBlur.a = 1.0f;

	//// 

	return horizontalBlur;
}

half4 PS_MAIN_LinearDOF_WritePass1(PS_IN In) : SV_TARGET
{
	//// Verticla Blur 

	half2 deltaPixel = half2(1.0f, 1.0f) / _TextureSize.xy;
	half4 accumulation = half4(0.0f, 0.0f, 0.0f, 1.0f);
	int size = _LinearDOFDesc.BlurNumSamples;
	int numSamples = size * 2 + 1;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustY = _LinearDOFDesc.BlurPixelDistance * i / numSamples;
		half2 sampleUV = In.uv + half2(0.0f, adjustY) * deltaPixel;

		half4 sampleColor = _Sample.SampleLevel(pointSampler, sampleUV, 0);
		accumulation += sampleColor;
	}

	half4 blur = accumulation / numSamples;
	blur.a = 1.0f;

	//// Blend 

	half4 color = _Result.Sample(pointSampler, In.uv);

	half depth = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv).r;
	half3 vPosition = ToViewSpace(In.uv, depth, Inverse(_ProjectionMatrix));

	half percent = smoothstep(_LinearDOFDesc.MinZ, _LinearDOFDesc.MinZ + _LinearDOFDesc.RangeZ, vPosition.z);

	half4 lerped = lerp(color, blur, percent);
	return lerped;

	//// 
}

half4 PS_MAIN_LinearDOF_Apply(PS_IN In) : SV_TARGET
{
	half4 color = _Sample.Sample(pointSampler, In.uv);
	return color;
}

// Blur ======================================================================================================

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
		half2 sampleUV = In.uv + half2(adjustX, 0.0f) * deltaPixel;

		half4 color = _Sample.SampleLevel(pointSampler, sampleUV, 0);
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

	half depth = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustY = _BlurDesc.PixelDistance * i / numSamples;
		half2 sampleUV = In.uv + half2(0.0f, adjustY) * deltaPixel;

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

	half depth = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustX = _BlurDesc.PixelDistance * (1 - depth) * i / numSamples;
		half2 sampleUV = In.uv + half2(adjustX, 0.0f) * deltaPixel;

		half4 color = _Sample.SampleLevel(pointSampler, sampleUV, 0);
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

	half depth = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustY = _BlurDesc.PixelDistance * (1 - depth) * i / numSamples;
		half2 sampleUV = In.uv + half2(0.0f, adjustY) * deltaPixel;

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

	half depth = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustX = _BlurDesc.PixelDistance * depth * i / numSamples;
		half2 sampleUV = In.uv + half2(adjustX, 0.0f) * deltaPixel;

		half4 sampleColor = _Sample.SampleLevel(pointSampler, sampleUV, 0);
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

	half depth = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.uv).r;

	[loop]
	for (int i = -size; i <= size; ++i)
	{
		half adjustY = _BlurDesc.PixelDistance * depth * i / numSamples;
		half2 sampleUV = In.uv + half2(0.0f, adjustY) * deltaPixel;

		half4 sampleColor = _Sample.SampleLevel(pointSampler, sampleUV, 0);
		accumulation += sampleColor;
	}

	half4 color = accumulation / numSamples;
	return color;
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
	pass LinearDOF_WritePass0
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_LinearDOF_WritePass0();
	}
	pass LinearDOF_WritePass1
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_LinearDOF_WritePass1();
	}
	pass LinearDOF_Apply
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_LinearDOF_Apply();
	}
}
technique11 Common
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