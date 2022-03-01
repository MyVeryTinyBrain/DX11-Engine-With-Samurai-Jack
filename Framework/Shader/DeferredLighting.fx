#include "Engine.hlsl"

struct VS_IN
{
	float3	position : POSITION;
	float2	uv : TEXCOORD;
};

struct PS_IN
{
	float4	position : SV_POSITION;
	float2	uv : TEXCOORD;
};

struct PS_OUT
{
	float4	light : SV_TARGET0;
	float4	specular : SV_TARGET1;
};

struct ShadowDesc
{
	float	Value;
	bool	InShadowMap;
};

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2

struct LightDesc
{
	bool		DrawShadow;
	uint		Type;
	uint		DepthSize;
	float		ShadowWhiteness;
	float		Intensity;
	float		Range;					// [0~inf]
	float		Angle;					// [0~90]
	float		Near;
	float		Far;
	float4		Position;
	float4		Direction;
	float4		Diffuse;
	float4		Ambient;
	matrix		ViewMatrix[6];
	matrix		ProjectionMatrix[6];
};

LightDesc				_LightDesc;
texture2D				_Normal;
texture2D				_WorldPosition;
texture2D				_Depth_Light_Occlusion_Shadow;
texture2D				_Specular_Power;
texture2D				_LightDepthMap[6];
SamplerState			textureSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
};
SamplerComparisonState	shadowSampler
{
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	MinLOD = 0.0f;
	MaxLOD = FLT_MAX;
	MipLODBias = 0.0f;
	MaxAnisotropy = 0;
	ComparisonFunc = LESS_EQUAL;
	Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	output.position = float4(In.position, 1.0f);
	output.uv = In.uv;

	return output;
}

void UnpackGBuffersForLight(float2 uv,
	out float3 normal,
	out float4 worldPosition,
	out float occlusionMask, out float shadowMask,
	out float3 specularMask, out float specular_Power)
{
	float4 packedNormal = _Normal.Sample(textureSampler, uv);
	float3 unpackedNormal = UnpackNormal(packedNormal);

	float4 packedWorldPosition = _WorldPosition.Sample(textureSampler, uv);
	float4 unpackedWorldPosition = float4(UnpackWorldPosition(packedWorldPosition), 1.0f);

	float4 unpackedDepthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, uv);
	float OcclusionMask = unpackedDepthLightOcclusionShadow.z;
	float ShadowMask = unpackedDepthLightOcclusionShadow.w;

	float4 packedSpecularPower = _Specular_Power.Sample(textureSampler, uv);
	float3 unpackedSpecularMask = packedSpecularPower.rgb;
	float unpackedSpecularPower = packedSpecularPower.a;

	normal = unpackedNormal;
	worldPosition = unpackedWorldPosition;
	occlusionMask = OcclusionMask;
	shadowMask = ShadowMask;
	specularMask = unpackedSpecularMask;
	specular_Power = unpackedSpecularPower;
}

float ComputeDistanceAtten(float3 lightToPixel)
{
	float d = length(lightToPixel);
	float distAtten = max(_LightDesc.Range - d, 0) / _LightDesc.Range;
	return distAtten;
}

float ComputeAngleAtten(float3 lightToPixel)
{
	float cosAngle = cos(_LightDesc.Angle * Deg2Rad);
	float oneMinusCosAngle = 1 - cosAngle;
	float angleAtten = (dot(normalize(lightToPixel), _LightDesc.Direction.xyz) - cosAngle) / oneMinusCosAngle;
	angleAtten = saturate(angleAtten);
	return angleAtten;
}

float ComputeLightIntensity(float3 lightDirection, float3 normal)
{
	lightDirection = normalize(lightDirection);
	float nDotL = dot(lightDirection * -1.0f, normal);
	return saturate(nDotL);
}

float ComputeFresnel(float3 viewToPixel, float3 normal)
{
	viewToPixel = -normalize(viewToPixel);
	float f0 = 0.04f;
	float cosTheta = dot(viewToPixel, normal);
	float fresnel = f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
	return saturate(fresnel);
}

float ComputeSpecular(float3 viewToPixel, float3 normal, float power)
{
	viewToPixel = normalize(viewToPixel);
	float3 reflection = reflect(viewToPixel * -1.0f, normal);
	float rDotV = dot(reflection, viewToPixel);
	rDotV = saturate(rDotV);
	float specular = pow(rDotV, power);
	return specular;
}

float SampleShadow(uint index, float2 texCoord, float depth)
{
	switch (index)
	{
		case 0: return _LightDepthMap[0].SampleCmpLevelZero(shadowSampler, texCoord, depth).r;
		case 1: return _LightDepthMap[1].SampleCmpLevelZero(shadowSampler, texCoord, depth).r;
		case 2: return _LightDepthMap[2].SampleCmpLevelZero(shadowSampler, texCoord, depth).r;
		case 3: return _LightDepthMap[3].SampleCmpLevelZero(shadowSampler, texCoord, depth).r;
		case 4: return _LightDepthMap[4].SampleCmpLevelZero(shadowSampler, texCoord, depth).r;
		case 5: return _LightDepthMap[5].SampleCmpLevelZero(shadowSampler, texCoord, depth).r;
		default: return _LightDepthMap[0].SampleCmpLevelZero(shadowSampler, texCoord, depth).r;
	}
}

ShadowDesc ComputeShadowCommon(uint i, float4 worldPosition, float lightIntensity, float shadowWhiteness)
{
	ShadowDesc shadowDesc = (ShadowDesc)0;

	float4 lightViewPosition;
	lightViewPosition = mul(worldPosition, _LightDesc.ViewMatrix[i]);
	lightViewPosition = mul(lightViewPosition, _LightDesc.ProjectionMatrix[i]);

	float2 projectTexCoord;
	projectTexCoord.x = (lightViewPosition.x / lightViewPosition.w) / 2.0f + 0.5f;
	projectTexCoord.y = -(lightViewPosition.y / lightViewPosition.w) / 2.0f + 0.5f;

	float depth = lightViewPosition.z / lightViewPosition.w;

	float bias = 0.0001f;
	float depthValueByCamera = lightViewPosition.z / lightViewPosition.w;
	depthValueByCamera = depthValueByCamera - bias;

	// 이 픽셀이 쉐도우맵 내부에 있는지 검사합니다.
	[flatten]
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		shadowDesc.InShadowMap = true;

		float depthValueByLight = SampleShadow(i, projectTexCoord.xy, depth);

		// 이 픽셀이 가려져 있지 않다면 조명 연산된 밝기를 사용합니다.
		[flatten]
		if (depthValueByCamera <= depthValueByLight)
		{
			[flatten]
			if (lightIntensity > 0.0f)
			{
				shadowDesc.Value = 1.0f;
				return shadowDesc;
			}
		}

		// 이 픽셀이 가려져 있다면 설정된 그림자 밝기를 사용합니다.
		shadowDesc.Value = shadowWhiteness;
		return shadowDesc;
	}

	// 이 픽셀이 그림자 맵의 영역 외부에 있다면 조명 연산된 밝기를 사용합니다.
	shadowDesc.Value = 1.0f;
	return shadowDesc;
}

ShadowDesc ComputeShadowCommonPCF3X3(uint i, float4 worldPosition, float lightIntensity, float shadowWhiteness)
{
	ShadowDesc shadowDesc = (ShadowDesc)0;

	float4 lightViewPosition;
	lightViewPosition = mul(worldPosition, _LightDesc.ViewMatrix[i]);
	lightViewPosition = mul(lightViewPosition, _LightDesc.ProjectionMatrix[i]);

	float2 projectTexCoord;
	projectTexCoord.x = (lightViewPosition.x / lightViewPosition.w) / 2.0f + 0.5f;
	projectTexCoord.y = -(lightViewPosition.y / lightViewPosition.w) / 2.0f + 0.5f;

	float depth = lightViewPosition.z / lightViewPosition.w;

	float bias = 0.0001f;
	float depthValueByCamera = lightViewPosition.z / lightViewPosition.w;
	depthValueByCamera = depthValueByCamera - bias;

	static const uint NUM_SAMPLES = 3;
	const float delta = 1.0f / _LightDesc.DepthSize;
	const float2 adjust[8] = 
	{
		float2(-delta, +delta),	float2(0, +delta),	float2(+delta, +delta),
		float2(-delta, 0),							float2(+delta, 0),
		float2(-delta, -delta), float2(0, -delta),	float2(+delta, -delta),
	};
	uint numInShadowMap = 1;
	
	// 이 픽셀(중심 픽셀)이 쉐도우맵 내부에 있는지 검사합니다.
	[flatten]
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		shadowDesc.InShadowMap = true;

		float depthValueByLight = SampleShadow(i, projectTexCoord.xy, depth);

		// 이 픽셀이 가려져 있지 않다면 조명 연산된 밝기를 사용합니다.
		[flatten]
		if (depthValueByCamera <= depthValueByLight)
		{
			[flatten]
			if (lightIntensity > 0.0f)
			{
				shadowDesc.Value += 1.0f;
			}
		}
		else
		{
			// 이 픽셀이 가려져 있다면 설정된 그림자 밝기를 사용합니다.
			shadowDesc.Value += shadowWhiteness;
		}
	}
	else
	{
		// 이 픽셀이 그림자 맵의 영역 외부에 있다면 조명 연산된 밝기를 사용합니다.
		shadowDesc.Value += 1.0f;
		// 중심 픽셀이 쉐도우맵 외부에 있다면, 중심 픽셀에 대해서만 연산합니다.
		return shadowDesc;
	}

	[unroll]
	for (uint j = 0; j < NUM_SAMPLES * NUM_SAMPLES - 1; ++j)
	{
		// 새로 샘플링할 UV 좌표입니다.
		float2 sampleProjectTexCoorrd = projectTexCoord + adjust[j];

		// 이 픽셀이 쉐도우맵 내부에 있는지 검사합니다.
		[flatten]
		if ((saturate(sampleProjectTexCoorrd.x) == sampleProjectTexCoorrd.x) && (saturate(sampleProjectTexCoorrd.y) == sampleProjectTexCoorrd.y))
		{
			float depthValueByLight = SampleShadow(i, sampleProjectTexCoorrd, depth);

			// 이 픽셀이 가려져 있지 않다면 조명 연산된 밝기를 사용합니다.
			[flatten]
			if (depthValueByCamera <= depthValueByLight)
			{
				[flatten]
				if (lightIntensity > 0.0f)
				{
					shadowDesc.Value += 1.0f;
				}
			}
			else
			{
				// 이 픽셀이 가려져 있다면 설정된 그림자 밝기를 사용합니다.
				shadowDesc.Value += shadowWhiteness;
			}

			++numInShadowMap;
		}
	}

	shadowDesc.Value /= numInShadowMap;
	return shadowDesc;
}

float ComputeShadow_Directional(float4 worldPosition, float lightIntensity)
{
	// 가까운 쉐도우맵부터 순회합니다.
	// 픽셀의 월드 위치가 i번째 쉐도우맵에 포함되어 있다면 i번째 쉐도우맵에 대하여 연산합니다.
	[unroll]
	for (uint i = 0; i < 3; ++i)
	{
		ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(i, worldPosition, lightIntensity, _LightDesc.ShadowWhiteness);

		[flatten]
		if (shadowDesc.InShadowMap)
			return shadowDesc.Value;
	}

	// 이 픽셀이 그림자 맵의 영역 외부에 있다면 조명 연산된 밝기를 사용합니다.
	return 1.0f;
}

float ComputeShadow_Point(float3 lightToPixel, float4 worldPosition, float lightIntensity)
{
	// PointLight.h에 정의된 뷰 행렬의 순서입니다.
	// const V3	m_arrDirection[6] = { V3::right(),V3::left(),V3::forward(),V3::back(),V3::up(),V3::down(), };

	// 선택된 쉐도우맵 인덱스입니다.
	uint shadowmapIndex = 0;

	// 쉐도우맵의 방향과 조명이 픽셀까지의 방향을 내적한 값들중 최대값입니다.
	// 이것은 쉐도우맵의 방향과 가장 적은 각도차이를 나타냅니다.
	// 또한 가장 적은 각도차이를 나타내는 인덱스를 사용하게 됩니다.
	float maxDot = -2.0f;
	float dotResult[6];
	dotResult[0] = dot(lightToPixel, float3(1, 0, 0));
	dotResult[1] = dot(lightToPixel, -float3(1, 0, 0));
	dotResult[2] = dot(lightToPixel, float3(0, 0, 1));
	dotResult[3] = dot(lightToPixel, -float3(0, 0, 1));
	dotResult[4] = dot(lightToPixel, float3(0, 1, 0));
	dotResult[5] = dot(lightToPixel, -float3(0, 1, 0));

	// 가장 적은 각도차이를 가지는 인덱스를 구합니다.
	[unroll]
	for (uint i = 0; i < 6; ++i)
	{
		[flatten]
		if (dotResult[i] > maxDot)
		{
			maxDot = dotResult[i];
			shadowmapIndex = i;
		}
	}

	ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(shadowmapIndex, worldPosition, lightIntensity, _LightDesc.ShadowWhiteness);
	return shadowDesc.Value;
}

float ComputeShadow_Spot(float4 worldPosition, float lightIntensity)
{
	ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(0, worldPosition, lightIntensity, _LightDesc.ShadowWhiteness);
	return shadowDesc.Value;
}

PS_OUT PS_MAIN_Directional(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	float3 normal;
	float4 worldPosition;
	float occlusionMask, shadowMask;
	float3 specularMask;
	float specular_Power;
	UnpackGBuffersForLight(In.uv,
		normal,
		worldPosition,
		occlusionMask, shadowMask,
		specularMask, specular_Power);

	float lightIntensity = ComputeLightIntensity(_LightDesc.Direction.xyz, normal);
	lightIntensity = saturate(lightIntensity * _LightDesc.Intensity);

	float ambientBrightness = Brightness(_LightDesc.Ambient * _LightDesc.Intensity);

	float shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow)
	{
		shadow = ComputeShadow_Directional(worldPosition, lightIntensity + ambientBrightness);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	lightIntensity = lightIntensity * shadow;

	float3 viewToPixel = worldPosition.xyz - _ViewPosition.xyz;
	float3 specularIntensity = ComputeSpecular(viewToPixel, normal, specular_Power);
	specularIntensity = saturate(specularIntensity * (lightIntensity + ambientBrightness));

	output.light = float4(_LightDesc.Diffuse.rgb * lightIntensity + _LightDesc.Ambient.rgb * occlusionMask, 1.0f);
	output.specular = float4(_LightDesc.Diffuse.rgb * specularMask * specularIntensity, 1.0f);

	return output;
}

PS_OUT PS_MAIN_Point(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	float3 normal;
	float4 worldPosition;
	float occlusionMask, shadowMask;
	float3 specularMask;
	float specular_Power;
	UnpackGBuffersForLight(In.uv,
		normal,
		worldPosition,
		occlusionMask, shadowMask,
		specularMask, specular_Power);

	float4 packedDepthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv);
	float depth = packedDepthLightOcclusionShadow.r;
	float3 viewPosition = ToViewSpace(In.uv, depth, Inverse(_ProjectionMatrix));
	worldPosition = mul(float4(viewPosition, 1.0f), Inverse(_ViewMatrix));
	float3 lightToPixel = (worldPosition.xyz - _LightDesc.Position.xyz);

	float atten = ComputeDistanceAtten(lightToPixel);

	float lightIntensity = ComputeLightIntensity(lightToPixel, normal);
	lightIntensity = saturate(lightIntensity * _LightDesc.Intensity * atten);

	float ambientBrightness = Brightness(_LightDesc.Ambient * _LightDesc.Intensity * atten);

	float shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow)
	{
		shadow = ComputeShadow_Point(lightToPixel, worldPosition, lightIntensity + ambientBrightness);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	lightIntensity = lightIntensity * shadow;

	float3 viewToPixel = worldPosition.xyz - _ViewPosition.xyz;
	float3 specularIntensity = ComputeSpecular(viewToPixel, normal, specular_Power);
	specularIntensity = saturate(specularIntensity * (lightIntensity + ambientBrightness));

	output.light = float4(_LightDesc.Diffuse.rgb * lightIntensity + _LightDesc.Ambient.rgb * atten * occlusionMask, 1.0f);
	output.specular = float4(_LightDesc.Diffuse.rgb * specularMask * specularIntensity, 1.0f);

	return output;
}

PS_OUT PS_MAIN_Spot(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	float3 normal;
	float4 worldPosition;
	float occlusionMask, shadowMask;
	float3 specularMask;
	float specular_Power;
	UnpackGBuffersForLight(In.uv,
		normal,
		worldPosition,
		occlusionMask, shadowMask,
		specularMask, specular_Power);

	float4 packedDepthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(textureSampler, In.uv);
	float depth = packedDepthLightOcclusionShadow.r;
	float3 viewPosition = ToViewSpace(In.uv, depth, Inverse(_ProjectionMatrix));
	worldPosition = mul(float4(viewPosition, 1.0f), Inverse(_ViewMatrix));
	float3 lightToPixel = (worldPosition.xyz - _LightDesc.Position.xyz);

	float distAtten = ComputeDistanceAtten(lightToPixel);
	float angleAtten = ComputeAngleAtten(lightToPixel);
	float atten = distAtten * angleAtten;

	float lightIntensity = ComputeLightIntensity(lightToPixel, normal);
	lightIntensity = saturate(lightIntensity * _LightDesc.Intensity * atten);

	float ambientBrightness = Brightness(_LightDesc.Ambient * _LightDesc.Intensity * atten);

	float shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow)
	{
		shadow = ComputeShadow_Spot(worldPosition, lightIntensity + ambientBrightness);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	lightIntensity = lightIntensity * shadow;

	float3 viewToPixel = worldPosition.xyz - _ViewPosition.xyz;
	float3 specularIntensity = ComputeSpecular(viewToPixel, normal, specular_Power);
	specularIntensity = saturate(specularIntensity * (lightIntensity + ambientBrightness));

	output.light = float4(_LightDesc.Diffuse.rgb * lightIntensity + _LightDesc.Ambient.rgb * atten * occlusionMask, 1.0f);
	output.specular = float4(_LightDesc.Diffuse.rgb * specularMask * specularIntensity, 1.0f);

	return output;
}

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

BlendState BlendState0
{
	BlendEnable[0] = true;
	SrcBlend[0] = One;
	DestBlend[0] = One;
	BlendOp[0] = Add;

	BlendEnable[1] = true;
	SrcBlend[1] = One;
	DestBlend[1] = One;
	BlendOp[1] = Add;

	BlendEnable[2] = true;
	SrcBlend[2] = One;
	DestBlend[2] = One;
	BlendOp[2] = Add;
};

technique11 Technique0
{
	pass DirectionalLight
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Directional();
	}
	pass PointLight
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Point();
	}
	pass SpotLight
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Spot();
	}
}
