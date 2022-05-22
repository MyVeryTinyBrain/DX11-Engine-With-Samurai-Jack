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

texture2D		_Albedo;
texture2D		_Depth;
texture2D		_Light_Shadow; //_Light_Occlusion_Shadow
texture2D		_Light;
texture2D		_Volumetric;
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

	output.Screen = float4(In.Position, 1.0f);
	output.UV = In.UV;

	return output;
}

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	half4 albedo = _Albedo.Sample(pointSampler, In.UV);
	half4 ls = _Light_Shadow.Sample(pointSampler, In.UV);
	half lightMask = ls.r;
	half4 light = _Light.Sample(pointSampler, In.UV);

	half3 lightedColor = saturate(light.rgb);
	// HDR tonemapping
	lightedColor = lightedColor / (lightedColor + 1.0f);
	// Gamma correction
	lightedColor = pow(lightedColor, 1.0f / 2.2f);

	half3 unlighttedColor = saturate(albedo.rgb);

	color.rgb = lerp(unlighttedColor, lightedColor, lightMask);
	//color.rgb = min(color.rgb, half3(1, 1, 1));
	color.a = albedo.a;

	return color;
}

float4 PS_MAIN_Volumetric(PS_IN In) : SV_TARGET
{
	half4 color = _Volumetric.Sample(pointSampler, In.UV);
	return color;
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

BlendState BlendState_None
{
	BlendEnable[0] = false;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
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

technique11 Technique0
{
	pass Blend
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass Volumetric
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Volumetric();
	}
}
