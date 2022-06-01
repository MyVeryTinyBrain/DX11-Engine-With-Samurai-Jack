#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float3 UVW : TEXCOORD;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float3 UVW : TEXCOORD;
};

texture2D		_BaseTexture < string Default = "white"; > ;
texture2D		_GradientTexture < string Default = "white"; > ;
SamplerState	linearSampler
{
	AddressU = Wrap;
	AddressV = Wrap;
};
SamplerState	grabSampler
{
	AddressU = Mirror;
	AddressV = Mirror;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);
	float4 worldPosition = mul(position, _WorldMatrix);
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UVW = In.UVW;

	return output;
}

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
	half4 baseColor = _BaseTexture.Sample(linearSampler, In.UVW.xy);

	half gradient = _GradientTexture.Sample(linearSampler, In.UVW.xz);
	baseColor.a *= pow(gradient.r, 3.0f);

	half endFade = 1.0f - smoothstep(0.8f, 1.0f, In.UVW.z);
	baseColor.a *= pow(endFade, 1.0f);

	return baseColor;
}

RasterizerState RasterizerState0
{
	FillMode = Solid;
	Cullmode = None;
};

DepthStencilState DepthStencilState0
{
	DepthEnable = true;
	DepthFunc = less;
	DepthWriteMask = zero;
};

BlendState BlendState0
{
	BlendEnable[0] = true;
	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;
};

technique11 T
{
	pass P < string RenderGroup = "Transparent"; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
