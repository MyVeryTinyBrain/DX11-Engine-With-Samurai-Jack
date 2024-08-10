#pragma once

ENGINE_BEGIN
class ICamera;
class ENGINE_API IRendererCullOp abstract
{
public:
	virtual ~IRendererCullOp() = default;
	// �Ķ���ͷ� ���޵� ī�޶� ���� �ø��� �� ����˴ϴ�.
	// true�� ��ȯ�ϸ� ī�޶� �������� ���ο� ���� ���Դϴ�.
	virtual bool CullTest(ICamera* camera) const = 0;
	// �ø� �׽�Ʈ�� ����ϸ� ����Ǵ� �Լ��Դϴ�.
	virtual void OnCullPass(ICamera* camera) {}
};

ENGINE_END
