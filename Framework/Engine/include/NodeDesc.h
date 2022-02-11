#pragma once

ENGINE_BEGIN

struct ENGINE_API NodeDesc
{
public:

	TCHAR			name[MAXLEN];

	// 노드와 매칭되는 뼈가 없는 경우에는 이 플래그는 꺼져 있습니다.
	// 이때 OffsetMatrix는 단위행렬입니다.
	bool			hasBone;

	// 뼈의 로컬스페이스를 메쉬의 로컬스페이스와 일치시키기 위한 행렬입니다.
	// 합쳐진 행렬 CombinedMatrix와 곱하여 최종적인 로컬 공간의 변환을 생성합니다.
	// OffsetMatrix * CombinedMatrix 형태입니다.
	M4				offsetMatrix;

	// 이 행렬과 부모 뼈의 이 행렬을 곱하여 이 뼈가 로컬공간상에 위치하는 변환을 생성합니다.
	// 합쳐진 행렬의 이름을 CombinedMatrix 라고 부르도록 합니다.
	// 이 CombinedMatrix 이후에 다른 행렬을 곱하여 추가적인 변환을 시행합니다.
	// CombinedMatrix = TransformationMatrix * Parent::TransformationMatrix
	M4				transformationMatrix;

	uint			index;

	bool			hasParent;

	uint			parent;

	uint			depth;

	const aiNode*	AINode;
};

ENGINE_END
