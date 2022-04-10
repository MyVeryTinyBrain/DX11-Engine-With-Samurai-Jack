#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float4 WorldPosition : TEXCOORD1;
};

float4			_Color < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
texture2D		_NormalMapTexture < string Default = "blue"; > ;
texture2D		_Grab : GRAB_TEXTURE;
SamplerState	linearSampler
{
	AddressU = Wrap;
	AddressV = Wrap;
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
	output.Normal = In.Normal;
	output.Tangent = In.Tangent;
	output.Binormal = In.Binormal;
	output.WorldPosition = worldPosition;

	return output;
}

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
	const static float power = 0.25f;

	float3 packedNormalMap = _NormalMapTexture.Sample(linearSampler, In.UV).rgb;
	float3 unpackedNormalMap = UnpackNormalMap(packedNormalMap, In.Normal, In.Tangent, In.Binormal);
	float4 wNormal = mul(float4(unpackedNormalMap, 0.0f), _WorldMatrix);
	float4 worldPosition = In.WorldPosition + wNormal * power;
	float4 vPosition = mul(float4(worldPosition.xyz, 1), _ViewMatrix);
	float4 ndcPosition = mul(vPosition, _ProjectionMatrix);
	ndcPosition /= ndcPosition.w;
	float2 sPosition = ndcPosition.xy * float2(1.0f, -1.0f) * 0.5f + 0.5f;

	half4 grab = _Grab.Sample(linearSampler, sPosition);
	return grab * _Color;
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
	DepthWriteMask = all;
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
