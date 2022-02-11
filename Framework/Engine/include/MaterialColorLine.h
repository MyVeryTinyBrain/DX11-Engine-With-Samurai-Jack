#pragma once

#include "Material.h"

ENGINE_BEGIN

class ENGINE_API MaterialColorLine : public Material
{
private:
	virtual tstring GetShaderPath() const override;
	virtual void OnCreated() override;
	virtual void OnSetMaterialValues() override;

public:

	Color color = Color::green();
};

ENGINE_END
