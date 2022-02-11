#pragma once

#include "RenderGroup.h"

ENGINE_BEGIN
class IMaterial;
class ENGINE_API ScreenRenderRequest
{
private:

	M4 m_worldMatrix = M4::identity();

	ScreenRenderGroup m_renderGroup = ScreenRenderGroup::Priority;

	int m_renderGroupOrder = 0;

	IMaterial* m_material = nullptr;

	size_t m_techniqueIndex = 0;

	size_t m_passIndex = 0;
};

ENGINE_END
