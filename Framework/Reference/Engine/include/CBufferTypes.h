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
	// x = y = z = w 인 값입니다.
	// 이 값이 1이면 본 매트릭스들을 사용합니다.
	// 이 값이 0이면 본 매트릭스들을 사용하지 않습니다.
	uint4 _BoneMatricesUsage;
};

struct BoneMatricesCBuffer
{
	M4 _BoneMatrices[512];
};

ENGINE_END
