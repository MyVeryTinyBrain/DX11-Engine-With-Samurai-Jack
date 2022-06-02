#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;

	float4 Right : INSTANCE_RIGHT;
	float4 Up : INSTANCE_UP;
	float4 Forward : INSTANCE_FORWARD;
	float4 Instance_Position : INSTANCE_POSITION;

	float4 Color : INSTANCE_DATA;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : INSTANCE_DATA;
};

texture2D		_SparkSpreadTexture < string Default = "White"; > ;
texture2D		_SparkDotTexture < string Default = "White"; > ;
SamplerState	diffuseSampler
{
	AddressU = wrap;
	AddressV = wrap;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);

	float4x4 instanceWorldMatrix;
	instanceWorldMatrix[0] = In.Right;
	instanceWorldMatrix[1] = In.Up;
	instanceWorldMatrix[2] = In.Forward;
	instanceWorldMatrix[3] = In.Instance_Position;

	float4 worldPosition = mul(position, instanceWorldMatrix);
	half4 vPosition = mul(worldPosition, _ViewMatrix);
	half4 outputPosition = mul(vPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UV = In.UV;
	output.Color = In.Color;

	return output;
}

half4 PS_MAIN(PS_IN In) : SV_TARGET
{
	half spreadAlpha = _SparkSpreadTexture.Sample(diffuseSampler, In.UV).r;
	half dotAlpha = _SparkDotTexture.Sample(diffuseSampler, In.UV).r;
	half4 spreadColor = spreadAlpha * In.Color;
	half4 dotColor = dotAlpha;

	half4 color;
	color.rgb = spreadColor.rgb * (1.0f - dotColor.a) + dotColor.rgb * dotColor.a;
	color.a = max(spreadColor.a, dotColor.a);
	return color;
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
	pass Pass0 < string RenderGroup = "Transparent"; int RenderGroupOrder = 0; bool DrawShadow = false; bool Instancing = true; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
