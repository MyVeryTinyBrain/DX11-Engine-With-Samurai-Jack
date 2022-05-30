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

// _Texture: 메인 알파를 표현하는 텍스쳐입니다. Red 채널을 사용합니다.
// _UVAdjust: 디퓨즈 텍스쳐의 UV값에 추가적으로 더해지는 값입니다.
// _StartFade: 시작 위치(위쪽)가 블렌딩됩니다.
// _EndFade: 끝 위치(아래쪽)가 블렌딩됩니다.

float4			_Color < float4 Default = float4(1.0f, 1.0f, 1.0f, 1.0f); > ;
texture2D		_Texture < string Default = "White"; > ;
float4			_UVAdjust < float4 Defaut = float4(0.0f, 0.0f, 0.0f, 0.0f); > ;
float			_StartFade < float Default = 0.35f; > ;
float			_StartFadePower < float Default = 5.0f; > ;
float			_EndFade < float Default = 0.0f; > ;
float			_EndFadePower < float Default = 1.0f; > ;
SamplerState	diffuseSampler
{
	AddressU = wrap;
	AddressV = wrap;
};

PS_IN VS_MAIN(VS_IN In)
{
	PS_IN output = (PS_IN)0;

	float4 position = float4(In.Position, 1);
	float4 worldPosition = mul(position, _WorldMatrix);
	float4 viewPosition = mul(worldPosition, _ViewMatrix);
	float4 outputPosition = mul(viewPosition, _ProjectionMatrix);

	output.Screen = outputPosition;
	output.UV = In.UV;

	return output;
}

half4 PS_MAIN(PS_IN In) : SV_TARGET
{
	half4 texColor = _Texture.Sample(diffuseSampler, In.UV + _UVAdjust.xy);
	half4 color = texColor * _Color;

	half begFade = pow(saturate(smoothstep(0.0f, _StartFade, In.UV.y)), _StartFadePower);
	half endFade = pow(saturate(1.0f - smoothstep(1.0 - _EndFade, 1.0f, In.UV.y)), _EndFadePower);
	half fade = begFade * endFade;

	color.a *= texColor.r;
	color.a = pow(abs(color.a), 2.0f) * fade;

	color.rgb *= (1.0f - smoothstep(0.9f, 1.0f, texColor.r) * 0.25f);

	return color;
}

RasterizerState RasterizerState0
{
	FillMode = Solid;
	Cullmode = Back;
};

RasterizerState RasterizerState1
{
	FillMode = Solid;
	Cullmode = Front;
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
	pass Pass0 < string RenderGroup = "Transparent"; int RenderGroupOrder = 0; bool DrawShadow = false; >
	{
		SetRasterizerState(RasterizerState0);
		SetDepthStencilState(DepthStencilState0, 0);
		SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	//pass Pass1 < string RenderGroup = "Transparent"; int RenderGroupOrder = 0; bool DrawShadow = false; >
	//{
	//	SetRasterizerState(RasterizerState1);
	//	SetDepthStencilState(DepthStencilState0, 0);
	//	SetBlendState(BlendState0, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	PixelShader = compile ps_5_0 PS_MAIN();
	//}
}
