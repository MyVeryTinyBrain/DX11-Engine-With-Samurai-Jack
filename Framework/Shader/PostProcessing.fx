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
SamplerState	textureSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	output.position = float4(In.position, 1.0f);
	output.uv = In.uv;

	return output;
}

// SSAO =======================================================================================================

float SSAORayMarch(float2 uv)
{
	const static int NUM_KERNEL = 16;
	const static float3 kernels[NUM_KERNEL] =
	{
		normalize(float3(0.2024537f, 0.841204f, 0.9060141f)),		//1
		normalize(float3(0.3677659f, 0.1086345f, 0.4466777f)),		//3
		normalize(float3(0.7867433f, -0.141479f, 0.1567597f)),		//4
		normalize(float3(0.4839356f, -0.8253108f, 0.1563844f)),		//5
		normalize(float3(-0.4192392f, 0.2084218f, 0.3672943f)),		//6
		normalize(float3(-0.8433938f, 0.1451271f, 0.2202872f)),		//7
		normalize(float3(0.0019193f, -0.8048455f, 0.0726584f)),		//11
		normalize(float3(-0.7578573f, -0.5583301f, 0.2347527f)),	//12
		normalize(float3(-0.4540417f, -0.252365f, 0.0694318f)),		//13
		normalize(float3(-0.4037157f, -0.8263387f, 0.4698132f)),	//15
		normalize(float3(-0.6657394f, 0.6298575f, 0.6342437f)),		//8
		normalize(float3(-0.2200423f, 0.6282339f, 0.8275437f)),		//2
		normalize(float3(0.8775856f, 0.4617546f, 0.6427765f)),		//9
		normalize(float3(-0.0001783f, 0.2834622f, 0.8343929f)),		//10
		normalize(float3(-0.0483353f, -0.2527294f, 0.5924745f)),	//14
		normalize(float3(0.4401554f, -0.4228428f, 0.3300118f)),		//16
	};

	float4 packedWorldPosition = _WorldPosition.Sample(textureSampler, uv, 0);
	float3 worldPosition = UnpackWorldPosition(packedWorldPosition);
	float3 viewPosition = mul(float4(worldPosition, 1.0f), _ViewMatrix).xyz;

	float4 packedNormal = _Normal.Sample(textureSampler, uv, 0);
	float3 normal = UnpackNormal(packedNormal);

	float4 packedDepthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, uv, 0);
	float depth = packedDepthLightOcclusionShadow.r;
	float occlusionMask = packedDepthLightOcclusionShadow.z;

	float3 randomSeed = 0;
	randomSeed.x = dot(worldPosition.xy, worldPosition.yz) + frac(worldPosition.x + worldPosition.y + worldPosition.z);
	randomSeed.y = dot(worldPosition.yz, worldPosition.zx) + frac(worldPosition.x + worldPosition.y + worldPosition.z);
	randomSeed.z = dot(worldPosition.zx, worldPosition.xy) + frac(worldPosition.x + worldPosition.y + worldPosition.z);

	float3 rvec = RandomVector(randomSeed.xy);
	float3 tangent = normalize(rvec - normal * dot(rvec, normal));
	float3 bitangent = cross(normal, tangent);
	float3x3 tbn = float3x3(tangent, bitangent, normal);

	float occlusion = 0.0f;
	[unroll(NUM_KERNEL)]
	for (uint i = 0; i < _SSAODesc.NumSamples; ++i)
	{
		float3 dir = mul(kernels[i], tbn);
		dir = mul(float4(dir, 0.0f), _ViewMatrix).xyz;
		float3 viewSamplePos = viewPosition + dir * _SSAODesc.Radius * Random(randomSeed.xy);

		float4 offset = float4(viewSamplePos, 1.0f);
		offset = mul(offset, _ProjectionMatrix);
		
		float2 sampleUV = float2(offset.xy / offset.w) * float2(1.0f, -1.0f);
		sampleUV = sampleUV * 0.5f + 0.5f;

		//[flatten]
		//if (saturate(sampleUV.x) != sampleUV.x || saturate(sampleUV.y) != sampleUV.y)
		//	continue;

		float4 samplePackedWorldPosition = _WorldPosition.SampleLevel(textureSampler, sampleUV, 0);
		float3 sampleWorldPosition = UnpackWorldPosition(samplePackedWorldPosition);
		float3 sampleViewPosition = mul(float4(sampleWorldPosition, 1.0f), _ViewMatrix).xyz;
		float sampleDepth = sampleViewPosition.z;

		//float rangeCheck = abs(viewPosition.z - sampleDepth) < _SSAODesc.Radius ? 1.0f : 0.0f;
		float rangeCheck = smoothstep(0.0, 1.0, _SSAODesc.Radius / abs(viewPosition.z - sampleDepth));

		float emptyDepthCheck = float(depth < 1.0f);

		occlusion += (sampleDepth <= viewSamplePos.z - _SSAODesc.MinZ ? 1.0 : 0.0) * rangeCheck * emptyDepthCheck;
	}

	occlusion = 1.0f - (occlusion / _SSAODesc.NumSamples);
	occlusion = (_SSAODesc.NumSamples > 0) ? occlusion : 1.0f;
	return pow(abs(occlusion), _SSAODesc.Power) * occlusionMask;
}

float4 PS_MAIN_SSAO_WriteOcclusion(PS_IN In) : SV_TARGET
{
	float occlusion = SSAORayMarch(In.uv);
	return float4(occlusion, occlusion, occlusion, 1.0f);
}

// Middle Step ==================
// Horizontal Blur
// Vertical Blur
// ==============================

float4 PS_MAIN_SSAO_ApplyOcclusion(PS_IN In) : SV_TARGET
{
	float4 diffuse = _Diffuse.Sample(textureSampler, In.uv);
	float occlusionMask = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv).b;
	float4 occlusion = 1.0f - _Sample.Sample(textureSampler, In.uv);
	return float4(0, 0, 0, occlusion.r * diffuse.a * (1.0f - _SSAODesc.Transparency) * occlusionMask);
}

// SSR =======================================================================================================

float2 SSRRayMarch(float2 uv)
{
	const int	_Loop = _SSRDesc.NumSamples;
	const float _Step = _SSRDesc.Step;
	const float _Thickness = _SSRDesc.Thickness;

	float depth = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, uv).r;

	float2 outputUV = float2(0, 0);
	[flatten]
	if (depth >= 1.0f)
		return outputUV;

	float3 viewPosition = ToViewSpace(uv, depth, Inverse(_ProjectionMatrix));
	float3 viewDir = normalize(viewPosition);
	float viewDepth = viewPosition.z;

	float3 worldNormal = UnpackNormal(_Normal.Sample(textureSampler, uv));
	worldNormal = normalize(worldNormal);
	float3 viewNormal = mul(float4(worldNormal, 0.0f), _ViewMatrix).xyz;
	viewNormal = normalize(viewNormal);

	float3 reflectDir = reflect(viewDir, viewNormal);
	reflectDir = normalize(reflectDir);

	float3 current = viewPosition;

	[loop]
	for (int i = 0; i < _Loop; ++i)
	{
		current += reflectDir * _Step;

		float4 sampleProj = mul(float4(current, 1.0f), _ProjectionMatrix);
		sampleProj.xyz /= sampleProj.w;

		float2 sampleUV = sampleProj.xy * float2(1.0f, -1.0f) * 0.5f + 0.5f;

		[branch]
		if (saturate(sampleUV.x) != sampleUV.x || saturate(sampleUV.y) != sampleUV.y)
			break;

		float targetDepth = current.z;
		float sampleDepth = _Depth_Light_Occlusion_Shadow.SampleLevel(textureSampler, sampleUV, 0).r;
		float3 sampleView = ToViewSpace(sampleUV, sampleDepth, Inverse(_ProjectionMatrix));
		sampleDepth = sampleView.z;

		float depthDiff = targetDepth - sampleDepth;

		float3 delta = sampleView - viewPosition;
		float3 deltaDir = normalize(delta);

		[branch]
		if (depthDiff >= _Thickness && depthDiff <= _SSRDesc.Bias)
		{
			outputUV = sampleUV;
			break;
		}
	}

	return outputUV;
}

inline float SSRFadeFunction(float a, float w, float h, float x)
{
	// a: downscale
	// w: range
	// h: step limit
	// x: variable
	float f0 = (x - w / 2);
	float f1 = 1.0f - (4.0f / w * w) * a * f0 * f0;
	float f2 = smoothstep(0, h, f1);
	return f2;
}

inline float SSRFadeFunction(float a, float w, float h, float2 uv)
{
	return SSRFadeFunction(a, w, h, uv.x) * SSRFadeFunction(a, w, h, uv.y);
}

float4 PS_MAIN_SSR_Write(PS_IN In) : SV_TARGET
{
	float4 color = float4(0,0,0,0);

	float2 uv = SSRRayMarch(In.uv);

	float fade = SSRFadeFunction(1, 1, 0.3f, uv);
	
	float4 reflectionReflectMask = _Reflection_ReflectMask.Sample(textureSampler, uv);
	float reflectMask = reflectionReflectMask.g;

	color.rgb = _Sample.Sample(textureSampler, uv, 0).rgb;
	color.a = reflectMask * fade;

	return color;
}

float4 PS_MAIN_SSR_Apply(PS_IN In) : SV_TARGET
{
	float4 color = _Sample.Sample(textureSampler, In.uv);

	float4 reflectionReflectMask = _Reflection_ReflectMask.Sample(textureSampler, In.uv);
	float reflection = reflectionReflectMask.r;
	color.a *= reflection;

	return color;
}

// Fog =======================================================================================================

float4 PS_MAIN_Fog_Apply_Distance(PS_IN In) : SV_TARGET
{
	float4 packedDepthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv);
	float depth = packedDepthLightOcclusionShadow.r;
	float3 viewPosition = ToViewSpace(In.uv, depth, Inverse(_ProjectionMatrix));

	float d = length(viewPosition.xyz - _ViewPosition.xyz);

	float percent = smoothstep(_FogDesc.MinZ, _FogDesc.MinZ + _FogDesc.RangeZ, d);

	float4 fogColor = _FogDesc.Color;
	fogColor.a *= percent;

	return fogColor;
}

float4 PS_MAIN_Fog_Apply_Z(PS_IN In) : SV_TARGET
{
	float4 packedDepthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv);
	float depth = packedDepthLightOcclusionShadow.r;
	float3 viewPosition = ToViewSpace(In.uv, depth, Inverse(_ProjectionMatrix));
	float viewDepth = viewPosition.z;

	float percent = smoothstep(_FogDesc.MinZ, _FogDesc.MinZ + _FogDesc.RangeZ, viewDepth);

	float4 fogColor = _FogDesc.Color;
	fogColor.a *= percent;

	return fogColor;
}

// Bloom =====================================================================================================

float4 PS_MAIN_Bloom_Extract(PS_IN In) : SV_TARGET
{
	const static float4 BLACK = float4(0, 0, 0, 1);

	float4 sampleColor = _Sample.Sample(textureSampler, In.uv);
	float brightness = Brightness(sampleColor.rgb) - 0.01f;

	float percent = smoothstep(1.0f - _BloomDesc.Threshold, 1.0f, brightness);
	float4 brightedColor = normalize(sampleColor);

	return lerp(BLACK, brightedColor, percent);
}

// Middle Step ==================
// Horizontal Blur
// Vertical Blur
// ==============================

float4 PS_MAIN_Bloom_Apply_Add(PS_IN In) : SV_TARGET
{
	float4 sampleColor = _Sample.Sample(textureSampler, In.uv);

	float4 color;
	color.rgb = sampleColor.rgb;
	color.a = 1.0f;
	color *= _BloomDesc.Intensity;
	color = min(float4(1.0f, 1.0f, 1.0f, 1.0f), color);

	return color;
}

float4 PS_MAIN_Bloom_Apply_Mix(PS_IN In) : SV_TARGET
{
	float4 sampleColor = _Sample.Sample(textureSampler, In.uv);
	float brightness = Brightness(sampleColor.rgb);

	float4 color;
	color.rgb = normalize(sampleColor.rgb);
	color.a = brightness;
	color *= _BloomDesc.Intensity;
	color = min(float4(1.0f, 1.0f, 1.0f, 1.0f), color);

	return color;
}

// Linear DOF ======================================================================================================

float4 PS_MAIN_LinearDOF_WritePass0(PS_IN In) : SV_TARGET
{
	//// Horizontal Blur 

	float2 deltaPixel = float2(1.0f, 1.0f) / _TextureSize.xy;
	float4 accumulation = float4(0.0f, 0.0f, 0.0f, 1.0f);
	int size = _LinearDOFDesc.BlurNumSamples;
	int numSamples = size * 2 + 1;

	[unroll(32 + 1)]
	for (int i = -size; i <= size; ++i)
	{
		float adjustX = _LinearDOFDesc.BlurPixelDistance * i / numSamples;
		float2 sampleUV = In.uv + float2(adjustX, 0.0f) * deltaPixel;

		float4 sampleColor = _Sample.Sample(textureSampler, sampleUV);
		accumulation += sampleColor;
	}

	float4 horizontalBlur = accumulation / numSamples;
	horizontalBlur.a = 1.0f;

	//// 

	return horizontalBlur;
}

float4 PS_MAIN_LinearDOF_WritePass1(PS_IN In) : SV_TARGET
{
	//// Verticla Blur 

	float2 deltaPixel = float2(1.0f, 1.0f) / _TextureSize.xy;
	float4 accumulation = float4(0.0f, 0.0f, 0.0f, 1.0f);
	int size = _LinearDOFDesc.BlurNumSamples;
	int numSamples = size * 2 + 1;

	[unroll(32 + 1)]
	for (int i = -size; i <= size; ++i)
	{
		float adjustY = _LinearDOFDesc.BlurPixelDistance * i / numSamples;
		float2 sampleUV = In.uv + float2(0.0f, adjustY) * deltaPixel;

		float4 sampleColor = _Sample.SampleLevel(textureSampler, sampleUV, 0);
		accumulation += sampleColor;
	}

	float4 blur = accumulation / numSamples;
	blur.a = 1.0f;

	//// Blend 

	float4 color = _Result.Sample(textureSampler, In.uv);

	float depth = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv).r;
	float3 viewPosition = ToViewSpace(In.uv, depth, Inverse(_ProjectionMatrix));

	float percent = smoothstep(_LinearDOFDesc.MinZ, _LinearDOFDesc.MinZ + _LinearDOFDesc.RangeZ, viewPosition.z);

	float4 lerped = lerp(color, blur, percent);
	return lerped;

	//// 
}

float4 PS_MAIN_LinearDOF_Apply(PS_IN In) : SV_TARGET
{
	float4 color = _Sample.Sample(textureSampler, In.uv);
	return color;
}

// Blur ======================================================================================================

float4 PS_MAIN_HorizontalBlur(PS_IN In) : SV_TARGET
{
	float2 deltaPixel = float2(1.0f, 1.0f) / _TextureSize.xy;
	float4 accumulation = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	[unroll(32 + 1)]
	for (int i = -size; i <= size; ++i)
	{
		float adjustX = _BlurDesc.PixelDistance * i / numSamples;
		float2 sampleUV = In.uv + float2(adjustX, 0.0f) * deltaPixel;

		float4 color = _Sample.SampleLevel(textureSampler, sampleUV, 0);
		accumulation += color;
	}

	float4 color = accumulation / numSamples;
	return color;
}

float4 PS_MAIN_VerticalBlur(PS_IN In) : SV_TARGET
{
	float2 deltaPixel = float2(1.0f, 1.0f) / _TextureSize.xy;
	float4 accumulation = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	float depth = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv).r;

	[unroll(32 + 1)]
	for (int i = -size; i <= size; ++i)
	{
		float adjustY = _BlurDesc.PixelDistance * i / numSamples;
		float2 sampleUV = In.uv + float2(0.0f, adjustY) * deltaPixel;

		float4 color = _Sample.SampleLevel(textureSampler, sampleUV, 0);
		accumulation += color;
	}

	float4 color = accumulation / numSamples;
	return color;
}

float4 PS_MAIN_HorizontalInvDepthBlur(PS_IN In) : SV_TARGET
{
	float2 deltaPixel = float2(1.0f, 1.0f) / _TextureSize.xy;
	float4 accumulation = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	float depth = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv).r;

	[unroll(32 + 1)]
	for (int i = -size; i <= size; ++i)
	{
		float adjustX = _BlurDesc.PixelDistance * (1 - depth) * i / numSamples;
		float2 sampleUV = In.uv + float2(adjustX, 0.0f) * deltaPixel;

		float4 color = _Sample.SampleLevel(textureSampler, sampleUV, 0);
		accumulation += color;
	}

	float4 color = accumulation / numSamples;
	return color;
}

float4 PS_MAIN_VerticalInvDepthBlur(PS_IN In) : SV_TARGET
{
	float2 deltaPixel = float2(1.0f, 1.0f) / _TextureSize.xy;
	float4 accumulation = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	float depth = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv).r;

	[unroll(32 + 1)]
	for (int i = -size; i <= size; ++i)
	{
		float adjustY = _BlurDesc.PixelDistance * (1 - depth) * i / numSamples;
		float2 sampleUV = In.uv + float2(0.0f, adjustY) * deltaPixel;

		float4 color = _Sample.SampleLevel(textureSampler, sampleUV, 0);
		accumulation += color;
	}

	float4 color = accumulation / numSamples;
	return color;
}

float4 PS_MAIN_HorizontalDepthBlur(PS_IN In) : SV_TARGET
{
	float2 deltaPixel = float2(1.0f, 1.0f) / _TextureSize.xy;
	float4 accumulation = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	float depth = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv).r;

	[unroll(32 + 1)]
	for (int i = -size; i <= size; ++i)
	{
		float adjustX = _BlurDesc.PixelDistance * depth * i / numSamples;
		float2 sampleUV = In.uv + float2(adjustX, 0.0f) * deltaPixel;

		float4 sampleColor = _Sample.SampleLevel(textureSampler, sampleUV, 0);
		accumulation += sampleColor;
	}

	float4 color = accumulation / numSamples;
	return color;
}

float4 PS_MAIN_VerticalDepthBlur(PS_IN In) : SV_TARGET
{
	float2 deltaPixel = float2(1.0f, 1.0f) / _TextureSize.xy;
	float4 accumulation = float4(0.0f, 0.0f, 0.0f, 0.0f);
	int size = _BlurDesc.NumSamples;
	int numSamples = size * 2 + 1;

	float depth = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv).r;

	[unroll(32 + 1)]
	for (int i = -size; i <= size; ++i)
	{
		float adjustY = _BlurDesc.PixelDistance * depth * i / numSamples;
		float2 sampleUV = In.uv + float2(0.0f, adjustY) * deltaPixel;

		float4 sampleColor = _Sample.SampleLevel(textureSampler, sampleUV, 0);
		accumulation += sampleColor;
	}

	float4 color = accumulation / numSamples;
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