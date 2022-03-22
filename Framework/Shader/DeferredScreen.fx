#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float2 UV : TEXCOORD;
};

texture2D		_Diffuse;
uint			_Blur;
SamplerState	pointSampler
{
	AddressU = Clamp;
	AddressV = Clamp;
	ComparisonFunc = Never;
	MinLOD = 0;
	MaxLOD = FLT_MAX;
	Filter = MIN_MAG_MIP_POINT;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	output.Screen = float4(In.Position, 1.0f);
	output.UV = In.UV;

	return output;
}

float4 PS_MAIN_NonBlend(PS_IN In) : SV_TARGET
{
	half4 color = (half4)0;

	color = _Diffuse.Sample(pointSampler, In.UV);
	if (color.a < 1.0f)
		color = half4(color.xyz, 1.0f);

	return color;
}

float4 PS_MAIN_Blend(PS_IN In) : SV_TARGET
{
	half4 color = (half4)0;

	color = _Diffuse.Sample(pointSampler, In.UV);

	return color;
}

float4 PS_MAIN_AlphaTest(PS_IN In) : SV_TARGET
{
	half4 color = (half4)0;

	color = _Diffuse.Sample(pointSampler, In.UV);

	clip(color.a - 0.99f);

	return color;
}

float4 PS_MAIN_PerspectiveDepthVisualize(PS_IN In) : SV_TARGET
{
	half4 color = (half4)0;

	color = _Diffuse.Sample(pointSampler, In.UV);

	const half const_near = 0.1f;
	const half const_far = 50.0f;
	float linearDepth = ToLinearDepth(const_near, const_far, color.r);
	return half4(linearDepth, linearDepth, linearDepth, 1.0f);
}

float4 PS_MAIN_OrthoDepthVisualize(PS_IN In) : SV_TARGET
{
	half4 color = (half4)0;

	color = _Diffuse.Sample(pointSampler, In.UV);
	half depth = pow(abs(color), 1.5f).r;
	return half4(depth, depth, depth, 1.0f);
}

float4 PS_MAIN_HorizontalBlur(PS_IN In) : SV_TARGET
{
	half4 color = (half4)0;

	float width, height;
	_Diffuse.GetDimensions(width, height);
	half2 pixelCoord = UVToTexPixelCoord(width, height, In.UV);

	half4 horizontalColor = _Diffuse.Sample(pointSampler, In.UV);
	for (uint i = 0; i < _Blur; ++i)
	{
		horizontalColor += _Diffuse.Sample(pointSampler, TexPixelCoordToUV(width, height, pixelCoord - half2(i, 0)));
		horizontalColor += _Diffuse.Sample(pointSampler, TexPixelCoordToUV(width, height, pixelCoord + half2(i, 0)));
	}
	horizontalColor /= (1 + _Blur * 2);

	return horizontalColor;
}

float4 PS_MAIN_VerticalBlur(PS_IN In) : SV_TARGET
{
	half4 color = (half4)0;

	half width, height;
	_Diffuse.GetDimensions(width, height);
	half2 pixelCoord = UVToTexPixelCoord(width, height, In.UV);

	float4 verticalColor = _Diffuse.Sample(pointSampler, In.UV);
	for (uint j = 0; j < _Blur; ++j)
	{
		verticalColor += _Diffuse.Sample(pointSampler, TexPixelCoordToUV(width, height, pixelCoord - half2(0, j)));
		verticalColor += _Diffuse.Sample(pointSampler, TexPixelCoordToUV(width, height, pixelCoord + half2(0, j)));
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
