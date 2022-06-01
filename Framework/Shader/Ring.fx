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
	float4 ProjPosition : TEXCOORD1;
};

float4			_Color < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
texture2D		_Texture < string Default = "White"; > ;
texture2D		_DistortionTexture < string Default = "White"; > ;
float			_DistortionPower < float Default = 100.0f; > ;
texture2D		_Grab : GRAB_TEXTURE;
SamplerState	linearSampler
{
	AddressU = wrap;
	AddressV = wrap;
};
SamplerState	grabSampler
{
	AddressU = Mirror;
	AddressV = Mirror;
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
	output.ProjPosition = outputPosition;

	return output;
}

half4 PS_MAIN(PS_IN In) : SV_TARGET
{
	half texAlpha = _Texture.Sample(linearSampler, In.UV).r;
	half4 baseColor = half4(1.0f, 1.0f, 1.0f, texAlpha) * _Color;

	half2 grabUV = In.ProjPosition.xy / In.ProjPosition.w;
	grabUV.x = grabUV.x * 0.5f + 0.5f;
	grabUV.y = grabUV.y * -0.5f + 0.5f;

	half2 distortion = (_DistortionTexture.Sample(linearSampler, In.UV.xy).r / _Size.xy) * _DistortionPower;
	half2 sampleCoord = grabUV + distortion;
	half4 grab = _Grab.Sample(grabSampler, sampleCoord);

	half4 color;
	color.rgb = grab.rgb * (1.0f - baseColor.a) + baseColor.rgb * baseColor.a;
	color.a = 1.0f;

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
	DepthWriteMask = zero;
};

BlendState BlendState0
{
	BlendEnable[0] = true;
	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;
};

technique11 Technique0
{
	pass Pass0 < string RenderGroup = "Transparent"; int RenderGroupOrder = 0; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
