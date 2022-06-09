#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
};

float4 _Color < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;

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
	return _Color;
}

RasterizerState CullOffRasterizerState
{
	FillMode = Solid;
	Cullmode = None;
};

DepthStencilState NoDepthWriteDepthStencilState
{
	DepthEnable = true;
	DepthFunc = less;
	DepthWriteMask = zero;
};

DepthStencilState NoDepthReadWriteDepthStencilState
{
	DepthEnable = false;
	DepthFunc = less;
	DepthWriteMask = zero;
};

BlendState MixBlendState
{
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

technique11 NoDepthWrite
{
	pass Pass0 < string RenderGroup = "Transparent"; >
	{
		SetRasterizerState(CullOffRasterizerState);
		SetDepthStencilState(NoDepthWriteDepthStencilState, 0);
		SetBlendState(MixBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
technique11 NoDepthReadWrite0
{
	pass Pass0 < string RenderGroup = "Transparent"; int RenderOrder = 10; >
	{
		SetRasterizerState(CullOffRasterizerState);
		SetDepthStencilState(NoDepthReadWriteDepthStencilState, 0);
		SetBlendState(MixBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
technique11 NoDepthReadWrite1
{
	pass Pass0 < string RenderGroup = "Transparent"; int RenderOrder = 11; >
	{
		SetRasterizerState(CullOffRasterizerState);
		SetDepthStencilState(NoDepthReadWriteDepthStencilState, 0);
		SetBlendState(MixBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
