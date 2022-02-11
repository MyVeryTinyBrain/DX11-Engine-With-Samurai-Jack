#pragma once

#include "RenderGroup.h"
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
	M4						worldMatrix;
	RenderGroup				renderGroup;
	int						renderGroupOrder;
	uint8_t					layerIndex;
	IMaterial*				material;
	uint					techniqueIndex;
	uint					passIndex;
	IMesh*					mesh;
	uint					subMeshIndex;
	bool					instance;

	bool IsValid() const;
};

struct ENGINE_API RenderRequestOp
{
	// * 특정 Operation은 instance 플래그가 활성화 상태인경우 처리되지 않습니다.

	IRendererBoundsOp*			boundsOp;
	IRendererCullOp*			cullOp;
	IRendererBoneOp*			boneOp;
};

struct ENGINE_API RenderRequestShadow
{
	bool						draw;
	bool						cutoffEnable;
	float						cutoffAlpha;
	ITexture*					cutoffTexture;
};

struct ENGINE_API RenderRequest
{
	RenderRequestEssential	essential = {};
	RenderRequestShadow		shadow = {};
	RenderRequestOp			op = {};

	inline bool IsValid() { return essential.IsValid(); }
};

ENGINE_END
