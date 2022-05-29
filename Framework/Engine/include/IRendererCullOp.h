#pragma once

ENGINE_BEGIN
class ICamera;
class ENGINE_API IRendererCullOp abstract
{
public:

	virtual ~IRendererCullOp() = default;

	virtual bool CullTest(ICamera* camera) const = 0;
	virtual void OnCullPass(ICamera* camera) {}
};

ENGINE_END
