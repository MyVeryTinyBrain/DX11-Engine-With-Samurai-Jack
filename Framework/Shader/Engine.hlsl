#include "Functions.hlsl"
#include "Macros.hlsl"

/*
	- Engine.hlsl
		Engine.hlsl ������ �ݵ�� �����ؾ� �ϸ�, ����� ��� ������ ��� ���̴����� ��� �����մϴ�.
		Engine.hlsl ������ Functions.hlsl, Macros.hlsl ������ �����մϴ�.

	- ��ũ�а� �н�
		��ũ�� �ε����� �����ϸ� ������Ʈ�� �׸��� �ش� ��ũ���� ��� �н��� �̿��Ͽ� �н��� ���ǵ� ������� �׸��� �˴ϴ�.
		���� �н��� 3����� �ش� ������Ʈ�� 3�� �׸��ϴ�.

	- ���ؽ� ���̴� �Է�
		�����ӿ�ũ���� �����ϴ� ���ؽ� ���̴� �Է��� �ޱ� ���ؼ��� ������ �ø�ƽ�� ����ؾ� �մϴ�.
		�̶� �ø�ƽ�� ��ҹ��ڸ� �����մϴ�.
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
		�н� ���𿡼� bool ShadowPass�� �����Ҽ� �ֽ��ϴ�.
		Ȱ��ȭ�Ǹ� �� �н��� �׸��ڸ� �׸��� ���˴ϴ�.
		�̶� �ȼ� ���̴��� ��ȯ���� void ���¸� ����մϴ�.
		���� Engine.hlsl�� ���ǵ� ShadowRasterizerState Ȥ�� �̿� ����� ������ �����Ͷ����� ���¸� ����ϴ� ���� �����ϴ�.
		�������� ���� ��쿡�� false�� �����˴ϴ�.

	- ������ �⺻�� ����
		scala, float4, Texture2D �� ���� �������� �⺻���� �����Ҽ� �ֽ��ϴ�.
		��Į�� bool, int, uint, float, double �� ���� �������� ��Į�� ������ �⺻����,
		float4 �������� float4 ������ �⺻����
		Texture2D �������� string ������ �⺻���� �����ϴ�.
		Texture2D �� ������ �ִ� �⺻���� ["white", "black", "red", "green", "blue", "clear"] �Դϴ�.

		float �⺻�� ����) float _Value < float Default = 5.0f ; > ;
		float4 �⺻�� ����) float4 _Value < float4 Default = float4(1, 2, 3, 4) ; > ;
		Texture2D �⺻�� ����) Texture2D _Value < string Default = "white" ; > ;

		Material.cpp���� ���޵Ǵ� ���¸� Ȯ���Ҽ� �ֽ��ϴ�.

	- ������ ������ �ø�ƽ
		Type VariableName : SPECIFIC_SEMANTIC; ���·� Ư���� ���� ������ ���޹����� �ֽ��ϴ�.
		�̶� �ø�ƽ�� ��ҹ��ڸ� �����մϴ�.
		������ �ø�ƽ���� ������ �����ϴ�.

		TIME: Ŭ���̾�Ʈ�� �����ð�
		UNSCALED_TIME: Ÿ�ӽ������� ������� ���� Ŭ���̾�Ʈ�� �����ð�
		DELTA_TIME: �̹� �����ӿ����� ��ŸŸ��
		FIXED_DELTA_TIME: �̹� �����ӿ����� ���� ��ŸŸ��
		GRAB_TEXTURE: ȭ�� ��ü�� ĸ���� �ؽ���
		DEPTH_TEXTURE: ���� �ؽ���

		Material.cpp���� Ư���� �ø�ƽ�� Ȯ���Ҽ� �ֽ��ϴ�.
*/

// ���� ������

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