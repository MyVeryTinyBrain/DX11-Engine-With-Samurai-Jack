#pragma once

#include "Renderer.h"
#include "IRendererCullOp.h"

ENGINE_BEGIN

class ENGINE_API TrailRenderer : public Renderer, public IRendererCullOp
{
private:

	struct Pair
	{
		V3		Position;
		float	DistanceAccumulation;
	};

public:

	enum class Alignment { View, Local };

private:

	virtual void Awake() override;
	virtual void LateUpdate() override;
	virtual void Render() override;

public:

	void AddPosition(const V3& position);
	void PopPosition();

public:

	inline uint GetRectCount() const { return m_numRect; }
	void SetRectCount(uint value);

	inline float GetWidth() const { return m_width; }
	inline void SetWidth(float value) { m_width = Saturate(value); }

	inline float GetMinVertexDistance() const { return m_minVertexDistance; }
	inline void SetMinVertexDistance(float value) { m_minVertexDistance = Saturate(value); }

	inline float GetBeginShrinkDelay() const { return m_beginShrinkDelay; }
	inline void SetBeginShrinkDelay(float value) { m_beginShrinkDelay = Saturate(value); }

	inline float GetShrinkDistance() const { return m_shrinkDistance; }
	inline void SetShrinkDistance(float value) { m_shrinkDistance = Saturate(value); }

	inline bool IsUseLengthScale() const { return m_useLengthScale; }
	inline void SetUseLengthScale(bool value) { m_useLengthScale = value; }

	inline bool IsApplyLengthScaleToU() const { return m_applyLengthScaleToU; }
	inline void SetApplyLengthScaleToU(bool value) { m_applyLengthScaleToU = value; }

	inline bool IsStretchV() const { return m_stretchV; }
	inline void SetStretchV(bool value) { m_stretchV = value; }

	inline float GetVScale() const { return m_vScale; }
	inline void SetVScale(float value) { m_vScale = value; }

	inline TrailRenderer::Alignment GetAlignment() const { return m_alignment; }
	inline void SetAlignment(TrailRenderer::Alignment value) { m_alignment = value; }

	_declspec(property(get = GetRectCount, put = SetRectCount)) uint rectCount;
	_declspec(property(get = GetWidth, put = SetWidth)) float width;
	_declspec(property(get = GetMinVertexDistance, put = SetMinVertexDistance)) float minVertexDistance;
	_declspec(property(get = GetBeginShrinkDelay, put = SetBeginShrinkDelay)) float beginShrinkDelay;
	_declspec(property(get = GetShrinkDistance, put = SetShrinkDistance)) float shrinkDistance;
	_declspec(property(get = IsUseLengthScale, put = SetUseLengthScale)) bool useLengthScale;
	_declspec(property(get = IsApplyLengthScaleToU, put = SetApplyLengthScaleToU)) bool applyLengthScaleToU;
	_declspec(property(get = IsStretchV, put = SetStretchV)) bool stretchV;
	_declspec(property(get = GetVScale, put = SetVScale)) float vScale;
	_declspec(property(get = GetAlignment, put = SetAlignment)) TrailRenderer::Alignment alignment;

public:

	virtual bool CullTest(ICamera* camera) const override;
	virtual Bounds GetBounds() const override;
	virtual bool IsValid() const override;

private:

	void SetupMesh(uint numRect);
	void Shrink(float shrinkDistance);
	void SetupVerticexPair(
		uint pairIndex, 
		const V3& camDir, 
		float width, 
		V3& inout_min, V3& inout_max);
	void ApplyVertices();

private:

	void SetupDefaultMaterial();

private:

	uint						m_numRect = 0;

	deque<Pair>					m_pairs;

	float						m_width = 1.0f;

	float						m_minVertexDistance = 0.1f;
	float						m_beginShrinkDelay = 0.1f;
	float						m_beginShrinkCounter = 0.0f;
	float						m_shrinkDistance = 5.0f;

	bool						m_useLengthScale = true;
	bool						m_applyLengthScaleToU = true;

	bool						m_stretchV = false;
	float						m_vScale = 1.0f; // v of UVW

	TrailRenderer::Alignment	m_alignment = Alignment::View;

	/*

	8---9	uvw.y = 4 * VDistance, uvw.z = 1
	| /	|
	6---7	uvw.y = 3 * VDistance
	| /	|
	4---5	uvw.y = 2 * VDistance
	| /	|
	2---3	uvw.y = 1 * VDistance
	| /	|
	0---1	uvw.y = 0 * VDistance, uvw.z = 0

	uvw.x = 0
		uvw.x = 1

	*/

};

ENGINE_END
