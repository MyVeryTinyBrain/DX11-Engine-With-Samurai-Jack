#include "Engine.hlsl"

struct VS_IN
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	uint4 blendIndices : BLENDINDICES;
	float4 blendWeight : BLENDWEIGHT;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL; // Local Normal
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 worldPosition : POSITION;
	float4 projPosition : POSITION1;
};

struct PS_OUT
{
	float4 diffuse : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 worldPosition : SV_TARGET2;
	float4 depthLightOcclusionShadow : SV_TARGET3;
	float4 specularPower : SV_TARGET4;
	float4 emissive : SV_TARGET5;
};

texture2D		_NormalMapTexture;
texture2D		_LightMapTexture;
texture2D		_OcclusionTexture;
texture2D		_ShadowMapTexture;
texture2D		_SpecularMapTexture;
float			_SpecularPower;
SamplerState	diffuseSampler
{
	AddressU = wrap;
	AddressV = wrap;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.position, 1);
	float4 normal = float4(In.normal, 0);

	if (_BoneMatricesUsage.x > 0)
	{
		matrix		boneMatrix =
			_BoneMatrices[In.blendIndices.x] * In.blendWeight.x +
			_BoneMatrices[In.blendIndices.y] * In.blendWeight.y +
			_BoneMatrices[In.blendIndices.z] * In.blendWeight.z +
			_BoneMatrices[In.blendIndices.w] * In.blendWeight.w;

		position = mul(position, boneMatrix);
		normal = mul(normal, boneMatrix);
	}

	float4 worldPosition = mul(position, _WorldMatrix);
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.position = outputPosition;
	output.uv = In.uv;
	output.normal = In.normal;
	output.tangent = In.tangent;
	output.binormal = In.binormal;
	output.worldPosition = worldPosition;
	output.projPosition = outputPosition;

	return output;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	output.diffuse = _DiffuseTexture.Sample(diffuseSampler, In.uv);

	float3 packedNormalMap = _NormalMapTexture.Sample(diffuseSampler, In.uv).rgb;
	float3 unpackedNormalMap = UnpackNormalMap(packedNormalMap, In.normal, In.tangent, In.binormal);
	float3 normal = mul(float4(unpackedNormalMap, 0.0f), _WorldMatrix).xyz;
	output.normal = PackNormal(normal);

	output.worldPosition = PackWorldPosition(In.worldPosition.xyz);

	float depth = In.projPosition.z / In.projPosition.w;
	float lightMask = 1.0f;
	float shadowMask = 1.0f;
	float occlusionMask = 1.0f;
	output.depthLightOcclusionShadow = float4(depth, lightMask, occlusionMask, shadowMask);

	float3 specularMask = 1.0f;
	float specularPower = 0.5f;
	output.specularPower = float4(specularMask, specularPower);

	output.emissive = float4(0, 0, 0, 0);

	return output;
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
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

technique11 Technique0
{
	pass Pass0 < string RenderGroup = "Transparent"; int RenderGroupOrder = 0; string TransparentLightMode = "Use"; bool DrawShadow = true; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
