#pragma once

ENGINE_BEGIN
class ICamera;
class ENGINE_API IRendererCullOp abstract
{
public:
	virtual ~IRendererCullOp() = default;
	// 파라미터로 전달된 카메라에 의해 컬링될 떄 수행됩니다.
	// true를 반환하면 카메라 프러스텀 내부에 속한 것입니다.
	virtual bool CullTest(ICamera* camera) const = 0;
	// 컬링 테스트에 통과하면 실행되는 함수입니다.
	virtual void OnCullPass(ICamera* camera) {}
};

ENGINE_END
