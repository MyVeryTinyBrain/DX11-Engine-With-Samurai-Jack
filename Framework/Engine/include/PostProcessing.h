#pragma once

#include "PostProcessingTypes.h"

ENGINE_BEGIN
class GraphicSystem;
class CBufferManager;
class InstanceBufferManager;
class VIBuffer;
class CompiledShaderDesc;
class ICamera;
class ENGINE_API PostProcessing
{
public:

	enum class Step 
	{
		DEFERRED,
	};

public:

	PostProcessing(GraphicSystem* graphicSystem, CBufferManager* cBufferManager, InstanceBufferManager* instanceBufferMaanger);

	~PostProcessing();

	HRESULT Initialize();

public:

	void PostProcess(ICamera* camera, PostProcessing::Step step);

private:

	HRESULT SetupQuads();
	HRESULT SetupShaders();

private:

	GraphicSystem* m_graphicSystem = nullptr;
	CBufferManager* m_CBufferManager = nullptr;
	InstanceBufferManager* m_instanceBufferManager = nullptr;

	VIBuffer* m_normalizedQuad = nullptr;		// Immutable Vertices

	CompiledShaderDesc* m_shaderPostProcessing = nullptr;
};

ENGINE_END
