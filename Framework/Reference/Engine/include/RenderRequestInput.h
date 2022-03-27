#pragma once

#include "ShaderTypes.h"
#include "IMaterial.h"
#include "IMesh.h"
#include "ICamera.h"
#include "ITexture.h"
#include "IRendererBoundsOp.h"
#include "IRendererCullOp.h"
#include "IRendererBoneOp.h"

ENGINE_BEGIN

struct ENGINE_API RenderRequestEssential
{
	M4										worldMatrix;
	RenderGroup								renderGroup;
	int										renderGroupOrder;
	uint8_t									layerIndex;
	IMaterial*								material;
	uint									techniqueIndex;
	uint									passIndex;
	IMesh*									mesh;
	uint									subMeshIndex;
	bool									instance;

	bool IsValid() const;
};

struct ENGINE_API RenderRequestCustomPrimitiveCount
{
	bool									usePrimitiveCount;
	uint									primitiveCount;
};

struct ENGINE_API RenderRequestOp
{
	IRendererBoundsOp*						boundsOp; // instance 플래그가 활성화 상태에서는 인식되지 않습니다.
	IRendererCullOp*						cullOp;
	IRendererBoneOp*						boneOp;
};

struct ENGINE_API RenderRequestShadow
{
	bool									draw;
	bool									shadowPass;
};

struct ENGINE_API RenderRequest
{
	RenderRequestEssential					essential = {};
	RenderRequestCustomPrimitiveCount		customPrimitiveCount = {};
	RenderRequestShadow						shadow = {};
	RenderRequestOp							op = {};

	inline bool IsValid() { return essential.IsValid(); }
};

ENGINE_END
