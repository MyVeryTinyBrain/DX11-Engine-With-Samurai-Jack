#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float3 UVW : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float3 UVW : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float4 WorldPosition : TEXCOORD1;
};

// _GradientTexture: 빨강 채널의 값을 사용합니다.
// _SideAttenFactor: 폭에 따른 감쇄에 대한 값입니다. 값이 커지면 더 빠르게 감쇄합니다.
// _EndFade: 트레일이 끊나는 곳의 감쇄에 대한 값입니다. 값이 커지면 더 이른 곳에서 감쇄합니다.

float4			_Color < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
float			_SideAttenFactor < float Default = 1.5f; > ;
float			_EndFade < float Default = 0.2f; > ;
texture2D		_GradientTexture < string Default = "white"; > ;
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
	output.UVW = In.UVW;
	output.Normal = In.Normal;
	output.Tangent = In.Tangent;
	output.Binormal = In.Binormal;
	output.WorldPosition = worldPosition;

	return output;
}

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
	half4 color = _Color;

	half4 gradient = _GradientTexture.Sample(linearSampler, In.UVW.xy);
	half sideFade = pow(abs(gradient.r), _SideAttenFactor);

	half lengthPercent = In.UVW.z;
	half endFade = 1.0f - smoothstep(1.0f - _EndFade, 1.0f, lengthPercent);

	color.a = sideFade * endFade;

	return color;
}

//float4 vPosition = mul(float4(In.WorldPosition.xyz, 1), _ViewMatrix);
//float4 ndcPosition = mul(vPosition, _ProjectionMatrix);
//ndcPosition /= ndcPosition.w;
//float2 sPosition = ndcPosition.xy * float2(1.0f, -1.0f) * 0.5f + 0.5f;
//
//half distortion = (_DistortionTexture.Sample(linearSampler, In.UVW.xy).r / _Size.x) * 10000.0f;
//half2 sampleCoord = sPosition + half2(distortion, 0.0f);
//sampleCoord = max(0.0f, sampleCoord);
//half4 grab = _Grab.Sample(linearSampler, sampleCoord);

RasterizerState RasterizerState0
{
	FillMode = Solid;
	Cullmode = None;
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
