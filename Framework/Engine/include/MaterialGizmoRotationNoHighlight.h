#pragma once

#include "Material.h"

ENGINE_BEGIN

class ENGINE_API  MaterialGizmoRotationNoHighlight : public Material
{
public:
	MaterialGizmoRotationNoHighlight();
	virtual ~MaterialGizmoRotationNoHighlight();
	virtual tstring GetShaderPath() const override;
	virtual void OnCreated() override;
	virtual void OnSetMaterialValues() override;
};

ENGINE_END
