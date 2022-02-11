#pragma once

#include "Material.h"

ENGINE_BEGIN

class ENGINE_API MaterialGizmoRotationLine : public Material
{
public:
	MaterialGizmoRotationLine();
	virtual ~MaterialGizmoRotationLine();
	virtual tstring GetShaderPath() const override;
	virtual void OnCreated() override;
	virtual void OnSetMaterialValues() override;
};

ENGINE_END
