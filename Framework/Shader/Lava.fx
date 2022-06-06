#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
	float4 ProjPosition : POSITION1;
	float4 ViewPosition : POSITION2;
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

float4			_MinColor < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
float4			_MaxColor < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
texture2D		_DistortionTexture < string Default = "White"; > ;
float			_DistortionScale < float Default = 0.3f; > ;
float			_DistortionSpeed < float Default = 2.0f; > ;
texture2D		_BaseNoiseTexture < string Default = "White"; > ;
float			_Time : TIME;
//texture2D		_DepthTexture : DEPTH_TEXTURE;
SamplerState	linearSampler
{
	AddressU = wrap;
	AddressV = wrap;
};
SamplerState	mirrorSampler
{
	AddressU = mirror;
	AddressV = mirror;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);
	float4 worldPosition = mul(position, _WorldMatrix);
	half4 vPosition = mul(worldPosition, _ViewMatrix);
	half4 outputPosition = mul(vPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UV = In.UV;
	output.Normal = In.Normal;
	output.ProjPosition = outputPosition;
	output.ViewPosition = vPosition;

	return output;
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	//half2 screenUV = In.ProjPosition.xy / In.ProjPosition.w;
	//screenUV.x = screenUV.x * 0.5f + 0.5f;
	//screenUV.y = screenUV.y * -0.5f + 0.5f;
	//half sampleDepth = _DepthTexture.Sample(linearSampler, screenUV).r;
	//half3 sampleVPosition = ToViewSpace(screenUV, sampleDepth, Inverse(_ProjectionMatrix));

	//half vDepth = In.ViewPosition.z;
	//half deltaVDepth = saturate(sampleVPosition.z - vDepth);

	//half deltaLength = 0.5f;
	//if (deltaVDepth > deltaLength)
	//	deltaVDepth = 0.0f;
	//else
	//	deltaVDepth = (1.0f - deltaVDepth / deltaLength);

	half4 distortion = _DistortionTexture.Sample(linearSampler, In.UV);
	half adjustUV = distortion.r * _DistortionScale;
	adjustUV *= _Time * _DistortionSpeed;

	half4 color = _BaseNoiseTexture.Sample(mirrorSampler, In.UV + adjustUV);
	color *= lerp(_MinColor, _MaxColor, color.r);
	//color.rgb = lerp(color.rgb, half3(1.0f, 0.5f, 0.25f), deltaVDepth);

	color.a = 1.0f;
	output.Albedo = color;

	half3 normal = mul(half4(In.Normal, 0.0f), NormalizeTransformationMatrix(_WorldMatrix)).xyz;
	normal = normalize(normal);
	output.Normal = PackNormal(normal);

	half depth = In.ProjPosition.z / In.ProjPosition.w;
	output.Depth = half4(depth, depth, depth, 1.0f);

	half lightMask = 0.0f;
	half shadowMask = 1.0f;
	output.Light_Shadow = half4(lightMask, shadowMask, 0.0f, 1.0f);

	output.Roughness_Metallic = 0.0f;

	output.Emissive = 0.0f;

	half occlusionMask = 1.0f;
	half reflection = 0.0f;
	half reflectionBlur = 0.0f;
	half reflectMask = 0.0f;
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
	DepthWriteMask = true;
};

BlendState BlendState0
{
	BlendEnable[0] = false;
	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;
};

technique11 Technique0
{
	pass Pass0 < string RenderGroup = "Standard"; int RenderGroupOrder = 100000; bool DrawShadow = false; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
