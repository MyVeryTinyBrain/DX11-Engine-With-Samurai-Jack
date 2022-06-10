#pragma once

#include "ResourceRef.h"

ENGINE_BEGIN

class Renderer;
class Shader;

namespace MaterialUtil
{
	ENGINE_API void SetupPBRMaterials(Renderer* renderer, ResourceRef<Shader> shader, uint techniqueIndex);
	ENGINE_API void SetupInstancePBRMaterials(Renderer* renderer, ResourceRef<Shader> shader, uint techniqueIndex);
};

ENGINE_END
