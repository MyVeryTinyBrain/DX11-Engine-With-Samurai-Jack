#include "Engine.hlsl"

struct VS_IN
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
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

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float yScale = Sin01(_Time * 10) * 0.2 + 0.8;
	float4x4 scale = {
		1, 0, 0, 0,
		0, yScale, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	float4 position = float4(In.position, 1);
	//position.xyz += In.normal;
	//position = mul(scale, position);

	float4 worldPosition = mul(position, _WorldMatrix);
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.position = outputPosition;
	output.uv = In.uv;
	output.normal = In.normal;

	return output;
}

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	color = _DiffuseTexture.Sample(diffuseSampler, In.uv);
	//color.rgb = In.normal * 0.5 + 0.5;
	//color.a =1;

	if (color.a < 0.9f)
		color.rgb = 1;

	return color;
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
	pass Pass0 < string RenderGroup = "Standard"; int RenderGroupOrder = 0; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}








//#include "Engine.hlsl"
//#include "Functions.fx"
//
//struct VS_IN
//{
//	float3 position : POSITION;
//	float2 uv : TEXCOORD;
//	float3 normal : NORMAL;
//};
//
//struct PS_IN
//{
//	float4 position : SV_POSITION;
//	float2 uv : TEXCOORD;
//	float3 normal : NORMAL;
//};
//
//float			time;
//texture2D		diffuseTexture;
//SamplerState	diffuseSampler
//{
//	AddressU = wrap;
//	AddressV = wrap;
//};
//
//PS_IN VS_MAIN(VS_IN In)
//{
//	PS_IN output = (PS_IN)0;
//
//	float yScale = NormalSin(time * 10) * 0.2 + 0.8;
//	float4x4 scale = {
//		1, 0, 0, 0,
//		0, yScale, 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1
//	};
//
//	float4 position = float4(In.position, 1);
//	//position.xyz += In.normal;
//	position = mul(scale, position);
//
//	float4 worldPosition = mul(worldMatrix, position);
//	float4 ViewPosition = mul(viewMatrix, worldPosition);
//	float4 outputPosition = mul(projectionMatrix, ViewPosition);
//
//	output.position = outputPosition;
//	output.uv = In.uv;
//	output.normal = In.normal;
//
//	return output;
//}
//
//float4 PS_MAIN(PS_IN In) : SV_TARGET
//{
//	float4 color = (float4)0;
//
//	color = diffuseTexture.Sample(diffuseSampler, In.uv);
//	//color.rgb = In.normal * 0.5 + 0.5;
//	//color.a =1;
//
//	if (color.a < 0.9f)
//		color.rgb = 1;
//
//	//if (color.a < 0.9f)
//	//	discard;
//
//	return color;
//}
//
//PS_IN VS_MAIN1(VS_IN In)
//{
//	PS_IN output = (PS_IN)0;
//
//	float yScale = NormalSin(time * 10) * 0.2 + 0.8;
//	float4x4 scale = {
//		1, 0, 0, 0,
//		0, yScale, 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1
//	};
//
//	float4 position = float4(In.position, 1);
//	position = mul(scale, position);
//
//	float4 worldPosition = mul(worldMatrix, position);
//	float4 ViewPosition = mul(viewMatrix, worldPosition);
//	float4 outputPosition = mul(projectionMatrix, ViewPosition);
//
//	output.position = outputPosition;
//	output.uv = In.uv;
//	output.normal = In.normal;
//
//	return output;
//}
//
//float4 PS_MAIN1(PS_IN In) : SV_TARGET
//{
//	float4 color = 0;
//
//	float2 scale = float2(1.1f, 1.1f);
//	float2 deltaScale = scale - 1;
//
//	uint w, h;
//	diffuseTexture.GetDimensions(w, h);
//
//	float2 uv = In.uv;
//	float2 inUV = uv;
//	float2 pixelCoord = UVToPixelCoord(w, h, inUV);
//
//	int pixelBias = 10;
//	pixelBias *= (sin(time * 10) * 0.5 + 0.5);
//
//	if (pixelBias <= 0)
//		discard;
//
//	[loop] for (int x = -pixelBias / 2; x <= pixelBias / 2; ++x)
//	{
//		[loop] for (int y = -pixelBias / 2; y <= pixelBias / 2; ++y)
//		{
//			float2 pixelUV = PixelCoordToUV(w, h, pixelCoord + float2(x, y));
//			float4 pixel = diffuseTexture.Sample(diffuseSampler, pixelUV);
//
//			if (pixel.a > 0.8f)
//			{
//				return lerp(float4(1, 1, 0, 1), float4(0, 1, 1, 1), sin(time * 5) * 0.5 + 0.5);
//			}
//		}
//	}
//
//	discard;
//	return color;
//}
//
//RasterizerState RasterizerState0
//{
//	FillMode = Solid;
//};
//
//RasterizerState RasterizerState1
//{
//	FillMode = Wireframe;
//	Cullmode = None;
//};
//
//RasterizerState RasterizerState2
//{
//	FillMode = Solid;
//	Cullmode = None;
//};
//
//DepthStencilState DepthStencilState0
//{
//	DepthEnable = true;
//	DepthFunc = less;
//	DepthWriteMask = all;
//};
//
//DepthStencilState DepthStencilState1
//{
//	DepthEnable = true;
//	DepthFunc = less;
//	DepthWriteMask = zero;
//};
//
//BlendState BlendState0
//{
//	BlendEnable[0] = true;
//	SrcBlend = Src_Alpha;
//	DestBlend = Inv_Src_Alpha;
//	BlendOp = Add;
//};
//
//technique11 Technique0
//{
//	pass Pass0 < string RenderGroup = "Standard"; int RenderGroupOrder = 0; >
//	{
//		SetRasterizerState(RasterizerState0);
//		SetDepthStencilState(DepthStencilState0, 0);
//		//SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
//		VertexShader = compile vs_5_0 VS_MAIN();
//		PixelShader = compile ps_5_0 PS_MAIN();
//	}
//	//pass Pass1 < string RenderGroup = "Standard"; int RenderGroupOrder = -1; >
//	//{
//	//	SetRasterizerState(RasterizerState2);
//	//	SetDepthStencilState(DepthStencilState1, 0);
//	//	//SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
//	//	VertexShader = compile vs_5_0 VS_MAIN1();
//	//	PixelShader = compile ps_5_0 PS_MAIN1();
//	//}
//}