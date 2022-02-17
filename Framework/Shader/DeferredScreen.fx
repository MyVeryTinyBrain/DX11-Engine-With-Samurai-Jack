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
uint			_Blur;
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

float4 PS_MAIN_NonBlend(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	color = _Diffuse.Sample(textureSampler, In.uv);
	if (color.a < 1.0f)
		color = float4(color.xyz, 1.0f);

	return color;
}

float4 PS_MAIN_Blend(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	color = _Diffuse.Sample(textureSampler, In.uv);

	return color;
}

float4 PS_MAIN_AlphaTest(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	color = _Diffuse.Sample(textureSampler, In.uv);

	clip(color.a - 0.99f);

	return color;
}

float4 PS_MAIN_PerspectiveDepthVisualize(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	color = _Diffuse.Sample(textureSampler, In.uv);

	const float const_near = 0.1f;
	const float const_far = 50.0f;
	float linearDepth = ToLinearDepth(const_near, const_far, color.r);
	return float4(linearDepth, linearDepth, linearDepth, 1.0f);
}

float4 PS_MAIN_OrthoDepthVisualize(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	color = _Diffuse.Sample(textureSampler, In.uv);
	float depth = pow(abs(color), 1.5f).r;
	return float4(depth, depth, depth, 1.0f);
}

float4 PS_MAIN_HorizontalBlur(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	float width, height;
	_Diffuse.GetDimensions(width, height);
	float2 pixelCoord = UVToPixelCoord(width, height, In.uv);

	float4 horizontalColor = _Diffuse.Sample(textureSampler, In.uv);
	for (uint i = 0; i < _Blur; ++i)
	{
		horizontalColor += _Diffuse.Sample(textureSampler, PixelCoordToUV(width, height, pixelCoord - float2(i, 0)));
		horizontalColor += _Diffuse.Sample(textureSampler, PixelCoordToUV(width, height, pixelCoord + float2(i, 0)));
	}
	horizontalColor /= (1 + _Blur * 2);

	return horizontalColor;
}

float4 PS_MAIN_VerticalBlur(PS_IN In) : SV_TARGET
{
	float4 color = (float4)0;

	float width, height;
	_Diffuse.GetDimensions(width, height);
	float2 pixelCoord = UVToPixelCoord(width, height, In.uv);

	float4 verticalColor = _Diffuse.Sample(textureSampler, In.uv);
	for (uint j = 0; j < _Blur; ++j)
	{
		verticalColor += _Diffuse.Sample(textureSampler, PixelCoordToUV(width, height, pixelCoord - float2(0, j)));
		verticalColor += _Diffuse.Sample(textureSampler, PixelCoordToUV(width, height, pixelCoord + float2(0, j)));
	}
	verticalColor /= (1 + _Blur * 2);

	return verticalColor;
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
	BlendEnable[0] = true;
	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

technique11 Technique0
{
	pass NonBlend
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_NonBlend();
	}
	pass Blend
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState2, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_Blend();
	}
	pass AlphaTest
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_AlphaTest();
	}
	pass PerspectiveDepthVisualize
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_PerspectiveDepthVisualize();
	}
	pass OrthoDepthVisualize
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_OrthoDepthVisualize();
	}
	pass HorizontalBlur
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_HorizontalBlur();
	}
	pass VerticalBlur
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_VerticalBlur();
	}
}
