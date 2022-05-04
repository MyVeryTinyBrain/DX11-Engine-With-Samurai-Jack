#include "Functions.hlsl"
#include "Macros.hlsl"

/*
	- Engine.hlsl
		Engine.hlsl 파일을 반드시 포함해야 하며, 헤더의 모든 내용은 모든 쉐이더에서 사용 가능합니다.
		Engine.hlsl 파일은 Functions.hlsl, Macros.hlsl 파일을 포함합니다.

	- 테크닉과 패스
		테크닉 인덱스를 설정하면 오브젝트를 그릴때 해당 테크닉의 모든 패스를 이용하여 패스가 정의된 순서대로 그리게 됩니다.
		만약 패스가 3개라면 해당 오브젝트를 3번 그립니다.

	- 버텍스 쉐이더 입력
		프레임워크에서 제공하는 버텍스 쉐이더 입력을 받기 위해서는 지정된 시멘틱을 사용해야 합니다.
		이때 시맨틱은 대소문자를 구별합니다.
		모든 시멘틱은 VITypes.h 파일에서 확인할수 있습니다.

	- 픽셀 쉐이더 출력
		프레임워크에서 제공하는 기능은 대부분 디퍼드 렌더링입니다.
		픽셀 쉐이더의 출력은 DeferredRenderTarget.h 파일에서 확인할수 있습니다.
		또한 특정 렌더 타겟에 출력하지 않으려면 단순히 출력 구조체에서 해당 렌더 타겟을 제외하면 됩니다.

	- 패스 옵션 설정
		패스에 설정 가능한 옵션은 PassDesc.h 파일에서 확인할수 있습니다.

	- 렌더 큐 설정
		패스 선언에서 string RenderGroup을 설정할수 있습니다.
		"Priority", "Standard", "AlphaTest", "Transparent", "Overlay" 를 사용할수 있습니다.
		설정하지 않을 경우에는 Standard로 설정됩니다.

	- 렌더 큐 순서 설정
		패스 선언에서 int RenderGroupOrder를 설정할수 있습니다.
		렌더 큐 내의 그리기 순서를 정의할수 있습니다.
		설정하지 않을 경우에는 0으로 설정됩니다.

	- 인스턴싱 설정
		패스 선언에서 bool Instancing을 설정할수 있습니다.
		활성화되면 이 쉐이더로 그리게 되는 객체를 동적으로 인스턴싱 합니다.
		스키닝 되지 않는 객체에만 사용할수 있습니다.
		설정하지 않을 경우에는 false로 설정됩니다.

		인스턴싱에서는 월드 행렬 대신에 아래의 시멘틱들을 사용합니다.
		float4 Right : INSTANCE_RIGHT;
		float4 Up : INSTANCE_UP;
		float4 Forward : INSTANCE_FORWARD;
		float4 Instance_Position : INSTANCE_POSITION;

	- 그림자 설정
		패스 선언에서 bool DrawShadow를 설정할수 있습니다.
		활성화되면 이 객체는 그림자를 생성합니다.
		설정하지 않을 경우에는 Standard: true, Other: false로 설정됩니다.

	- 투명도가 있는 텍스쳐를 가지는 모델이 투영하는 그림자 설정
		패스 선언에서 bool ShadowPass를 설정할수 있습니다.
		활성화되면 그 패스는 그림자를 그릴때 사용됩니다.
		이때 픽셀 쉐이더의 반환값은 void 형태를 사용합니다.
		또한 Engine.hlsl에 정의된 ShadowRasterizerState 혹은 이와 비슷한 형태의 래스터라이저 상태를 사용하는 것이 좋습니다.
		설정하지 않을 경우에는 false로 설정됩니다.

	- 변수의 기본값 설정
		scala, float4, Texture2D 와 같은 변수들은 기본값을 설정할수 있습니다.
		스칼라 bool, int, uint, float, double 과 같은 변수들은 스칼라 형태의 기본값을,
		float4 변수들은 float4 형태의 기본값을
		Texture2D 변수들은 string 형태의 기본값을 가집니다.
		Texture2D 가 가질수 있는 기본값은 ["white", "black", "red", "green", "blue", "clear"] 입니다.

		float 기본값 예제) float _Value < float Default = 5.0f ; > ;
		float4 기본값 예제) float4 _Value < float4 Default = float4(1, 2, 3, 4) ; > ;
		Texture2D 기본값 예제) Texture2D _Value < string Default = "white" ; > ;

		Material.cpp에서 전달되는 형태를 확인할수 있습니다.

	- 변수의 지정된 시맨틱
		Type VariableName : SPECIFIC_SEMANTIC; 형태로 특수한 값을 변수로 전달받을수 있습니다.
		이때 시맨틱은 대소문자를 구별합니다.
		유용한 시맨틱들은 다음과 같습니다.

		TIME: 클라이언트의 누적시간
		UNSCALED_TIME: 타임스케일이 적용되지 않은 클라이언트의 누적시간
		DELTA_TIME: 이번 프레임에서의 델타타임
		FIXED_DELTA_TIME: 이번 프레임에서의 고정 델타타임
		GRAB_TEXTURE: 화면 전체를 캡쳐한 텍스쳐
		DEPTH_TEXTURE: 깊이 텍스쳐

		Material.cpp에서 특수한 시맨틱을 확인할수 있습니다.
*/

// 정점 데이터

cbuffer WorldMatrixCBuffer
{
	float4x4	_WorldMatrix : ENGINE;
};

cbuffer CameraCBuffer 
{
	float4		_ViewPosition : ENGINE;
	float4		_ViewDirection : ENGINE;
	float4x4	_ViewMatrix : ENGINE;
	float4x4	_ProjectionMatrix : ENGINE;
	uint2		_Size : ENGINE;
	float		_Near : ENGINE;
	float		_Far : ENGINE;
};

cbuffer BoneMatricesUsageCBuffer
{
	uint4		_BoneMatricesUsage : ENGINE;
};

cbuffer BoneMatricesCBuffer
{
	float4x4	_BoneMatrices[512] : ENGINE;
};

RasterizerState ShadowRasterizerState
{
	FillMode = Solid;
	Cullmode = Back;
	DepthClipEnable = TRUE;
	DepthBias = 1000; // DepthBias = 300 + 100 * ShadowSmoothness (Default: 300)
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 15.0f;
};

/*
	Stencil testOp or stenciling is among the operations on the pixels/fragments (Per-pixel operations), 
	located after the alpha testOp, 
	and before the depth testOp. 
	The stencil testOp ensures undesired pixels do not reach the depth testOp. 
	This saves processing time for the scene. 
	Similarly, the alpha testOp can prevent corresponding pixels to reach the stencil testOp.
*/