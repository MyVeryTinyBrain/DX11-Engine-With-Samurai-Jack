#include "Engine.hlsl"

struct VS_IN
{
	float3	Position : POSITION;
	float2	UV : TEXCOORD;
};

struct PS_IN
{
	float4	Position : SV_POSITION;
	float2	UV : TEXCOORD;
};

struct PS_OUT
{
	float4	light : SV_TARGET0;
	float4	specular : SV_TARGET1;
	float4	volumetric : SV_TARGET2;
};

struct ShadowDesc
{
	half	Value;
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
	float		ShadowBias;				// [0~1]
	float		Near;
	float		Far;
	float4		Position;
	float4		Direction;
	float4		Diffuse;
	float4		Ambient;
	matrix		ViewMatrix[6];
	matrix		ProjectionMatrix[6];
};

struct VolumetricDesc
{
	bool		DrawVolumetric;
	uint		NumSamples;
	float		Intensity;
	float		Power;
};

LightDesc				_LightDesc;
VolumetricDesc			_VolumetricDesc;
texture2D				_Normal;
texture2D				_WorldPosition;
texture2D				_Depth;
texture2D				_Light_Occlusion_Shadow;
texture2D				_Specular_Power;
texture2D				_LightDepthMap[6];
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

	output.Position = float4(In.Position, 1.0f);
	output.UV = In.UV;

	return output;
}

void UnpackGBuffersForLight(half2 uv,
	out half3 normal,
	out float4 worldPosition,
	out half depth,
	out half occlusionMask, out half shadowMask,
	out half3 specularMask, out half specularPower)
{
	normal = _Normal.Sample(pointSampler, uv).xyz;

	depth = _Depth.Sample(pointSampler, uv).r;

	half4 depthLightOcclusionShadow = _Light_Occlusion_Shadow.Sample(pointSampler, uv);
	occlusionMask = depthLightOcclusionShadow.g;
	shadowMask = depthLightOcclusionShadow.b;

	half3 viewPosition = ToViewSpace(uv, depth, Inverse(_ProjectionMatrix));
	worldPosition.xyz = mul(float4(viewPosition, 1.0f), Inverse(_ViewMatrix)).xyz;
	worldPosition.w = 1.0f;

	half4 specular_Power = _Specular_Power.Sample(pointSampler, uv);
	specularMask = specular_Power.rgb;
	specularPower = specular_Power.a;
}

half ComputeDistanceAtten(half3 lightToPixel)
{
	half d = length(lightToPixel);
	half distAtten = max(_LightDesc.Range - d, 0) / _LightDesc.Range;
	return distAtten;
}

half ComputeAngleAtten(half3 lightToPixel)
{
	half cosAngle = cos(_LightDesc.Angle * Deg2Rad);
	half oneMinusCosAngle = 1 - cosAngle;
	half angleAtten = (dot(normalize(lightToPixel), _LightDesc.Direction.xyz) - cosAngle) / oneMinusCosAngle;
	angleAtten = saturate(angleAtten);
	return angleAtten;
}

half ComputeLightIntensity(half3 lightDirection, half3 normal)
{
	lightDirection = normalize(lightDirection);
	half nDotL = dot(lightDirection * -1.0f, normal);
	return saturate(nDotL);
}

half ComputeFresnel(half3 viewToPixel, half3 normal)
{
	viewToPixel = -normalize(viewToPixel);
	half f0 = 0.04f;
	half cosTheta = dot(viewToPixel, normal);
	half fresnel = f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
	return saturate(fresnel);
}

half ComputeSpecular(half3 viewToPixel, half3 normal, half power)
{
	viewToPixel = normalize(viewToPixel);
	half3 reflection = reflect(viewToPixel * -1.0f, normal);
	half rDotV = dot(reflection, viewToPixel);
	rDotV = saturate(rDotV);
	half specular = pow(rDotV, power);
	return specular;
}

inline half SampleShadow(uint index, half2 texCoord, half depth)
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

ShadowDesc ComputeShadowCommon(uint index, half shadowWhiteness, float4 worldPosition)
{
	ShadowDesc shadowDesc = (ShadowDesc)0;

	half4 lightViewPosition;
	lightViewPosition = mul(worldPosition, _LightDesc.ViewMatrix[index]);
	lightViewPosition = mul(lightViewPosition, _LightDesc.ProjectionMatrix[index]);

	half2 projectTexCoord;
	projectTexCoord.x = (lightViewPosition.x / lightViewPosition.w) / 2.0f + 0.5f;
	projectTexCoord.y = -(lightViewPosition.y / lightViewPosition.w) / 2.0f + 0.5f;

	half depthValueByCamera = lightViewPosition.z / lightViewPosition.w;
	depthValueByCamera = depthValueByCamera - _LightDesc.ShadowBias;

	half depth = lightViewPosition.z / lightViewPosition.w;

	[flatten]
	if (IsSaturated(projectTexCoord))
	{
		shadowDesc.InShadowMap = true;

		half depthValueByLight = SampleShadow(index, projectTexCoord, depth);

		[flatten]
		if (depthValueByCamera <= depthValueByLight)
		{
			shadowDesc.Value += 1.0f;
		}
		else
		{
			shadowDesc.Value += shadowWhiteness;
		}
	}
	else
	{
		shadowDesc.InShadowMap = false;
		return shadowDesc;
	}

	return shadowDesc;
}

ShadowDesc ComputeShadowCommonPCF3X3(uint index, half shadowWhiteness, float4 worldPosition)
{
	ShadowDesc shadowDesc = (ShadowDesc)0;

	half4 lightViewPosition;
	lightViewPosition = mul(worldPosition, _LightDesc.ViewMatrix[index]);
	lightViewPosition = mul(lightViewPosition, _LightDesc.ProjectionMatrix[index]);

	half2 projectTexCoord;
	projectTexCoord.x = (lightViewPosition.x / lightViewPosition.w) / 2.0f + 0.5f;
	projectTexCoord.y = -(lightViewPosition.y / lightViewPosition.w) / 2.0f + 0.5f;

	half depthValueByCamera = lightViewPosition.z / lightViewPosition.w;
	depthValueByCamera = depthValueByCamera - _LightDesc.ShadowBias;

	half depth = lightViewPosition.z / lightViewPosition.w;

	static const int NUM_SAMPLES = 3;
	const half delta = 1.0f / _LightDesc.DepthSize;
	int numInShadowMap = 1;

	[flatten]
	if (IsSaturated(projectTexCoord))
	{
		shadowDesc.InShadowMap = true;
	}
	else
	{
		shadowDesc.InShadowMap = false;
		return shadowDesc;
	}

	[unroll]
	for (int i = 0; i < NUM_SAMPLES * NUM_SAMPLES; ++i)
	{
		int stepU = (i % NUM_SAMPLES) - (NUM_SAMPLES / 2);
		int stepV = (i / NUM_SAMPLES) - NUM_SAMPLES / 2;
		half2 sampeUV = projectTexCoord + half2(stepU, stepV) * delta;
		half depthValueByLight = SampleShadow(index, sampeUV, depth);

		[flatten]
		if (depthValueByCamera <= depthValueByLight)
		{
			shadowDesc.Value += 1.0f;
		}
		else
		{
			shadowDesc.Value += shadowWhiteness;
		}
	}

	shadowDesc.Value /= (NUM_SAMPLES * NUM_SAMPLES);
	return shadowDesc;
}

half ComputeShadow_Directional(half shadowWhiteness, float4 worldPosition)
{
	// 가까운 쉐도우맵부터 순회합니다.
	// 픽셀의 월드 위치가 i번째 쉐도우맵에 포함되어 있다면 i번째 쉐도우맵에 대하여 연산합니다.
	[unroll]
	for (uint i = 0; i < 3; ++i)
	{
		ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(i, shadowWhiteness, worldPosition);

		[flatten]
		if (shadowDesc.InShadowMap)
			return shadowDesc.Value;
	}

	// 이 픽셀이 그림자 맵의 영역 외부에 있다면 조명 연산된 밝기를 사용합니다.
	return 1.0f;
}

half ComputeShadow_Point(half3 lightToPixel, half shadowWhiteness, float4 worldPosition)
{
	// PointLight.h에 정의된 뷰 행렬의 순서입니다.
	// const V3	m_arrDirection[6] = { V3::right(),V3::left(),V3::forward(),V3::back(),V3::up(),V3::down(), };

	// 선택된 쉐도우맵 인덱스입니다.
	uint shadowmapIndex = 0;

	// 쉐도우맵의 방향과 조명이 픽셀까지의 방향을 내적한 값들중 최대값입니다.
	// 이것은 쉐도우맵의 방향과 가장 적은 각도차이를 나타냅니다.
	// 또한 가장 적은 각도차이를 나타내는 인덱스를 사용하게 됩니다.
	half maxDot = -2.0f;
	half dotResult[6];
	dotResult[0] = dot(lightToPixel, half3(1, 0, 0));
	dotResult[1] = dot(lightToPixel, -half3(1, 0, 0));
	dotResult[2] = dot(lightToPixel, half3(0, 0, 1));
	dotResult[3] = dot(lightToPixel, -half3(0, 0, 1));
	dotResult[4] = dot(lightToPixel, half3(0, 1, 0));
	dotResult[5] = dot(lightToPixel, -half3(0, 1, 0));

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

	ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(shadowmapIndex, shadowWhiteness, worldPosition);
	return shadowDesc.Value;
}

half ComputeShadow_Spot(half shadowWhiteness, float4 worldPosition)
{
	ShadowDesc shadowDesc = ComputeShadowCommonPCF3X3(0, shadowWhiteness, worldPosition);
	return shadowDesc.Value;
}

int RaycastSphere(float3 rayPoint, half3 rayDir, float3 spherePoint, float radius, out float3 intersects[2])
{
	float3 o_minus_c = rayPoint - spherePoint;

	float p = dot(rayDir, o_minus_c);
	float q = dot(o_minus_c, o_minus_c) - (radius * radius);

	float discriminant = (p * p) - q;

	[flatten]
	if (discriminant < 0.0f)
	{
		return 0.0f;
	}
	else
	{
		float dRoot = sqrt(discriminant);
		float dist0 = -p - abs(dRoot);
		float dist1 = -p + abs(dRoot);

		intersects[0] = rayPoint + rayDir * dist0;
		intersects[1] = rayPoint + rayDir * dist1;

		return (discriminant > 1e-7) ? 2 : 1;
	}
}

half4 ComputeVolumetric_Point(float3 worldPosition)
{
	float3 viewToPixel = worldPosition - _ViewPosition.xyz;
	float3 intersects[2];
	int numPoints = RaycastSphere(_ViewPosition.xyz, normalize(viewToPixel), _LightDesc.Position.xyz, _LightDesc.Range, intersects);

	float3 start, end;

	[flatten]
	if (numPoints == 0)
	{
		return half4(0, 0, 0, 0);
	}
	else if (numPoints == 1 || length(_LightDesc.Position.xyz - _ViewPosition.xyz) < _LightDesc.Range)
	{
		start = _ViewPosition.xyz;
		end = intersects[1];
	}
	else
	{
		start = intersects[0];
		end = intersects[1];
	}

	float3 delta = end - start;
	float deltaDistance = length(delta);
	half3 dir = delta / deltaDistance;
	half endVDepth = mul(float4(worldPosition, 1.0f), _ViewMatrix).z;

	uint numStep = _VolumetricDesc.NumSamples;
	half step = deltaDistance / numStep;
	uint numSampled = 0;

	half shaft = 0.0f;
	[loop]
	for (uint i = 0; i < numStep; ++i)
	{
		half distance = step * i;
		float4 wp;
		wp.xyz = start + dir * distance;
		wp.w = 1.0f;

		half sampleVDepth = mul(wp, _ViewMatrix).z;
		if (sampleVDepth > endVDepth)
			break;

		half3 lightToWP = (wp.xyz - _LightDesc.Position.xyz);
		half da = ComputeDistanceAtten(lightToWP);
		half aa = ComputeAngleAtten(lightToWP);
		half a = da * aa;
		half wpShaft = ComputeShadow_Spot(0, wp) * a;
		shaft += wpShaft;
		++numSampled;
	}
	shaft /= numSampled;
	shaft = pow(shaft * _VolumetricDesc.Intensity, _VolumetricDesc.Power);
	return half4(_LightDesc.Diffuse.rgb * shaft, 1.0f);
}

half4 ComputeVolumetric_Spot(float3 worldPosition)
{
	float3 viewToPixel = worldPosition - _ViewPosition.xyz;
	float coneSideLength = _LightDesc.Range / cos(_LightDesc.Angle * Deg2Rad);
	float3 coneCenter = (_LightDesc.Position.xyz + _LightDesc.Position.xyz + _LightDesc.Direction.xyz * _LightDesc.Range * 0.5f) * 0.5f;
	float3 intersects[2];
	int numPoints = RaycastSphere(_ViewPosition.xyz, normalize(viewToPixel), coneCenter, coneSideLength * 0.5f, intersects);

	float3 start, end;

	[flatten]
	if (numPoints == 0)
	{
		return half4(0, 0, 0, 0);
	}
	else if (numPoints == 1 || length(coneCenter - _ViewPosition.xyz) < coneSideLength * 0.5f)
	{
		start = _ViewPosition.xyz;
		end = intersects[1];
	}
	else
	{
		start = intersects[0];
		end = intersects[1];
	}

	float3 delta = end - start;
	float deltaDistance = length(delta);
	half3 dir = delta / deltaDistance;
	half endVDepth = mul(float4(worldPosition, 1.0f), _ViewMatrix).z;

	uint numStep = _VolumetricDesc.NumSamples;
	half step = deltaDistance / numStep;
	uint numSampled = 0;

	half shaft = 0.0f;
	[loop]
	for (uint i = 0; i < numStep; ++i)
	{
		half distance = step * i;
		float4 wp;
		wp.xyz = start + dir * distance;
		wp.w = 1.0f;

		half sampleVDepth = mul(wp, _ViewMatrix).z;
		if (sampleVDepth > endVDepth)
			break;

		half3 lightToWP = (wp.xyz - _LightDesc.Position.xyz);
		half da = ComputeDistanceAtten(lightToWP);
		half aa = ComputeAngleAtten(lightToWP);
		half a = da * aa;
		half wpShaft = ComputeShadow_Spot(0, wp) * a;
		shaft += wpShaft;
		++numSampled;
	}
	shaft /= numSampled;
	shaft = pow(shaft * _VolumetricDesc.Intensity, _VolumetricDesc.Power);
	return half4(_LightDesc.Diffuse.rgb * shaft, 1.0f);
}

PS_OUT PS_MAIN_Directional(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	half3 normal;
	float4 worldPosition;
	half depth;
	half occlusionMask, shadowMask;
	half3 specularMask;
	half specular_Power;
	UnpackGBuffersForLight(In.UV,
		normal,
		worldPosition,
		depth,
		occlusionMask, shadowMask,
		specularMask, specular_Power);

	half lightIntensity = ComputeLightIntensity(_LightDesc.Direction.xyz, normal);
	lightIntensity = (lightIntensity * _LightDesc.Intensity);

	half ambientBrightness = Brightness(_LightDesc.Ambient * _LightDesc.Intensity);

	half shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow)
	{
		shadow = ComputeShadow_Directional(_LightDesc.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	lightIntensity = lightIntensity * shadow;

	half3 viewToPixel = worldPosition.xyz - _ViewPosition.xyz;
	half3 specularIntensity = ComputeSpecular(viewToPixel, normal, specular_Power);
	specularIntensity = (specularIntensity * (lightIntensity + ambientBrightness));

	output.light = half4(_LightDesc.Diffuse.rgb * lightIntensity + _LightDesc.Ambient.rgb * occlusionMask, 1.0f);
	output.specular = half4(_LightDesc.Diffuse.rgb * specularMask * specularIntensity, 1.0f);

	output.volumetric = half4(0, 0, 0, 0);

	return output;
}

PS_OUT PS_MAIN_Point(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	half3 normal;
	float4 worldPosition;
	half depth;
	half occlusionMask, shadowMask;
	half3 specularMask;
	half specular_Power;
	UnpackGBuffersForLight(In.UV,
		normal,
		worldPosition,
		depth,
		occlusionMask, shadowMask,
		specularMask, specular_Power);

	half3 lightToPixel = (worldPosition.xyz - _LightDesc.Position.xyz);

	half atten = ComputeDistanceAtten(lightToPixel);

	half lightIntensity = ComputeLightIntensity(lightToPixel, normal);
	lightIntensity = (lightIntensity * _LightDesc.Intensity * atten);

	half ambientBrightness = Brightness(_LightDesc.Ambient * _LightDesc.Intensity * atten);

	half shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow)
	{
		shadow = ComputeShadow_Point(lightToPixel, _LightDesc.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	lightIntensity = lightIntensity * shadow;

	half3 viewToPixel = worldPosition.xyz - _ViewPosition.xyz;
	half3 specularIntensity = ComputeSpecular(viewToPixel, normal, specular_Power);
	specularIntensity = (specularIntensity * (lightIntensity + ambientBrightness));

	output.light = half4(_LightDesc.Diffuse.rgb * lightIntensity + _LightDesc.Ambient.rgb * atten * occlusionMask, 1.0f);
	output.specular = half4(_LightDesc.Diffuse.rgb * specularMask * specularIntensity, 1.0f);

	half4 volumetric = half4(0, 0, 0, 0);
	[branch]
	if (_VolumetricDesc.DrawVolumetric)
	{
		volumetric = ComputeVolumetric_Point(worldPosition.xyz);
	}
	output.volumetric = volumetric;

	return output;
}

PS_OUT PS_MAIN_Spot(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	half3 normal;
	float4 worldPosition;
	half depth;
	half occlusionMask, shadowMask;
	half3 specularMask;
	half specular_Power;
	UnpackGBuffersForLight(In.UV,
		normal,
		worldPosition,
		depth,
		occlusionMask, shadowMask,
		specularMask, specular_Power);

	half3 lightToPixel = (worldPosition.xyz - _LightDesc.Position.xyz);

	half distAtten = ComputeDistanceAtten(lightToPixel);
	half angleAtten = ComputeAngleAtten(lightToPixel);
	half atten = distAtten * angleAtten;

	half lightIntensity = ComputeLightIntensity(lightToPixel, normal);
	lightIntensity = (lightIntensity * _LightDesc.Intensity * atten);

	half ambientBrightness = Brightness(_LightDesc.Ambient * _LightDesc.Intensity * atten);

	half shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow)
	{
		shadow = ComputeShadow_Spot(_LightDesc.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	lightIntensity = lightIntensity * shadow;

	half3 viewToPixel = worldPosition.xyz - _ViewPosition.xyz;
	half3 specularIntensity = ComputeSpecular(viewToPixel, normal, specular_Power);
	specularIntensity = (specularIntensity * (lightIntensity + ambientBrightness));

	output.light = half4(_LightDesc.Diffuse.rgb * lightIntensity + _LightDesc.Ambient.rgb * atten * occlusionMask, 1.0f);
	output.specular = half4(_LightDesc.Diffuse.rgb * specularMask * specularIntensity, 1.0f);

	half4 volumetric = half4(0, 0, 0, 0);
	[branch]
	if (_VolumetricDesc.DrawVolumetric)
	{
		volumetric = ComputeVolumetric_Spot(worldPosition.xyz);
	}
	output.volumetric = volumetric;

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
