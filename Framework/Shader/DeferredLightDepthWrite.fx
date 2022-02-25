#include "Engine.hlsl"

struct VS_IN_Skinned
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	uint4 blendIndices : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHT;
};

struct VS_IN_Instance
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float4 right : INSTANCE_RIGHT;
	float4 up : INSTANCE_UP;
	float4 forward : INSTANCE_FORWARD;
	float4 instance_position : INSTANCE_POSITION;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

texture2D		_ShadowCutoffTexture;
float			_ShadowCutoffAlpha;
SamplerState	textureSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
};

PS_IN VS_MAIN_Instance(VS_IN_Instance In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.position, 1);

	float4x4 instanceWorldMatrix;
	instanceWorldMatrix[0] = In.right;
	instanceWorldMatrix[1] = In.up;
	instanceWorldMatrix[2] = In.forward;
	instanceWorldMatrix[3] = In.instance_position;

	float4 worldPosition = mul(position, instanceWorldMatrix);
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.position = outputPosition;
	output.uv = In.uv;

	return output;
}

PS_IN VS_MAIN_Skinned(VS_IN_Skinned In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.position, 1);

	if (_BoneMatricesUsage.x > 0)
	{
		matrix		boneMatrix =
			_BoneMatrices[In.blendIndices.x] * In.blendWeight.x +
			_BoneMatrices[In.blendIndices.y] * In.blendWeight.y +
			_BoneMatrices[In.blendIndices.z] * In.blendWeight.z +
			_BoneMatrices[In.blendIndices.w] * In.blendWeight.w;

		position = mul(position, boneMatrix);
	}

	float4 worldPosition = mul(position, _WorldMatrix);
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.position = outputPosition;
	output.uv = In.uv;

	return output;
}

void PS_MAIN(PS_IN In)
{
	// Do nothing
}

void PS_MAIN_AlphaTest(PS_IN In)
{
	float alpha = _ShadowCutoffTexture.Sample(textureSampler, In.uv).a;
	if (alpha < _ShadowCutoffAlpha)
		discard;
}

RasterizerState RasterizerState0
{
	FillMode = Solid;
	Cullmode = Back;
	DepthClipEnable = TRUE;
	DepthBias = 100;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 10.0f;
};

DepthStencilState DepthStencilState0
{
	DepthEnable = true;
	DepthFunc = less;
	DepthWriteMask = all;
};

BlendState BlendState0
{
	BlendEnable[0] = false;
};

technique11 Instance
{
	pass Default
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_Instance();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass AlphaTest
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_Instance();
		PixelShader = compile ps_5_0 PS_MAIN_AlphaTest();
	}
}

technique11 Skinned
{
	pass Default
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_Skinned();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	pass AlphaTest
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_Skinned();
		PixelShader = compile ps_5_0 PS_MAIN_AlphaTest();
	}
}
