#include "Engine.hlsl"

float4 color;

struct VS_IN
{
	float3 Position : POSITION;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
};

SamplerState	diffuseSampler
{
	AddressU = wrap;
	AddressV = wrap;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);
	float4 worldPosition = mul(position, _WorldMatrix);
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.Screen = outputPosition;

	return output;
}

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
	return color;
}

RasterizerState RasterizerState0
{
	FillMode = Wireframe;
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
	BlendEnable[0] = false;
};

technique11 Technique0
{
	pass Pass0 < string RenderGroup = "Overlay"; int RenderGroupOrder = 0; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
