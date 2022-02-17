#include "Engine.hlsl"

struct VS_IN
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

texture2D		_Diffuse;
texture2D		_DepthLightOcclusionShadow;
texture2D		_Light;
texture2D		_Specular;
SamplerState	textureSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	output.position = float4(In.position, 1.0f);
	output.uv = In.uv;

	return output;
}

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	float4 unpackedDiffuse = _Diffuse.Sample(textureSampler, In.uv);
	float4 unpackedDepthLightOcclusionShadow = _DepthLightOcclusionShadow.Sample(textureSampler, In.uv);
	float unpackedLightMask = unpackedDepthLightOcclusionShadow.y;
	float unpackedOcclusion = unpackedDepthLightOcclusionShadow.z;
	float4 unpackedLight = _Light.Sample(textureSampler, In.uv);
	float4 unpackedSpecular = _Specular.Sample(textureSampler, In.uv);

	float3 lightedColor = saturate(unpackedDiffuse.rgb * unpackedLight.rgb + unpackedSpecular.rgb);
	float3 unlighttedColor = saturate(unpackedDiffuse.rgb);

	color.rgb = lerp(unlighttedColor, lightedColor, unpackedLightMask);
	color.a = unpackedDiffuse.a;
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
