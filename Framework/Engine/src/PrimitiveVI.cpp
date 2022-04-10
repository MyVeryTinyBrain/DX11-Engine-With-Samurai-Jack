#include "EnginePCH.h"
#include "PrimitiveVI.h"
#include "VI.h"

VI* PrimitiveVI::CreateNormalizedQuad()
{
	Vertex vertices[4] = {};
	vertices[0].position = V3(-1, +1, 0);
	vertices[1].position = V3(+1, +1, 0);
	vertices[2].position = V3(+1, -1, 0);
	vertices[3].position = V3(-1, -1, 0);
	vertices[0].uvw = V2(0, 0);
	vertices[1].uvw = V2(1, 0);
	vertices[2].uvw = V2(1, 1);
	vertices[3].uvw = V2(0, 1);

	for (uint i = 0; i < 4; ++i)
		vertices[i].normal = V3(0, 0, -1);

	TriangleIndex triangles[2] = {};
	triangles[0] = { 0,1,2 };
	triangles[1] = { 0,2,3 };

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, 4, triangles, 2);

	return vi;
}

VI* PrimitiveVI::CreateQuad(V2 extents)
{
	Vertex vertices[4] = {};
	TriangleIndex triangles[2] = {};

	vertices[0].position = V3(-extents.x, +extents.y, +0.0f) * 0.5f;
	vertices[1].position = V3(+extents.x, +extents.y, +0.0f) * 0.5f;
	vertices[2].position = V3(+extents.x, -extents.y, +0.0f) * 0.5f;
	vertices[3].position = V3(-extents.x, -extents.y, +0.0f) * 0.5f;
	vertices[0].uvw = V2(0, 0);
	vertices[1].uvw = V2(1, 0);
	vertices[2].uvw = V2(1, 1);
	vertices[3].uvw = V2(0, 1);

	for (uint i = 0; i < 4; ++i)
		vertices[i].normal = V3(0, 0, -1);

	triangles[0] = { 0, 1, 2 };
	triangles[1] = { 0, 2, 3 };

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, 4, triangles, 2);

	return vi;
}

VI* PrimitiveVI::CreatePlane(V2 extents)
{
	Vertex vertices[4] = {};
	TriangleIndex triangles[2] = {};

	vertices[0].position = V3(-extents.x, +0.0f, +extents.y) * 0.5f;
	vertices[1].position = V3(+extents.x, +0.0f, +extents.y) * 0.5f;
	vertices[2].position = V3(+extents.x, +0.0f, -extents.y) * 0.5f;
	vertices[3].position = V3(-extents.x, +0.0f, -extents.y) * 0.5f;
	vertices[0].uvw = V2(0, 0);
	vertices[1].uvw = V2(1, 0);
	vertices[2].uvw = V2(1, 1);
	vertices[3].uvw = V2(0, 1);

	for (uint i = 0; i < 4; ++i)
		vertices[i].normal = V3(0, 1, 0);

	triangles[0] = { 0, 1, 2 };
	triangles[1] = { 0, 2, 3 };

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, 4, triangles, 2);

	return vi;
}

VI* PrimitiveVI::CreateBox(V3 extents)
{
	Vertex vertices[24] = {};
	TriangleIndex triangles[12] = {};

	// front
	vertices[0].position = V3(-extents.x, +extents.y, -extents.z);
	vertices[1].position = V3(+extents.x, +extents.y, -extents.z);
	vertices[2].position = V3(+extents.x, -extents.y, -extents.z);
	vertices[3].position = V3(-extents.x, -extents.y, -extents.z);
	vertices[0].uvw = V2(0, 0);
	vertices[1].uvw = V2(1, 0);
	vertices[2].uvw = V2(1, 1);
	vertices[3].uvw = V2(0, 1);
	vertices[0].normal = V3(0, 0, -1);
	vertices[1].normal = V3(0, 0, -1);
	vertices[2].normal = V3(0, 0, -1);
	vertices[3].normal = V3(0, 0, -1);
	triangles[0] = { 0,1,2 };
	triangles[1] = { 0,2,3 };

	// back
	vertices[4].position = V3(-extents.x, +extents.y, +extents.z);
	vertices[5].position = V3(+extents.x, +extents.y, +extents.z);
	vertices[6].position = V3(+extents.x, -extents.y, +extents.z);
	vertices[7].position = V3(-extents.x, -extents.y, +extents.z);
	vertices[4].uvw = V2(1, 0);
	vertices[5].uvw = V2(0, 0);
	vertices[6].uvw = V2(0, 1);
	vertices[7].uvw = V2(1, 1);
	vertices[4].normal = V3(0, 0, 1);
	vertices[5].normal = V3(0, 0, 1);
	vertices[6].normal = V3(0, 0, 1);
	vertices[7].normal = V3(0, 0, 1);
	triangles[2] = { 4,6,5 };
	triangles[3] = { 4,7,6 };

	// top
	vertices[8].position =  V3(-extents.x, +extents.y, +extents.z);
	vertices[9].position =  V3(+extents.x, +extents.y, +extents.z);
	vertices[10].position = V3(+extents.x, +extents.y, -extents.z);
	vertices[11].position = V3(-extents.x, +extents.y, -extents.z);
	vertices[8].uvw = V2(0, 0);
	vertices[9].uvw = V2(1, 0);
	vertices[10].uvw = V2(1, 1);
	vertices[11].uvw = V2(0, 1);
	vertices[8].normal = V3(0, 1, 0);
	vertices[9].normal = V3(0, 1, 0);
	vertices[10].normal = V3(0, 1, 0);
	vertices[11].normal = V3(0, 1, 0);
	triangles[4] = { 8,9,10 };
	triangles[5] = { 8,10,11 };

	// bottom
	vertices[12].position = V3(-extents.x, -extents.y, +extents.z);
	vertices[13].position = V3(+extents.x, -extents.y, +extents.z);
	vertices[14].position = V3(+extents.x, -extents.y, -extents.z);
	vertices[15].position = V3(-extents.x, -extents.y, -extents.z);
	vertices[12].uvw = V2(1, 0);
	vertices[13].uvw = V2(0, 0);
	vertices[14].uvw = V2(0, 1);
	vertices[15].uvw = V2(1, 1);
	vertices[12].normal = V3(0, -1, 0);
	vertices[13].normal = V3(0, -1, 0);
	vertices[14].normal = V3(0, -1, 0);
	vertices[15].normal = V3(0, -1, 0);
	triangles[6] = { 12,14,13 };
	triangles[7] = { 12,15,14 };

	// right
	vertices[16].position = V3(+extents.x, +extents.y, -extents.z);
	vertices[17].position = V3(+extents.x, +extents.y, +extents.z);
	vertices[18].position = V3(+extents.x, -extents.y, +extents.z);
	vertices[19].position = V3(+extents.x, -extents.y, -extents.z);
	vertices[16].uvw = V2(0, 0);
	vertices[17].uvw = V2(1, 0);
	vertices[18].uvw = V2(1, 1);
	vertices[19].uvw = V2(0, 1);
	vertices[16].normal = V3(1, 0, 0);
	vertices[17].normal = V3(1, 0, 0);
	vertices[18].normal = V3(1, 0, 0);
	vertices[19].normal = V3(1, 0, 0);
	triangles[8] = { 16,17,18 };
	triangles[9] = { 16,18,19 };

	// left
	vertices[20].position = V3(-extents.x, +extents.y, -extents.z);
	vertices[21].position = V3(-extents.x, +extents.y, +extents.z);
	vertices[22].position = V3(-extents.x, -extents.y, +extents.z);
	vertices[23].position = V3(-extents.x, -extents.y, -extents.z);
	vertices[20].uvw = V2(1, 0);
	vertices[21].uvw = V2(0, 0);
	vertices[22].uvw = V2(0, 1);
	vertices[23].uvw = V2(1, 1);
	vertices[20].normal = V3(-1, 0, 0);
	vertices[21].normal = V3(-1, 0, 0);
	vertices[22].normal = V3(-1, 0, 0);
	vertices[23].normal = V3(-1, 0, 0);
	triangles[10] = { 20,22,21 };
	triangles[11] = { 20,23,22 };

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, 24, triangles, 12);

	return vi;
}

VI* PrimitiveVI::CreateSkyBox()
{
	V3 extents = V3::one() * 0.5f;

	Vertex vertices[24] = {};
	TriangleIndex triangles[12] = {};

	// front
	vertices[0].position = V3(-extents.x, +extents.y, +extents.z);
	vertices[1].position = V3(+extents.x, +extents.y, +extents.z);
	vertices[2].position = V3(+extents.x, -extents.y, +extents.z);
	vertices[3].position = V3(-extents.x, -extents.y, +extents.z);
	vertices[0].uvw = V2(1.0f / 4.0f, 1.0f / 3.0f);
	vertices[1].uvw = V2(2.0f / 4.0f, 1.0f / 3.0f);
	vertices[2].uvw = V2(2.0f / 4.0f, 2.0f / 3.0f);
	vertices[3].uvw = V2(1.0f / 4.0f, 2.0f / 3.0f);
	vertices[0].normal = V3(0, 0, +1);
	vertices[1].normal = V3(0, 0, +1);
	vertices[2].normal = V3(0, 0, +1);
	vertices[3].normal = V3(0, 0, +1);
	triangles[0] = { 0,1,2 };
	triangles[1] = { 0,2,3 };

	// back
	vertices[4].position = V3(+extents.x, +extents.y, -extents.z);
	vertices[5].position = V3(-extents.x, +extents.y, -extents.z);
	vertices[6].position = V3(-extents.x, -extents.y, -extents.z);
	vertices[7].position = V3(+extents.x, -extents.y, -extents.z);
	vertices[4].uvw = V2(3.0f / 4.0f, 1.0f / 3.0f);
	vertices[5].uvw = V2(4.0f / 4.0f, 1.0f / 3.0f);
	vertices[6].uvw = V2(4.0f / 4.0f, 2.0f / 3.0f);
	vertices[7].uvw = V2(3.0f / 4.0f, 2.0f / 3.0f);
	vertices[4].normal = V3(0, 0, -1);
	vertices[5].normal = V3(0, 0, -1);
	vertices[6].normal = V3(0, 0, -1);
	vertices[7].normal = V3(0, 0, -1);
	triangles[2] = { 4,5,6 };
	triangles[3] = { 4,6,7 };

	// top
	vertices[8].position = V3(+extents.x, +extents.y, +extents.z);
	vertices[9].position = V3(-extents.x, +extents.y, +extents.z);
	vertices[10].position = V3(-extents.x, +extents.y, -extents.z);
	vertices[11].position = V3(+extents.x, +extents.y, -extents.z);
	vertices[8].uvw = V2(2.0f / 4.0f, 1.0f / 3.0f);
	vertices[9].uvw = V2(1.0f / 4.0f, 1.0f / 3.0f);
	vertices[10].uvw = V2(1.0f / 4.0f, 0.0f / 3.0f);
	vertices[11].uvw = V2(2.0f / 4.0f, 0.0f / 3.0f);
	vertices[8].normal = V3(0, -1, 0);
	vertices[9].normal = V3(0, -1, 0);
	vertices[10].normal = V3(0, -1, 0);
	vertices[11].normal = V3(0, -1, 0);
	triangles[4] = { 8,9,10 };
	triangles[5] = { 8,10,11 };

	// bottom
	vertices[12].position = V3(-extents.x, -extents.y, +extents.z);
	vertices[13].position = V3(+extents.x, -extents.y, +extents.z);
	vertices[14].position = V3(+extents.x, -extents.y, -extents.z);
	vertices[15].position = V3(-extents.x, -extents.y, -extents.z);
	vertices[12].uvw = V2(1.0f / 4.0f, 2.0f / 3.0f);
	vertices[13].uvw = V2(2.0f / 4.0f, 2.0f / 3.0f);
	vertices[14].uvw = V2(2.0f / 4.0f, 3.0f / 3.0f);
	vertices[15].uvw = V2(1.0f / 4.0f, 3.0f / 3.0f);
	vertices[12].normal = V3(0, +1, 0);
	vertices[13].normal = V3(0, +1, 0);
	vertices[14].normal = V3(0, +1, 0);
	vertices[15].normal = V3(0, +1, 0);
	triangles[6] = { 12,13,14 };
	triangles[7] = { 12,14,15 };

	// right
	vertices[16].position = V3(+extents.x, +extents.y, +extents.z);
	vertices[17].position = V3(+extents.x, +extents.y, -extents.z);
	vertices[18].position = V3(+extents.x, -extents.y, -extents.z);
	vertices[19].position = V3(+extents.x, -extents.y, +extents.z);
	vertices[16].uvw = V2(2.0f / 4.0f, 1.0f / 3.0f);
	vertices[17].uvw = V2(3.0f / 4.0f, 1.0f / 3.0f);
	vertices[18].uvw = V2(3.0f / 4.0f, 2.0f / 3.0f);
	vertices[19].uvw = V2(2.0f / 4.0f, 2.0f / 3.0f);
	vertices[16].normal = V3(-1, 0, 0);
	vertices[17].normal = V3(-1, 0, 0);
	vertices[18].normal = V3(-1, 0, 0);
	vertices[19].normal = V3(-1, 0, 0);
	triangles[8] = { 16,17,18 };
	triangles[9] = { 16,18,19 };

	// left
	vertices[20].position = V3(-extents.x, +extents.y, -extents.z);
	vertices[21].position = V3(-extents.x, +extents.y, +extents.z);
	vertices[22].position = V3(-extents.x, -extents.y, +extents.z);
	vertices[23].position = V3(-extents.x, -extents.y, -extents.z);
	vertices[20].uvw = V2(0.0f / 4.0f, 1.0f / 3.0f);
	vertices[21].uvw = V2(1.0f / 4.0f, 1.0f / 3.0f);
	vertices[22].uvw = V2(1.0f / 4.0f, 2.0f / 3.0f);
	vertices[23].uvw = V2(0.0f / 4.0f, 2.0f / 3.0f);
	vertices[20].normal = V3(-1, 0, 0);
	vertices[21].normal = V3(-1, 0, 0);
	vertices[22].normal = V3(-1, 0, 0);
	vertices[23].normal = V3(-1, 0, 0);
	triangles[10] = { 20,21,22 };
	triangles[11] = { 20,22,23 };

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, 24, triangles, 12);

	return vi;
}

VI* PrimitiveVI::CreateSphere(float radius, uint numV, uint numH)
{
	return CreateCapsule(radius, 0.0f, numV, numH);
}

VI* PrimitiveVI::CreateCapsule(float radius, float halfHeight, uint numV, uint numCapH)
{
	if (numV < 3)
		numV = 3;

	uint numH = numCapH * 2;

	if (numH < 2)
		numH = 2;

	uint numSpin = numV + 1;
	uint numVertices = numSpin * numH + numSpin * 2;
	uint numTriangles = numSpin * (numH + 1) * 2;

	Vertex* vertices = new Vertex[numVertices]{};
	TriangleIndex* triangles = new TriangleIndex[numTriangles]{};

	// Setup vertices =====================================================================

	SphericalYaw spherical;
	spherical.Radius = radius;

	uint vertexIndex = 0;
	for (uint i = 1; i <= numH; ++i)
	{
		float heightPercent = float(i) / float(numH);
		spherical.PhiAngle = 180.0f * heightPercent;

		for (uint j = 0; j < numSpin; ++j)
		{
			float spinPercent = (float(j) / float(numV));
			spherical.ThetaAngle = 360.0f * spinPercent;
			V3 cartesian = spherical.cartesian;
			float direction = (i <= numH / 2) ? (+1.0f) : (-1.0f);

			vertices[vertexIndex].position = spherical.cartesian + (V3::up() * halfHeight * direction);

			float normalizedHeight = (vertices[vertexIndex].position.y + halfHeight + radius) / (2.0f * (halfHeight + radius));

			vertices[vertexIndex].uvw = V2(spinPercent, 1.0f - normalizedHeight);
			vertices[vertexIndex].normal = cartesian.normalized;

			++vertexIndex;
		}
	}

	uint topVertexBeginIndex = vertexIndex;
	uint bottomVertexBeginIndex = vertexIndex + numSpin;

	for (uint i = 0; i < numSpin; ++i)
	{
		float spinPercent = (float(i) / float(numV));

		vertices[topVertexBeginIndex + i].position = V3::up() * (radius + halfHeight);
		vertices[topVertexBeginIndex + i].uvw = V2(spinPercent, 0.0f);
		vertices[topVertexBeginIndex + i].normal = V3::up();

		vertices[bottomVertexBeginIndex + i].position = V3::down() * (radius + halfHeight);
		vertices[bottomVertexBeginIndex + i].uvw = V2(spinPercent, 1.0f);
		vertices[bottomVertexBeginIndex + i].normal = V3::down();

		vertexIndex += 2;
	}

	// Setup triangles =====================================================================

	uint triangleIndex = 0;
	for (uint i = 0; i <= numH - 2; ++i)
	{
		for (uint j = 0; j < numSpin; ++j)
		{
			uint rect[4];
			uint vertex = j + i * numSpin;
			rect[0] = vertex;
			rect[1] = rect[0] + 1;
			rect[2] = rect[1] + numSpin;
			rect[3] = rect[0] + numSpin;

			triangles[triangleIndex]._0 = rect[0];
			triangles[triangleIndex]._1 = rect[1];
			triangles[triangleIndex]._2 = rect[2];

			triangles[triangleIndex + 1]._0 = rect[0];
			triangles[triangleIndex + 1]._1 = rect[2];
			triangles[triangleIndex + 1]._2 = rect[3];

			triangleIndex += 2;
		}
	}

	for (uint i = 0; i < numSpin; ++i)
	{
		uint topVertex = i;
		uint bottomVerterx = numSpin * (numH - 1) + i;
		uint topTriangle[3];
		uint bottomTriangle[3];

		topTriangle[0] = topVertex;
		topTriangle[1] = topVertexBeginIndex + i;
		topTriangle[2] = topVertex + 1;

		bottomTriangle[0] = bottomVerterx;
		bottomTriangle[1] = bottomVerterx + 1;
		bottomTriangle[2] = bottomVertexBeginIndex + i;

		triangles[triangleIndex]._0 = topTriangle[0];
		triangles[triangleIndex]._1 = topTriangle[1];
		triangles[triangleIndex]._2 = topTriangle[2];

		triangles[triangleIndex + 1]._0 = bottomTriangle[0];
		triangles[triangleIndex + 1]._1 = bottomTriangle[1];
		triangles[triangleIndex + 1]._2 = bottomTriangle[2];

		triangleIndex += 2;
	}

	// =====================================================================================

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, numVertices, triangles, numTriangles);

	SafeDeleteArray(vertices);
	SafeDeleteArray(triangles);

	return vi;
}

VI* PrimitiveVI::CreateCylinder(float radius, float halfHeight, uint numV)
{
	if (numV < 3)
		numV = 3;

	uint numSpin = numV + 1;
	uint oneCircleVertice = numSpin;
	uint circleVertices = oneCircleVertice * 4;
	uint numVertices = circleVertices + 2;
	uint numTriangles = numSpin * 4;

	Vertex* vertices = new Vertex[numVertices]{};
	TriangleIndex* triangles = new TriangleIndex[numTriangles]{};

	// Setup vertices =====================================================================

	uint vertexIndex = 0;
	for (uint i = 0; i < numSpin; ++i)
	{
		uint topCapVIdx = circleVertices / 4 * 0 + i;
		uint bottomCapVIdx = circleVertices / 4 * 1 + i;
		uint topSideVIdx = circleVertices / 4 * 2 + i;
		uint bottomSideVIdx = circleVertices / 4 * 3 + i;

		float spinPercent = (float(i) / float(numV));
		float radian = spinPercent * TAU;
		float x = Cos(radian) * radius;
		float y = halfHeight;
		float z = Sin(radian) * radius;

		vertices[topCapVIdx].position = vertices[topSideVIdx].position = V3(x, +y, z);
		vertices[bottomCapVIdx].position = vertices[bottomSideVIdx].position = V3(x, -y, z);

		vertices[topCapVIdx].normal = V3::up();
		vertices[bottomCapVIdx].normal = V3::down();
		vertices[topSideVIdx].normal = vertices[bottomSideVIdx].normal = V3(x, 0, z);

		vertices[topCapVIdx].uvw = vertices[bottomCapVIdx].uvw = V2(x + 1.0f, z + 1.0f) * 0.5f;
		vertices[topSideVIdx].uvw.x = vertices[bottomSideVIdx].uvw.x = spinPercent;
		vertices[topSideVIdx].uvw.y = 0;
		vertices[bottomSideVIdx].uvw.y = 1;

		vertexIndex += 4;
	}

	uint topCenterVIdx = vertexIndex;
	uint bottomCenterVIdx = vertexIndex + 1;

	vertexIndex += 2;

	vertices[topCenterVIdx].position = V3(0, halfHeight, 0);
	vertices[bottomCenterVIdx].position = V3(0, -halfHeight, 0);
	vertices[topCenterVIdx].normal = V3::up();
	vertices[bottomCenterVIdx].normal = V3::down();
	vertices[topCenterVIdx].uvw = vertices[bottomCenterVIdx].uvw = V2(0.5, 0.5f);

	// Setup triangles =====================================================================

	uint tIndex = 0;
	for (uint i = 0; i < numSpin; ++i)
	{
		uint topCapVIdx0 = circleVertices / 4 * 0 + i;
		uint bottomCapVIdx0 = circleVertices / 4 * 1 + i;
		uint topSideVIdx0 = circleVertices / 4 * 2 + i;
		uint bottomSideVIdx0 = circleVertices / 4 * 3 + i;

		uint topCapVIdx1 = topCenterVIdx;
		uint topCapVIdx2 = topCapVIdx0 + 1;

		uint bottomCapVIdx1 = bottomCapVIdx0 + 1;
		uint bottomCapVIdx2 = bottomCenterVIdx;

		uint sideIdx0 = topSideVIdx0;
		uint sideIdx1 = topSideVIdx0 + 1;
		uint sideIdx2 = bottomSideVIdx0 + 1;
		uint sideIdx3 = bottomSideVIdx0;

		if (i == numV)
		{
			topCapVIdx2 = circleVertices / 4 * 0;
			bottomCapVIdx2 = circleVertices / 4 * 1;
			sideIdx1 = circleVertices / 4 * 2;
			sideIdx2 = circleVertices / 4 * 3;
		}

		triangles[tIndex + 0]._0 = topCapVIdx0;
		triangles[tIndex + 0]._1 = topCapVIdx1;
		triangles[tIndex + 0]._2 = topCapVIdx2;

		triangles[tIndex + 1]._0 = bottomCapVIdx0;
		triangles[tIndex + 1]._1 = bottomCapVIdx1;
		triangles[tIndex + 1]._2 = bottomCapVIdx2;

		triangles[tIndex + 2]._0 = sideIdx0;
		triangles[tIndex + 2]._1 = sideIdx1;
		triangles[tIndex + 2]._2 = sideIdx2;
		
		triangles[tIndex + 3]._0 = sideIdx0;
		triangles[tIndex + 3]._1 = sideIdx2;
		triangles[tIndex + 3]._2 = sideIdx3;

		tIndex += 4;
	}

	// =====================================================================================

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, numVertices, triangles, numTriangles);

	SafeDeleteArray(vertices);
	SafeDeleteArray(triangles);

	return vi;
}

HRESULT PrimitiveVI::CreateAndSetup(VI** out_vi, Vertex* vertices, uint vCount, TriangleIndex* triangles, uint tCount)
{
	if (!out_vi)
		return E_FAIL;

	*out_vi = nullptr;

	VI* vi = new VI;

	auto ReleaseVars = [&]() { SafeDelete(vi); };

	HRESULT hr = S_OK;

	if (vi && FAILED(hr = vi->SetVertices(vertices, vCount)))
	{
		ReleaseVars();
		return hr;
	}

	if (vi && FAILED(hr = vi->SetIndicesAsTriangles(triangles, tCount)))
	{
		ReleaseVars();
		return hr;
	}

	if (vi && FAILED(hr = vi->RecalculateTangentsBinormals()))
	{
		ReleaseVars();
		return hr;
	}

	if (vi && FAILED(hr = vi->RecalculateBounds()))
	{
		ReleaseVars();
		return hr;
	}

	*out_vi = vi;

	return S_OK;
}
