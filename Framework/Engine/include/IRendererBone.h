#pragma once

#include "CBufferTypes.h"

ENGINE_BEGIN

class ENGINE_API IRendererBone abstract
{
public:

	virtual ~IRendererBone() = default;

	virtual void OnSetBoneMatricesCBuffer(uint subMeshIndex, BoneMatricesCBuffer* pCBuffer) = 0;
};

ENGINE_END
