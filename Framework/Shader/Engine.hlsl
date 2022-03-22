#include "Functions.hlsl"
#include "Macros.hlsl"

/*
	- Engine.hlsl
		Engine.hlsl ������ �ݵ�� �����ؾ� �ϸ�, ����� ��� ������ ��� ���̴����� ��� �����մϴ�.
		Engine.hlsl ������ Functions.hlsl, Macros.hlsl ������ �����մϴ�.

	- ���ؽ� ���̴� �Է�
		�����ӿ�ũ���� �����ϴ� ���ؽ� ���̴� �Է��� �ޱ� ���ؼ��� ������ �ø�ƽ�� ����ؾ� �մϴ�.
		��� �ø�ƽ�� VITypes.h ���Ͽ��� Ȯ���Ҽ� �ֽ��ϴ�.

	- �ȼ� ���̴� ���
		�����ӿ�ũ���� �����ϴ� ����� ��κ� ���۵� �������Դϴ�.
		�ȼ� ���̴��� ����� DeferredRenderTarget.h ���Ͽ��� Ȯ���Ҽ� �ֽ��ϴ�.
		���� Ư�� ���� Ÿ�ٿ� ������� �������� �ܼ��� ��� ����ü���� �ش� ���� Ÿ���� �����ϸ� �˴ϴ�.

	- �н� �ɼ� ����
		�н��� ���� ������ �ɼ��� PassDesc.h ���Ͽ��� Ȯ���Ҽ� �ֽ��ϴ�.

	- ���� ť ����
		�н� ���𿡼� string RenderGroup�� �����Ҽ� �ֽ��ϴ�.
		"Priority", "Standard", "AlphaTest", "Transparent", "Overlay" �� ����Ҽ� �ֽ��ϴ�.
		�������� ���� ��쿡�� Standard�� �����˴ϴ�.

	- ���� ť ���� ����
		�н� ���𿡼� int RenderGroupOrder�� �����Ҽ� �ֽ��ϴ�.
		���� ť ���� �׸��� ������ �����Ҽ� �ֽ��ϴ�.
		�������� ���� ��쿡�� 0���� �����˴ϴ�.

	- �ν��Ͻ� ����
		�н� ���𿡼� bool Instancing�� �����Ҽ� �ֽ��ϴ�.
		Ȱ��ȭ�Ǹ� �� ���̴��� �׸��� �Ǵ� ��ü�� �������� �ν��Ͻ� �մϴ�.
		��Ű�� ���� �ʴ� ��ü���� ����Ҽ� �ֽ��ϴ�.
		�������� ���� ��쿡�� false�� �����˴ϴ�.

		�ν��Ͻ̿����� ���� ��� ��ſ� �Ʒ��� �ø�ƽ���� ����մϴ�.
		float4 Right : INSTANCE_RIGHT;
		float4 Up : INSTANCE_UP;
		float4 Forward : INSTANCE_FORWARD;
		float4 Instance_Position : INSTANCE_POSITION;

	- �׸��� ����
		�н� ���𿡼� bool DrawShadow�� �����Ҽ� �ֽ��ϴ�.
		Ȱ��ȭ�Ǹ� �� ��ü�� �׸��ڸ� �����մϴ�.
		�������� ���� ��쿡�� Standard: true, Other: false�� �����˴ϴ�.

	- ������ �ִ� �ؽ��ĸ� ������ ���� �����ϴ� �׸��� ����
		�н� ���𿡼� bool ShadowCutoffEnable �׸��� float ShadowCutoffAlpha�� �����Ҽ� �ֽ��ϴ�.
		ShadowCutoffEnable: Ȱ��ȭ�Ǹ� ShadowCutoffAlpha ���� ���� DiffuseTexture�� ���Ŀ� ���ؼ� �����׽�Ʈ�� �����մϴ�.
		ShadowCutoffAlpha: Standard ���� ť�϶� 1.0, �� �̿��� ��쿡�� 0�� �⺻���Դϴ�.
		�� ������ ���� ������ ������ �ȼ��� �׸��ڸ� �������� �ʽ��ϴ�.

	- Transparent �� Overlay ���� �׷쿡�� ���� �� �׸��� ����
		�н� ���𿡼� string TransparentLightMode�� �����Ҽ� �ֽ��ϴ�.
		"None", "Use" �� ����Ҽ� �ֽ��ϴ�.
		"None": ���� �� �׸��ڸ� �������� �ʽ��ϴ�.
		"Use": ���� �� �׸��ڸ� �����մϴ�.

*/

// From Vertex

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
	float		_Near : ENGINE;
	float		_Far : ENGINE;
	float2		_CameraCBufferUnused[2] : ENGINE;
};

cbuffer BoneMatricesUsageCBuffer
{
	uint4		_BoneMatricesUsage : ENGINE;
};

cbuffer BoneMatricesCBuffer
{
	float4x4	_BoneMatrices[512] : ENGINE;
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