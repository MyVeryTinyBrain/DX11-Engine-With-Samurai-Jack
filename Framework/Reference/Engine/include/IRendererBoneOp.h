#pragma once

#include "CBufferTypes.h"

ENGINE_BEGIN

class ENGINE_API IRendererBoneOp abstract
{
public:

	virtual ~IRendererBoneOp() = default;

	virtual void OnSetBoneMatricesCBuffer(uint subMeshIndex, BoneMatricesCBuffer* pCBuffer) = 0;
};

ENGINE_END
