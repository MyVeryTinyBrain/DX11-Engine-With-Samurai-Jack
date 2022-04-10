#pragma once

ENGINE_BEGIN
class VI;
struct Vertex;
struct TriangleIndex;
class ENGINE_API PrimitiveVI final abstract
{
public:

	static VI* CreateNormalizedQuad();

	static VI* CreateQuad(V2 extents = V2::one() * 0.5f);

	static VI* CreatePlane(V2 extents = V2::one() * 0.5f);

	static VI* CreateBox(V3 extents = V3::one() * 0.5f);

	static VI* CreateSkyBox();

	static VI* CreateSphere(float radius = 0.5f, uint numV = 16, uint numH = 16);

	static VI* CreateCapsule(float radius = 0.5f, float halfHeight = 0.5f, uint numV = 16, uint numCapH = 16);

	static VI* CreateCylinder(float radius = 0.5f, float halfHeight = 1.0f, uint numV = 16);

private:

	static HRESULT CreateAndSetup(VI** out_vi, Vertex* vertices, uint vCount, TriangleIndex* triangles, uint tCount);

};

ENGINE_END
