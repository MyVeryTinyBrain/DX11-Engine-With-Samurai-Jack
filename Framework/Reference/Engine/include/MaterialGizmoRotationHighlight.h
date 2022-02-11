#pragma once

#include "Material.h"

ENGINE_BEGIN

class ENGINE_API MaterialGizmoRotationHighlight : public Material
{
public:
	MaterialGizmoRotationHighlight();
	virtual ~MaterialGizmoRotationHighlight();
	virtual tstring GetShaderPath() const override;
	virtual void OnCreated() override;
	virtual void OnSetMaterialValues() override;
};

ENGINE_END
