#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;

	float4 Right : INSTANCE_RIGHT;
	float4 Up : INSTANCE_UP;
	float4 Forward : INSTANCE_FORWARD;
	float4 Instance_Position : INSTANCE_POSITION;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float2 UV : TEXCOORD;
};

float4			_Color < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
texture2D		_Texture < string Default = "White"; > ;
uint			_NumSlicedX;
uint			_NumSlicedY;
uint			_IndexX;
uint			_IndexY;
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
	output.UV = In.UV;

	return output;
}

half4 PS_MAIN(PS_IN In) : SV_TARGET
{
	half uDelta = 1.0f / half(_NumSlicedX);
	half vDelta = 1.0f / half(_NumSlicedY);

	half uBeg = (half(_IndexX) / half(_NumSlicedX));
	half vBeg = (half(_IndexY) / half(_NumSlicedY));

	half u = uBeg + uDelta * In.UV.x;
	half v = vBeg + vDelta * In.UV.y;

	half4 color = _Texture.Sample(diffuseSampler, half2(u, v));
	if (color.a <= 0.0f)
		discard;

	return pow(color, 2.0f) * _Color;
}

RasterizerState RasterizerState0
{
	FillMode = Solid;
	Cullmode = Back;
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

technique11 Technique0
{
	pass Pass0 < string RenderGroup = "Transparent"; int RenderGroupOrder = 0; bool DrawShadow = false; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
