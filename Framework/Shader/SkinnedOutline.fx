#include "Engine.hlsl"

struct VS_IN
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	uint4 blendIndices : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHT;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

SamplerState	diffuseSampler
{
	AddressU = wrap;
	AddressV = wrap;
};

PS_IN VS_MAIN0(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	matrix		boneMatrix =
		_BoneMatrices[In.blendIndices.x] * In.blendWeight.x +
		_BoneMatrices[In.blendIndices.y] * In.blendWeight.y +
		_BoneMatrices[In.blendIndices.z] * In.blendWeight.z +
		_BoneMatrices[In.blendIndices.w] * In.blendWeight.w;

	float4 position = float4(In.position, 1);

	position = mul(position, boneMatrix);

	float4 worldPosition = mul(position, _WorldMatrix);
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.position = outputPosition;
	output.uv = In.uv;
	output.normal = In.normal;

	return output;
}

PS_IN VS_MAIN1(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	matrix		boneMatrix =
		_BoneMatrices[In.blendIndices.x] * In.blendWeight.x +
		_BoneMatrices[In.blendIndices.y] * In.blendWeight.y +
		_BoneMatrices[In.blendIndices.z] * In.blendWeight.z +
		_BoneMatrices[In.blendIndices.w] * In.blendWeight.w;

	float4 position = float4(In.position, 1);

	position.xyz += In.normal * 0.02f;

	position = mul(position, boneMatrix);

	float4 worldPosition = mul(position, _WorldMatrix);

	float3 dirToThis = worldPosition - viewPosition;
	dirToThis = normalize(dirToThis);
	worldPosition.xyz += dirToThis;

	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.position = outputPosition;
	output.uv = In.uv;
	output.normal = In.normal;

	return output;
}

float4 PS_MAIN0(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	color = _DiffuseTexture.Sample(diffuseSampler, In.uv);

	clip(color.a - 0.9f);

	return color;
}

float4 PS_MAIN1(PS_IN In) : SV_TARGET
{
	return float4(1, 1, 1, 1);
}

RasterizerState RasterizerState0
{
	FillMode = Solid;
	CullMode = Back;
};

DepthStencilState DepthStencilState0
{
	DepthEnable = true;
	DepthFunc = less;
	DepthWriteMask = all;
};

RasterizerState RasterizerState1
{
	FillMode = Solid;
	Cullmode = Back;
};

DepthStencilState DepthStencilState1
{
	DepthEnable = true;
	DepthFunc = less;
	DepthWriteMask = zero;
};

BlendState BlendState0
{
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

technique11 Technique0
{
	pass Pass0 < string RenderGroup = "Standard"; int RenderGroupOrder = 0; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN0();
		PixelShader = compile ps_5_0 PS_MAIN0();
	}

	pass Pass1 < string RenderGroup = "Standard"; int RenderGroupOrder = -1; >
	{
		SetRasterizerState(RasterizerState1);
		SetDepthStencilState(DepthStencilState1, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN1();
		PixelShader = compile ps_5_0 PS_MAIN1();
	}
}
