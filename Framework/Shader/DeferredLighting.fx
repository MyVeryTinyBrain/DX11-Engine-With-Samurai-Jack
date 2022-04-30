#include "Engine.hlsl"
#include "Light.hlsl"

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
	float4	volumetric : SV_TARGET1;
};

LightDesc				_LightDesc;
VolumetricDesc			_VolumetricDesc;
texture2D				_Albedo;
texture2D				_Normal;
texture2D				_Depth;
texture2D				_Light_Shadow;
texture2D				_Roughness_Metallic;
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

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	output.Position = float4(In.Position, 1.0f);
	output.UV = In.UV;

	return output;
}

void UnpackGBuffersForLight(half2 uv,
	out half3 albedo,
	out half3 normal,
	out float4 worldPosition,
	out half depth,
	out half shadowMask,
	out half roughness, out half metallic)
{
	albedo = _Albedo.Sample(pointSampler, uv).rgb;

	normal = UnpackNormal(_Normal, pointSampler, uv);

	depth = _Depth.Sample(pointSampler, uv).r;

	half4 ls = _Light_Shadow.Sample(pointSampler, uv);
	shadowMask = ls.g;

	half3 viewPosition = ToViewSpace(uv, depth, Inverse(_ProjectionMatrix));
	worldPosition.xyz = mul(float4(viewPosition, 1.0f), Inverse(_ViewMatrix)).xyz;
	worldPosition.w = 1.0f;

	half4 rm = _Roughness_Metallic.Sample(pointSampler, uv);
	roughness = rm.r;
	metallic = rm.g;
}

half ComputeLightIntensity(half3 lightDirection, half3 normal)
{
	lightDirection = normalize(lightDirection);
	half nDotL = dot(lightDirection * -1.0f, normal);
	return saturate(nDotL);
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

PS_OUT PS_MAIN_Directional(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	half3 albedo;
	half3 normal;
	float4 worldPosition;
	half depth;
	half shadowMask;
	half roughness, metallic;
	UnpackGBuffersForLight(In.UV,
		albedo,
		normal,
		worldPosition,
		depth,
		shadowMask,
		roughness, metallic);

	half atten = 1.0f;

	float3 world2Light = -_LightDesc.Direction.xyz;
	float3 world2Camera = _ViewPosition.xyz - worldPosition.xyz;
	half lightIntensity;
	half3 Lo = ComputePBRLightIntensity(_LightDesc, atten, albedo, normal, world2Light, world2Camera, roughness, metallic, lightIntensity);

	half shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow && shadowMask > 0.0f)
	{
		shadow = ComputeShadow_Directional(_LightDesc, _LightDepthMap, _LightDesc.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	Lo *= shadow;

	output.light = half4(Lo + _LightDesc.Ambient.rgb * albedo, 1.0f);

	output.volumetric = half4(0, 0, 0, 1);

	return output;
}

PS_OUT PS_MAIN_Point(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	half3 albedo;
	half3 normal;
	float4 worldPosition;
	half depth;
	half shadowMask;
	half roughness, metallic;
	UnpackGBuffersForLight(In.UV,
		albedo,
		normal,
		worldPosition,
		depth,
		shadowMask,
		roughness, metallic);

	half3 lightToPixel = (worldPosition.xyz - _LightDesc.Position.xyz);

	half atten = ComputeDistanceAtten(_LightDesc, worldPosition.xyz);
	float3 world2Light = _LightDesc.Position.xyz - worldPosition.xyz;
	float3 world2Camera = _ViewPosition.xyz - worldPosition.xyz;
	half lightIntensity;
	half3 Lo = ComputePBRLightIntensity(_LightDesc, atten, albedo, normal, world2Light, world2Camera, roughness, metallic, lightIntensity);

	half shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow && shadowMask > 0.0f)
	{
		shadow = ComputeShadow_Point(_LightDesc, _LightDepthMap, lightToPixel, _LightDesc.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	Lo *= shadow;

	output.light = half4(Lo + _LightDesc.Ambient.rgb * albedo * atten, 1.0f);

	half4 volumetric = half4(0, 0, 0, 1);
	[branch]
	if (_VolumetricDesc.DrawVolumetric)
	{
		volumetric = ComputeVolumetric_Point(_LightDesc, _VolumetricDesc, _LightDepthMap, worldPosition.xyz, _ViewPosition.xyz);
	}
	output.volumetric = volumetric;

	return output;
}

PS_OUT PS_MAIN_Spot(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	half3 albedo;
	half3 normal;
	float4 worldPosition;
	half depth;
	half shadowMask;
	half roughness, metallic;
	UnpackGBuffersForLight(In.UV,
		albedo,
		normal,
		worldPosition,
		depth,
		shadowMask,
		roughness, metallic);

	half3 lightToPixel = (worldPosition.xyz - _LightDesc.Position.xyz);

	half distAtten = ComputeDistanceAtten(_LightDesc, worldPosition.xyz);
	half angleAtten = ComputeAngleAtten(_LightDesc, worldPosition.xyz);
	half atten = distAtten * angleAtten;
	float3 world2Light = _LightDesc.Position.xyz - worldPosition.xyz;
	float3 world2Camera = _ViewPosition.xyz - worldPosition.xyz;
	half lightIntensity;
	half3 Lo = ComputePBRLightIntensity(_LightDesc, atten, albedo, normal, world2Light, world2Camera, roughness, metallic, lightIntensity);

	half shadow = 1.0f;
	[branch]
	if (_LightDesc.DrawShadow && shadowMask > 0.0f)
	{
		shadow = ComputeShadow_Spot(_LightDesc, _LightDepthMap, _LightDesc.ShadowWhiteness, worldPosition);
		shadow = lerp(1.0f, shadow, shadowMask);
	}

	Lo *= shadow;

	output.light = half4(Lo + _LightDesc.Ambient.rgb * albedo * atten, 1.0f);

	half4 volumetric = half4(0, 0, 0, 1);
	[branch]
	if (_VolumetricDesc.DrawVolumetric)
	{
		volumetric = ComputeVolumetric_Spot(_LightDesc, _VolumetricDesc, _LightDepthMap, worldPosition.xyz, _ViewPosition.xyz);
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
