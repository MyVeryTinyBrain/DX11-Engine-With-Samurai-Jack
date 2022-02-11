#pragma once

ENGINE_BEGIN

struct ENGINE_API NodeDesc
{
public:

	TCHAR			name[MAXLEN];

	// ���� ��Ī�Ǵ� ���� ���� ��쿡�� �� �÷��״� ���� �ֽ��ϴ�.
	// �̶� OffsetMatrix�� ��������Դϴ�.
	bool			hasBone;

	// ���� ���ý����̽��� �޽��� ���ý����̽��� ��ġ��Ű�� ���� ����Դϴ�.
	// ������ ��� CombinedMatrix�� ���Ͽ� �������� ���� ������ ��ȯ�� �����մϴ�.
	// OffsetMatrix * CombinedMatrix �����Դϴ�.
	M4				offsetMatrix;

	// �� ��İ� �θ� ���� �� ����� ���Ͽ� �� ���� ���ð����� ��ġ�ϴ� ��ȯ�� �����մϴ�.
	// ������ ����� �̸��� CombinedMatrix ��� �θ����� �մϴ�.
	// �� CombinedMatrix ���Ŀ� �ٸ� ����� ���Ͽ� �߰����� ��ȯ�� �����մϴ�.
	// CombinedMatrix = TransformationMatrix * Parent::TransformationMatrix
	M4				transformationMatrix;

	uint			index;

	bool			hasParent;

	uint			parent;

	uint			depth;

	const aiNode*	AINode;
};

ENGINE_END
