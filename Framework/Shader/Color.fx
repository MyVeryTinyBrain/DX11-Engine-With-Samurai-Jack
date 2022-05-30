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
RasterizerState WireframeRasterizerState
{
	FillMode = Wireframe;
	Cullmode = None;
};

DepthStencilState DefaultDepthStencilState
{
	DepthEnable = true;
	DepthFunc = less;
	DepthWriteMask = all;
};

DepthStencilState NoWriteDepthStencilState
{
	DepthEnable = false;
	DepthFunc = less;
	DepthWriteMask = all;
};

BlendState DefaultBlendState
{
	BlendEnable[0] = false;
};
BlendState MixBlendState
{
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

technique11 Default
{
	pass Pass0 < string RenderGroup = "Transparent";>
	{
		SetRasterizerState(CullOffRasterizerState);
		SetDepthStencilState(DefaultDepthStencilState, 0);
		SetBlendState(MixBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
technique11 Object
{
	pass Pass0 < string RenderGroup = "Overlay"; int RenderGroupOrder = 65532; >
	{
		SetRasterizerState(CullOffRasterizerState);
		SetDepthStencilState(NoWriteDepthStencilState, 0);
		SetBlendState(MixBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
technique11 NextObject
{
	pass Pass0 < string RenderGroup = "Overlay"; int RenderGroupOrder = 65533; >
	{
		SetRasterizerState(CullOffRasterizerState);
		SetDepthStencilState(NoWriteDepthStencilState, 0);
		SetBlendState(MixBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
technique11 Wireframe
{
	pass Pass0 < string RenderGroup = "Overlay"; int RenderGroupOrder = 65534; >
	{
		SetRasterizerState(WireframeRasterizerState);
		SetDepthStencilState(NoWriteDepthStencilState, 0);
		SetBlendState(MixBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
technique11 NextWireframe
{
	pass Pass0 < string RenderGroup = "Overlay"; int RenderGroupOrder = 65535; >
	{
		SetRasterizerState(WireframeRasterizerState);
		SetDepthStencilState(NoWriteDepthStencilState, 0);
		SetBlendState(MixBlendState, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
