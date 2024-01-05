#pragma once

#include "Renderer.h"
#include "IRendererCullOp.h"
#include "IOnCamera.h"

ENGINE_BEGIN

class ENGINE_API LineRenderer : public Renderer, public IRendererCullOp, public IOnCamera
{
public:

	enum class Alignment { View, Local };

private:

	struct VertexPairInput
	{
		V3 start, end;
		LineRenderer::Alignment alignment;
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
	virtual void OnCamera(ICamera* camera, RenderRequest* inout_pInout) override;

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

	inline LineRenderer::Alignment GetAlignment() const { return m_alignment; }
	inline void SetAlignment(LineRenderer::Alignment value) { m_alignment = value; }

	inline uint GetInterpolateStep() const { return m_interpolateStep; }
	inline void SetInterpolateStep(uint value) { m_interpolateStep = value; }

	_declspec(property(get = GetNumPoints)) uint numPoints;
	_declspec(property(get = GetWidth, put = SetWidth)) float width;
	_declspec(property(get = IsLocalSpace, put = SetLocalSpace)) bool isLocalSpace;
	_declspec(property(get = GetAlignment, put = SetAlignment)) LineRenderer::Alignment alignment;
	_declspec(property(get = GetInterpolateStep, put = SetInterpolateStep)) uint interpolateStep;

private:

	void SetupMesh();
	void SetNumMeshRect(uint numRect);
	uint GetNumMeshRect() const;
	uint GetNumInterpolatedRect() const;

	void ApplyVertices(ICamera* camera);
	LineRenderer::VertexPairOutput CalcVertexPair(const LineRenderer::VertexPairInput& desc, V3& inout_min, V3& inout_max) const;

public:

	virtual bool CullTest(ICamera* camera) const override;
	virtual Bounds GetBounds() const override;
	virtual bool IsValid() const override;

private:

	vector<V3> m_points;
	float m_width = 0.5f;
	bool m_localSpace = false;
	LineRenderer::Alignment m_alignment = LineRenderer::Alignment::View;
	uint m_interpolateStep = 0;
};

ENGINE_END
