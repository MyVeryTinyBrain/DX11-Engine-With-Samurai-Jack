#include "Engine.hlsl"

texture2D	_BRDF < string Default = "white"; > ;
texture2D	_Cubemap < string Default = "white"; > ;
float		_Metallic < float Default = 0.0f; > ;
float		_Roughness < float Default = 0.0f; > ;
float		_AmbientOcclusion < float Default = 1.0f; > ;

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
	float4 WorldPosition : POSITION;
	float4 ProjPosition : POSITION1;
};

struct PS_OUT
{
	float4 Diffuse : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Depth : SV_TARGET2;
	float4 Light_Shadow : SV_TARGET3;
	float4 Occlusion_Reflection_ReflectionBlur_ReflectMask : SV_TARGET6;
};

SamplerState	diffuseSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
};

SamplerState	brdfSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
	//ComparisonFunc = Never;
	//MinLOD = 0;
	//MaxLOD = FLT_MAX;
	//Filter = MIN_MAG_MIP_POINT;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);
	half3 normal = mul(half4(In.Normal, 0.0f), _WorldMatrix).xyz;
	normal = normalize(normal);

	float4 worldPosition = mul(position, _WorldMatrix);
	half4 vPosition = mul(worldPosition, _ViewMatrix);
	half4 outputPosition = mul(vPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UV = In.UV;
	output.Normal.xyz = normal.xyz;
	output.WorldPosition = worldPosition;
	output.ProjPosition = outputPosition;

	return output;
}

float distributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float geometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(1.0f - roughness, F0) - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT output = (PS_OUT)0;

	output.Normal = PackNormal(In.Normal);

	half depth = In.ProjPosition.z / In.ProjPosition.w;
	output.Depth = half4(depth, depth, depth, 1.0f);

	output.Light_Shadow = half4(0, 0, 0, 1);
	output.Occlusion_Reflection_ReflectionBlur_ReflectMask = half4(1, 0.5f, 0.5f, 1.0f);

	///////////////////////////////////////////////////////////////////////////////////////////////

	float3 Lo = 0.0f;

	float3 lightPosition[5] = { float3(2, 5, 0), float3(2 - 10, 5, 0), float3(2 + 10, 5, 0), float3(2, 5, 10), float3(2, 5, -10) };
	float3 lightColor[5] = { float3(1, 1, 1), float3(1, 1, 1), float3(1, 1, 1), float3(1, 1, 1), float3(1, 1, 1) };
	float lightIntensity[5] = { 1.0f ,1.0f ,1.0f ,1.0f, 1.0f };
	float lightAmbient[5] = { 1,1,1,1,1 };

	float3 albedo = float3(1, 0, 0);
	float metallic = _Metallic;
	float roughness = _Roughness;
	roughness = min(roughness, 0.995f);
	float ambientOcclusion = _AmbientOcclusion;

	float3 worldPos = In.WorldPosition.xyz;
	float3 N = normalize(In.Normal.xyz);
	float3 V = normalize(_ViewPosition.xyz - worldPos);
	float3 R = reflect(-V, N);

	//float3 F0 = lerp(0.04f, pow(albedo, 2.2f), metallic);
	float3 F0 = lerp(0.04f, albedo, metallic);

	for (int i = 0; i < 1; ++i)
	{
		float3 L = normalize(lightPosition[i] - worldPos);
		float3 H = normalize(V + L);
		float3 radiance = lightIntensity[i] * lightColor[i];

		// Cook-Torrance BRDF
		float NDF = distributionGGX(N, H, roughness);
		float G = geometrySmith(N, V, L, roughness);
		float3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

		float3 numerator = NDF * G * F;
		float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f; // + 0.0001 to prevent divide by zero
		float3 specular = numerator / denominator;

		float3 kS = F;
		float3 kD = 1.0f - kS;
		kD *= 1.0f - metallic;

		float NdotL = max(dot(N, L), 0.0f);

		Lo += (kD * albedo / PI + specular) * NdotL * radiance;
	}

	Lo = Lo / (Lo + 1.0f);
	Lo = pow(Lo, 1.0f / 2.2f);

	output.Diffuse.rgb = Lo;
	//output.Diffuse.rgb = _Cubemap.SampleLevel(diffuseSampler, uv, roughness * 10.0f).rgb;
	output.Diffuse.a = 1;

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

technique11 Technique0
{
	pass Pass0 < string RenderGroup = "Standard"; int RenderGroupOrder = 0; bool DrawShadow = true; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
