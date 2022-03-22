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

texture2D		_Diffuse;
texture2D		_Depth_Light_Occlusion_Shadow;
texture2D		_Light;
texture2D		_Specular;
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

	half4 diffuse = _Diffuse.Sample(pointSampler, In.UV);
	half4 depthLightOcclusionShadow = _Depth_Light_Occlusion_Shadow.Sample(pointSampler, In.UV);
	half lightMask = depthLightOcclusionShadow.g;
	half occlusion = depthLightOcclusionShadow.b;
	half4 light = _Light.Sample(pointSampler, In.UV);
	half4 specular = _Specular.Sample(pointSampler, In.UV);

	half3 lightedColor = saturate(diffuse.rgb * light.rgb + specular.rgb);
	half3 unlighttedColor = saturate(diffuse.rgb);

	color.rgb = lerp(unlighttedColor, lightedColor, lightMask);
	color.a = diffuse.a;
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

BlendState BlendState0
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
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
