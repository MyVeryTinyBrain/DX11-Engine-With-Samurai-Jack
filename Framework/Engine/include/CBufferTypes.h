#pragma once

ENGINE_BEGIN

struct WorldMatrixCBuffer
{
	M4 _WorldMatrix;
};

struct CameraCBuffer
{
	V4 _ViewPosition;
	V4 _ViewDirection;
	M4 _ViewMatrix;
	M4 _ProjectionMatrix;
	float _Near;
	float _Far;
	float _CameraCBufferUnused[2];
};

struct BoneMatricesUsageCBuffer
{
	// x = y = z = w �� ���Դϴ�.
	// �� ���� 1�̸� �� ��Ʈ�������� ����մϴ�.
	// �� ���� 0�̸� �� ��Ʈ�������� ������� �ʽ��ϴ�.
	uint4 _BoneMatricesUsage;
};

struct BoneMatricesCBuffer
{
	M4 _BoneMatrices[512];
};

ENGINE_END
