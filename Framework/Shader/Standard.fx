#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	uint4  BlendIndices : BLENDINDICES;
	float4 BlendWeight : BLENDWEIGHT;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL; // Local Normal
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float4 WorldPosition : POSITION;
	float4 ProjPosition : POSITION1;
};

struct PS_OUT
{
	float4 Albedo : SV_TARGET0; 
	float4 Normal : SV_TARGET1; 
	float4 Depth : SV_TARGET2; 
	float4 Light_Shadow : SV_TARGET3; 
	float4 Roughness_Metallic : SV_TARGET4;
	float4 Emissive : SV_TARGET5;
	float4 Occlusion_Reflection_ReflectionBlur_ReflectMask : SV_TARGET6; 
};

texture2D		_AlbedoTexture < string Default = "White"; > ;
texture2D		_NormalMapTexture < string Default = "Normal"; > ;
texture2D		_LightMaskTexture < string Default = "White"; > ;
texture2D		_ShadowMaskTexture < string Default = "White"; > ;
texture2D		_RoughnessTexture < string Default = "Roughness"; > ; // Green Channel
texture2D		_MetallicTexture < string Default = "Black"; > ; // Red Channel
texture2D		_EmissiveTexture < string Default = "Clear"; > ;
texture2D		_OcclusionTexture < string Default = "White"; > ;
texture2D		_ReflectionTexture < string Default = "Clear"; > ;
float			_ReflectionTransparency <float Default = 0.2f; > ;
float			_ReflectionBlur <float Default = 1.0f; > ;
float			_ReflectMask <float Default = 1.0f; > ;
SamplerState	diffuseSampler
{
	AddressU = wrap;
	AddressV = wrap;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);
	half4 normal = half4(In.Normal, 0);
	half4 tangent = half4(In.Tangent, 0);
	half4 binormal = half4(In.Binormal, 0);

	if (_BoneMatricesUsage.x > 0)
	{
		matrix		boneMatrix =
			_BoneMatrices[In.BlendIndices.x] * In.BlendWeight.x +
			_BoneMatrices[In.BlendIndices.y] * In.BlendWeight.y +
			_BoneMatrices[In.BlendIndices.z] * In.BlendWeight.z +
			_BoneMatrices[In.BlendIndices.w] * In.BlendWeight.w;

		position = mul(position, boneMatrix);
		normal = normalize(mul(normal, boneMatrix));
		tangent = normalize(mul(tangent, boneMatrix));
		binormal = normalize(mul(binormal, boneMatrix));
	}

	float4 worldPosition = mul(position, _WorldMatrix);
	half4 vPosition = mul(worldPosition, _ViewMatrix);
	half4 outputPosition = mul(vPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UV = In.UV;
	output.Normal.xyz = normal.xyz;
	output.Tangent.xyz = tangent.xyz;
	output.Binormal.xyz = binormal.xyz;
	output.WorldPosition = worldPosition;
	output.ProjPosition = outputPosition;

	return output;
}

half ComputeFresnel(half3 viewToPixel, half3 normal)
{
	viewToPixel = -normalize(viewToPixel);
	half f0 = 0.04f;
	half cosTheta = dot(viewToPixel, normal);
	half fresnel = f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
	return saturate(fresnel);
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	output.Albedo = _AlbedoTexture.Sample(diffuseSampler, In.UV);

	half3 packedNormalMap = _NormalMapTexture.Sample(diffuseSampler, In.UV).rgb;
	half3 unpackedNormalMap = UnpackNormalMap(packedNormalMap, In.Normal, In.Tangent, In.Binormal);
	half3 normal = mul(half4(unpackedNormalMap, 0.0f), NormalizeTransformationMatrix(_WorldMatrix)).xyz;
	normal = normalize(normal);
	output.Normal = PackNormal(normal);

	half depth = In.ProjPosition.z / In.ProjPosition.w;
	output.Depth = half4(depth, depth, depth, 1.0f);

	half lightMask = _LightMaskTexture.Sample(diffuseSampler, In.UV).r;
	half shadowMask = _ShadowMaskTexture.Sample(diffuseSampler, In.UV).r;
	output.Light_Shadow = half4(lightMask, shadowMask, 0.0f, 1.0f);

	half roughness = _RoughnessTexture.Sample(diffuseSampler, In.UV).g;
	half metallic = _MetallicTexture.Sample(diffuseSampler, In.UV).r;
	output.Roughness_Metallic = half4(roughness, metallic, 0.0f, 1.0f);

	half4 emissive = _EmissiveTexture.Sample(diffuseSampler, In.UV);
	output.Emissive = emissive;

	half occlusionMask = _OcclusionTexture.Sample(diffuseSampler, In.UV).r;
	half reflection = _ReflectionTexture.Sample(diffuseSampler, In.UV).r * (1.0f - _ReflectionTransparency);
	half reflectionBlur = _ReflectionBlur;
	half reflectMask = _ReflectMask;
	output.Occlusion_Reflection_ReflectionBlur_ReflectMask = half4(occlusionMask, reflection, reflectionBlur, reflectMask);

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
	BlendEnable[0] = false;
	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;
};

technique11 Default
{
	pass Pass0 < string RenderGroup = "Standard"; int RenderGroupOrder = 0; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
technique11 NoShadow
{
	pass Pass0 < string RenderGroup = "Standard"; int RenderGroupOrder = 0; bool DrawShadow = false; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
