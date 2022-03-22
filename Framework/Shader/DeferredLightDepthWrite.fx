#include "Engine.hlsl"

struct VS_IN_Skinned
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
	uint4  BlendIndices : BLENDINDICES;
	float4 BlendWeight : BLENDWEIGHT;
};

struct VS_IN_Instance
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

texture2D		_ShadowCutoffTexture;
float			_ShadowCutoffAlpha;
SamplerState	pointSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
	ComparisonFunc = Never;
	MinLOD = 0;
	MaxLOD = FLT_MAX;
	Filter = MIN_MAG_MIP_POINT;
};

PS_IN VS_MAIN_Instance(VS_IN_Instance In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);

	float4x4 instanceWorldMatrix;
	instanceWorldMatrix[0] = In.Right;
	instanceWorldMatrix[1] = In.Up;
	instanceWorldMatrix[2] = In.Forward;
	instanceWorldMatrix[3] = In.Instance_Position;

	float4 worldPosition = mul(position, instanceWorldMatrix);
	half4 viewPosition = mul(worldPosition, _ViewMatrix);
	half4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UV = In.UV;

	return output;
}

PS_IN VS_MAIN_Skinned(VS_IN_Skinned In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);

	if (_BoneMatricesUsage.x > 0)
	{
		matrix		boneMatrix =
			_BoneMatrices[In.BlendIndices.x] * In.BlendWeight.x +
			_BoneMatrices[In.BlendIndices.y] * In.BlendWeight.y +
			_BoneMatrices[In.BlendIndices.z] * In.BlendWeight.z +
			_BoneMatrices[In.BlendIndices.w] * In.BlendWeight.w;

		position = mul(position, boneMatrix);
	}

	float4 worldPosition = mul(position, _WorldMatrix);
	half4 viewPosition = mul(worldPosition, _ViewMatrix);
	half4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UV = In.UV;

	return output;
}

void PS_MAIN(PS_IN In)
{
	// Do nothing
}

void PS_MAIN_AlphaTest(PS_IN In)
{
	float alpha = _ShadowCutoffTexture.Sample(pointSampler, In.UV).a;
	if (alpha < _ShadowCutoffAlpha)
		discard;
}

RasterizerState RasterizerState0
{
	FillMode = Solid;
	Cullmode = Back;
	DepthClipEnable = TRUE;
	DepthBias = 300; // DepthBias = 300 + 100 * ShadowSmoothness
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
