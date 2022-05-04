#pragma once

ENGINE_BEGIN

class VI;
struct Vertex;
struct TriangleIndex;

namespace VIUtility
{
	VI* CreateNormalizedQuad();
	VI* CreateQuad(V2 extents = V2::one() * 0.5f);
	VI* CreatePlane(V2 extents = V2::one() * 0.5f);
	VI* CreateBox(V3 extents = V3::one() * 0.5f);
	VI* CreateSkyBox();
	VI* CreateSphere(float radius = 0.5f, uint numV = 16, uint numH = 16);
	VI* CreateCapsule(float radius = 0.5f, float halfHeight = 0.5f, uint numV = 32, uint numCapH = 32);
	VI* CreateCylinder(float radius = 0.5f, float halfHeight = 1.0f, uint numV = 32);

	void SetQuadInScreen(const V2& screenSize, const FRect& rect, VI* vi);

	namespace Private
	{
		HRESULT CreateAndSetup(VI** out_vi, Vertex* vertices, uint vCount, TriangleIndex* triangles, uint tCount);
	}
};

ENGINE_END
