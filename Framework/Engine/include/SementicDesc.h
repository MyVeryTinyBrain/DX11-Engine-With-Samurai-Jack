#pragma once

ENGINE_BEGIN

struct SementicDesc
{
	string name = "";
	size_t size = 0;
	uint32_t index = 0;
	DXGI_FORMAT format;
	D3D_REGISTER_COMPONENT_TYPE type;
};

ENGINE_END

