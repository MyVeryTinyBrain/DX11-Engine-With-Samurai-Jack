#pragma once

#include "Renderer.h"
#include "IRendererCullOp.h"
#include "IOnCamera.h"

ENGINE_BEGIN

class ENGINE_API TrailRenderer : public Renderer, public IRendererCullOp, public IOnCamera
{
public:

	enum class Alignment { View, Local };

private:

	struct Data
	{
		V3		position;
		Q		rotation;
		float	distanceAcc;
	};
	struct VertexPairInput
	{
		V3 start, end;
		Q rotation; // Using on Local alignment
		float distanceAcc;
		TrailRenderer::Alignment alignment;
		float width;
		ICamera* camera;
		float percent;
		bool inverse;
	};
	struct VertexPairOutput
	{
		V3 position[2];
		V3 normal, binormal, tangent;
		V3 uvw[2];
	};

private:

	virtual void Awake() override;
	virtual void LateUpdate() override;
	virtual void Render() override;
	virtual void OnCamera(ICamera* camera, RenderRequest* inout_pinput) override;

public:

	void AddPosition(const V3& position, const Q& rotation);
	void PopPosition();

public:

	inline float GetWidth() const { return m_width; }
	inline void SetWidth(float value) { m_width = Max(0.0f, value); }

	inline float GetMinVertexDistance() const { return m_minVertexDistance; }
	inline void SetMinVertexDistance(float value) { m_minVertexDistance = Max(0.0f, value); }

	inline float GetBeginShrinkDelay() const { return m_beginShrinkDelay; }
	inline void SetBeginShrinkDelay(float value) { m_beginShrinkDelay = Max(0.0f, value); }

	inline float GetShrinkDistance() const { return m_shrinkDistance; }
	inline void SetShrinkDistance(float value) { m_shrinkDistance = Max(0.0f, value); }

	inline bool IsApplyWidthByLength() const { return m_applyWidthByLength; }
	inline void SetApplyWidthByLength(bool value) { m_applyWidthByLength = value; }

	inline bool IsFitUToWidth() const { return m_fitUToWidth; }
	inline void SetFitUToWidth(bool value) { m_fitUToWidth = value; }

	inline bool IsFitVToLength() const { return m_fitVToLength; }
	inline void SetFitVToLength(bool value) { m_fitVToLength = value; }

	inline float GetVScale() const { return m_vScale; }
	inline void SetVScale(float value) { m_vScale = value; }

	inline TrailRenderer::Alignment GetAlignment() const { return m_alignment; }
	inline void SetAlignment(TrailRenderer::Alignment value) { m_alignment = value; }

	inline bool IsAutoTrailMode() const { return m_autoTrail; }
	inline void SetAutoTrailMode(bool value) { m_autoTrail = value; }

	inline uint GetInterpolateStep() const { return m_interpolateStep; }
	inline void SetInterpolateStep(uint value) { m_interpolateStep = value; }

	_declspec(property(get = GetWidth, put = SetWidth)) float width;
	_declspec(property(get = GetMinVertexDistance, put = SetMinVertexDistance)) float minVertexDistance;
	_declspec(property(get = GetBeginShrinkDelay, put = SetBeginShrinkDelay)) float beginShrinkDelay;
	_declspec(property(get = GetShrinkDistance, put = SetShrinkDistance)) float shrinkDistance;
	_declspec(property(get = IsApplyWidthByLength, put = SetApplyWidthByLength)) bool applyWidthByLength;
	_declspec(property(get = IsFitUToWidth, put = SetFitUToWidth)) bool fitUToWidth;
	_declspec(property(get = IsFitVToLength, put = SetFitVToLength)) bool fitVToLength;
	_declspec(property(get = GetVScale, put = SetVScale)) float vScale;
	_declspec(property(get = GetAlignment, put = SetAlignment)) TrailRenderer::Alignment alignment;
	_declspec(property(get = IsAutoTrailMode, put = SetAutoTrailMode)) bool autoTrail;
	_declspec(property(get = GetInterpolateStep, put = SetInterpolateStep)) uint interpolateStep;

public:

	virtual bool CullTest(ICamera* camera) const override;
	virtual Bounds GetBounds() const override;
	virtual bool IsValid() const override;

private:

	void SetupMesh();
	void SetNumMeshRect(uint numRect);
	uint GetNumMeshRect() const;
	uint GetNumInterpolatedRect() const;

	void Shrink(float shrinkDistance);
	void ApplyVertices(ICamera* camera);
	TrailRenderer::VertexPairOutput CalcVertexPair(const TrailRenderer::VertexPairInput& desc, V3& inout_min, V3& inout_max) const;

private:

	deque<Data>					m_datas;

	float						m_width = 1.0f;

	float						m_minVertexDistance = 0.2f;
	float						m_beginShrinkDelay = 0.1f;
	float						m_beginShrinkCounter = 0.0f;
	float						m_shrinkDistance = 5.0f;

	// 트레일의 길이에 따라서 폭을 자동으로 조정합니다.
	// 시작 폭은 Width가 되며, 끝 폭은 0이 됩니다.
	bool						m_applyWidthByLength = true;

	// U를 폭에 맞춥니다.
	bool						m_fitUToWidth = true;

	// V를 길이에 맞춥니다.
	// 비활성화되면 vScale 단위로 반복됩니다.
	bool						m_fitVToLength = false;
	float						m_vScale = 1.0f;

	TrailRenderer::Alignment	m_alignment = Alignment::View;

	bool						m_autoTrail = true;

	uint						m_interpolateStep = 10;
};

ENGINE_END
