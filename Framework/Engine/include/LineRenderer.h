#pragma once

#include "Renderer.h"
#include "IRendererCullOp.h"

ENGINE_BEGIN

class ENGINE_API LineRenderer : public Renderer, public IRendererCullOp
{
private:

	virtual void Awake() override;
	virtual void Render() override;

public:

	inline uint GetNumPoints() const { return (uint)m_points.size(); }
	void AddPoint(const V3& point);
	inline const V3& GetPoint(uint index) const { return m_points[index]; }
	inline void SetPoint(uint index, const V3& point) { m_points[index] = point; }
	bool RemovePoint(uint index);
	void ClearPoints();
	 
	inline float GetWidth() const { return m_width; }
	inline void SetWidth(float value) { m_width = value; }

	inline bool IsLocalSpace() const { return m_localSpace; }
	inline void SetLocalSpace(bool value) { m_localSpace = value; }

	_declspec(property(get = GetNumPoints)) uint numPoints;
	_declspec(property(get = GetWidth, put = SetWidth)) float width;
	_declspec(property(get = IsLocalSpace, put = SetLocalSpace)) bool isLocalSpace;

public:

	virtual bool CullTest(ICamera* camera) const override;
	virtual Bounds GetBounds() const override;
	virtual bool IsValid() const override;

private:

	void FitNumRect();
	void SetupMesh(uint numRect);
	void SetupVertexPair(
		uint pointIndex,
		const V3& camDir,
		const V3& camPos,
		float width,
		V3& inout_min, V3& inout_max);
	void ApplyVertices();

private:

	uint						m_numRect = 0;
	float						m_width = 0.5f;
	bool						m_localSpace = false;
	vector<V3>					m_points;

	/*

	8---9	..uvw.y = 1, uvw.z = 1
	| /	|
	6---7	
	| /	|
	4---5	
	| /	|
	2---3	
	| /	|
	0---1	..uvw.y = 0, uvw.z = 0

	.	.
	.	.
	uvw.x = 0
		.
		.
		uvw.x = 1

	*/
};

ENGINE_END
