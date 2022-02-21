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

struct SSAODesc
{
	float	MinZ;
	float	Radius;
};

SSAODesc		_SSAODesc;
float4			_TextureSize;
texture2D		_Diffuse;
texture2D		_Normal;
texture2D		_WorldPosition;
texture2D		_DepthLightOcclusionShadow;
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

float SSAO(float2 uv)
{
	const int NUM_KERNEL = 16;
	const float3 kernels[NUM_KERNEL] =
	{
		normalize(float3(0.2024537f, 0.841204f, 0.9060141f)),
		normalize(float3(-0.2200423f, 0.6282339f, 0.8275437f)),
		normalize(float3(0.3677659f, 0.1086345f, 0.4466777f)),
		normalize(float3(0.8775856f, 0.4617546f, 0.6427765f)),
		normalize(float3(0.7867433f, -0.141479f, 0.1567597f)),
		normalize(float3(0.4839356f, -0.8253108f, 0.1563844f)),
		normalize(float3(0.4401554f, -0.4228428f, 0.3300118f)),
		normalize(float3(0.0019193f, -0.8048455f, 0.0726584f)),
		normalize(float3(-0.7578573f, -0.5583301f, 0.2347527f)),
		normalize(float3(-0.4540417f, -0.252365f, 0.0694318f)),
		normalize(float3(-0.0483353f, -0.2527294f, 0.5924745f)),
		normalize(float3(-0.4192392f, 0.2084218f, 0.3672943f)),
		normalize(float3(-0.8433938f, 0.1451271f, 0.2202872f)),
		normalize(float3(-0.4037157f, -0.8263387f, 0.4698132f)),
		normalize(float3(-0.6657394f, 0.6298575f, 0.6342437f)),
		normalize(float3(-0.0001783f, 0.2834622f, 0.8343929f)),
	};

	float4 packedWorldPosition = _WorldPosition.Sample(textureSampler, uv);
	float3 worldPosition = UnpackWorldPosition(packedWorldPosition);
	float3 viewPosition = mul(float4(worldPosition, 1.0f), _ViewMatrix).xyz;

	float4 packedNormal = _Normal.Sample(textureSampler, uv);
	float3 normal = UnpackNormal(packedNormal);

	float4 packedDepthLightOcclusionShadow = _DepthLightOcclusionShadow.Sample(textureSampler, uv);
	float occlusionMask = packedDepthLightOcclusionShadow.z;

	float3 rvec = RandomVector(uv);
	float3 tangent = normalize(rvec - normal * dot(rvec, normal));
	float3 bitangent = cross(normal, tangent);
	float3x3 tbn = float3x3(tangent, bitangent, normal);

	float occlusion = 0.0f;
	[unroll]
	for (int i = 0; i < NUM_KERNEL; ++i)
	{
		float3 dir = mul(kernels[i], tbn);
		dir = mul(float4(dir, 0.0f), _ViewMatrix).xyz;
		float3 viewSamplePos = viewPosition + dir * _SSAODesc.Radius * Random(uv);

		float4 offset = float4(viewSamplePos, 1.0f);
		offset = mul(offset, _ProjectionMatrix);
		
		float2 sampleUV = float2(offset.xy / offset.w) * float2(1.0f, -1.0f);
		sampleUV = sampleUV * 0.5f + 0.5f;

		[flatten]
		if (saturate(sampleUV.x) != sampleUV.x || saturate(sampleUV.y) != sampleUV.y)
			continue;

		float4 samplePackedWorldPosition = _WorldPosition.Sample(textureSampler, sampleUV);
		float3 sampleWorldPosition = UnpackWorldPosition(samplePackedWorldPosition);
		float3 sampleViewPosition = mul(float4(sampleWorldPosition, 1.0f), _ViewMatrix).xyz;
		float sampleDepth = sampleViewPosition.z;

		float rangeCheck = abs(viewPosition.z - sampleDepth) < _SSAODesc.Radius ? 1.0f : 0.0f;

		// Error guard
		[flatten]
		if (samplePackedWorldPosition.a == 0)
			continue;

		occlusion += (sampleDepth <= viewSamplePos.z - 0.03f ? 1.0 : 0.0) * rangeCheck;
	}

	return 1.0f - (occlusion / NUM_KERNEL) * occlusionMask;
}

float4 PS_MAIN_SSAO(PS_IN In) : SV_TARGET
{
	float occlusion = SSAO(In.uv);
	return float4(occlusion, occlusion, occlusion, 1.0f);
	//return float4(0, 0, 0, occlusion);
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
	BlendEnable[0] = false;
};

BlendState BlendState1
{
	BlendEnable[0] = true;
	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;
};

BlendState BlendState2
{
	BlendEnable[0] = false;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

technique11 Technique0
{
	pass SSAO
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState2, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_SSAO();
	}
}