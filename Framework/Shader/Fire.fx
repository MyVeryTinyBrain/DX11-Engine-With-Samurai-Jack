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
	float4 Elapsed : INSTANCE_DATA1;
	float4 MinColor : INSTANCE_DATA2;
	float4 MaxColor : INSTANCE_DATA3;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float2 UV : TEXCOORD;

	float4 Color : INSTANCE_DATA;
	float4 Elapsed : INSTANCE_DATA1;
	float4 MinColor : INSTANCE_DATA2;
	float4 MaxColor : INSTANCE_DATA3;
};

texture2D		_DistortionTexture < string Default = "White"; > ;
texture2D		_BaseTexture < string Default = "White"; > ;
SamplerState	mirrorSampler
{
	AddressU = mirror;
	AddressV = mirror;
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

	output.Elapsed = In.Elapsed;
	output.Color = In.Color;
	output.MinColor = In.MinColor;
	output.MaxColor = In.MaxColor;

	return output;
}

half4 PS_MAIN(PS_IN In) : SV_TARGET
{
	half2 distortionUV = (In.UV + In.Elapsed.x);
	half4 distortion = _DistortionTexture.Sample(mirrorSampler, distortionUV);
	half adjustUV = distortion.r * 0.2f;

	half4 color = _BaseTexture.Sample(mirrorSampler, In.UV + adjustUV);
	color *= lerp(In.MinColor, In.MaxColor, color.r);
	return color * In.Color;
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
