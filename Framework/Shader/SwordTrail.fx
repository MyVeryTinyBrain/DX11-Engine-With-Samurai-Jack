#include "Engine.hlsl"

struct VS_IN
{
	float3 Position : POSITION;
	float3 UVW : TEXCOORD;
};

struct PS_IN
{
	float4 Screen : SV_POSITION;
	float3 UVW : TEXCOORD;
	float4 ProjPosition : TEXCOORD1;
};

float			_InvU <float Default = 0.0f; > ;
float			_LeftFade < float Default = 0.4f; > ;
float			_StartFade < float Default = 0.2f; > ;
float			_EndFade < float Default = 0.2f; > ;
float4			_MinColor < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
float4			_MaxColor < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
texture2D		_BaseTexture < string Default = "white"; > ;
texture2D		_DistortionTexture < string Default = "White"; > ;
float			_DistortionPower < float Default = 200.0f; > ;
texture2D		_GrabTexture : GRAB_TEXTURE;
SamplerState	linearSampler
{
	AddressU = Wrap;
	AddressV = Wrap;
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
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UVW = In.UVW;
	output.ProjPosition = outputPosition;

	return output;
}

float4 PS_MAIN(PS_IN In) : SV_TARGET
{
	In.UVW.x = lerp(In.UVW.x, 1.0f - In.UVW.x, _InvU);

	half4 baseTexColor = _BaseTexture.Sample(linearSampler, In.UVW.xy);

	half startFade = smoothstep(0.0f, _StartFade, In.UVW.z);
	half endFade = 1.0f - smoothstep(1.0f - _EndFade, 1.0f, In.UVW.z);
	startFade = pow(saturate(startFade), 2.0f);
	endFade = pow(saturate(endFade), 2.0f);

	half4 baseColor = lerp(_MinColor, _MaxColor, baseTexColor.r);
	baseColor.a *= baseTexColor.a * endFade * startFade;

	half2 screenUV = In.ProjPosition.xy / In.ProjPosition.w;
	screenUV.x = screenUV.x * 0.5f + 0.5f;
	screenUV.y = screenUV.y * -0.5f + 0.5f;

	half2 distortion = (_DistortionTexture.Sample(linearSampler, In.UVW.xy).r / _Size.xy) * _DistortionPower;
	half2 sampleCoord = screenUV + distortion;
	half4 grab = _GrabTexture.Sample(grabSampler, sampleCoord);

	half leftFade = smoothstep(0.0f, _LeftFade, In.UVW.x);
	leftFade = pow(saturate(leftFade), 2.0f);

	half4 color;
	color.rgb = grab.rgb * (1.0f - baseColor.a) + baseColor.rgb * baseColor.a;
	color.a = endFade * startFade * leftFade;

	return color;
}

RasterizerState RasterizerState0
{
	FillMode = Solid;
	Cullmode = None;
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

technique11 T
{
	pass P < string RenderGroup = "Transparent"; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}
