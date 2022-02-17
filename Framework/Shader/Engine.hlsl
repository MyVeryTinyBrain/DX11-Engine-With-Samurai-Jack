#include "Functions.hlsl"
#include "Macros.hlsl"

/*
	- Engine.hlsl
		Engine.hlsl 파일을 반드시 포함해야 하며, 헤더의 모든 내용은 모든 쉐이더에서 사용 가능합니다.
		Engine.hlsl 파일은 Functions.hlsl, Macros.hlsl 파일을 포함합니다.

	- 버텍스 쉐이더 입력
		프레임워크에서 제공하는 버텍스 쉐이더 입력을 받기 위해서는 지정된 시멘틱을 사용해야 합니다.
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
		float4 right : INSTANCE_RIGHT;
		float4 up : INSTANCE_UP;
		float4 forward : INSTANCE_FORWARD;
		float4 instance_position : INSTANCE_POSITION;

	- 그림자 설정
		패스 선언에서 bool DrawShadow를 설정할수 있습니다.
		활성화되면 이 객체는 그림자를 생성합니다.
		설정하지 않을 경우에는 Standard: true, Other: false로 설정됩니다.

	- 투명도가 있는 텍스쳐를 가지는 모델이 투영하는 그림자 설정
		패스 선언에서 float ShadowCutoffAlpha를 설정할수 있습니다.
		Standard 렌더 큐일때 1.0, 그 이외의 경우에는 0이 기본값입니다.
		이 값보다 작은 투명도를 가지는 픽셀은 그림자를 투영하지 않습니다.

	- Transparent 및 Overlay 렌더 그룹에서 조명 및 그림자 적용
		패스 선언에서 string TransparentLightMode를 설정할수 있습니다.
		"None", "Use", "UseAndApplyGBuffer" 를 사용할수 있습니다.
		"None": 조명 및 그림자를 적용하지 않습니다.
		"Use": 조명 및 그림자를 적용합니다.
		"UseAndApplyGBuffer": 조명 및 그림자를 적용하고, G버퍼의 내용을 적용합니다.

*/

// From Vertex

cbuffer WorldMatrixCBuffer
{
	float4x4	_WorldMatrix;
};

cbuffer CameraCBuffer 
{
	float4		_ViewPosition;
	float4		_ViewDirection;
	float4x4	_ViewMatrix;
	float4x4	_ProjectionMatrix;
	float		_Near;
	float		_Far;
	float2		_CameraCBufferUnused[2];
};

cbuffer BoneMatricesUsageCBuffer
{
	uint4		_BoneMatricesUsage;
};

cbuffer BoneMatricesCBuffer
{
	float4x4	_BoneMatrices[512];
};

// From material

float			_Time;
float			_UnscaledTime;
texture2D		_DiffuseTexture;
texture2D		_NormalTexture;

/*
	Stencil testOp or stenciling is among the operations on the pixels/fragments (Per-pixel operations), 
	located after the alpha testOp, 
	and before the depth testOp. 
	The stencil testOp ensures undesired pixels do not reach the depth testOp. 
	This saves processing time for the scene. 
	Similarly, the alpha testOp can prevent corresponding pixels to reach the stencil testOp.
*/