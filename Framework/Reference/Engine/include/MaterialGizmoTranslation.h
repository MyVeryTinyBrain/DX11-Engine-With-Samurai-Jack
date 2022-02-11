#pragma once

#include "Material.h"

ENGINE_BEGIN

class ENGINE_API MaterialGizmoTranslation : public Material
{
public:
	MaterialGizmoTranslation();
	virtual ~MaterialGizmoTranslation();
	virtual tstring GetShaderPath() const override;
	virtual void OnCreated() override;
	virtual void OnSetMaterialValues() override;
};

ENGINE_END
