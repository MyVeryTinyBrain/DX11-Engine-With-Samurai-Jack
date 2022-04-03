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

VI* PrimitiveVI::CreateSphere(float radius, uint slice, uint step)
{
	if (slice < 3)
		slice = 3;

	if (step < 2)
		step = 2;

	slice += 1;

	uint numVertices = slice * (step - 1) + slice * 2;
	uint numTriangles = slice * step * 2;

	Vertex* vertices = new Vertex[numVertices]{};
	TriangleIndex* triangles = new TriangleIndex[numTriangles]{};

	// Setup vertices =====================================================================

	SphericalYaw spherical;
	spherical.Radius = radius;

	uint vIndex = 0;
	for (uint i = 1; i < step; ++i)
	{
		float heightPercent = float(i) / float(step);
		spherical.PhiAngle = 180.0f * heightPercent;

		for (uint j = 0; j < slice; ++j)
		{
			float spinPercent = (float(j) / float(slice - 1));
			spherical.ThetaAngle = 360.0f * spinPercent;

			vertices[vIndex].position = spherical.cartesian;
			vertices[vIndex].uvw = V2(spinPercent, heightPercent);
			vertices[vIndex].normal = vertices[vIndex].position.normalized;
			++vIndex;
		}
	}

	uint topVertexBeginIndex = vIndex;
	uint bottomVertexBeginIndex = vIndex + slice;

	for (uint i = 0; i < slice; ++i)
	{
		float spinPercent = (float(i) / float(slice - 1));

		vertices[topVertexBeginIndex + i].position = V3::up() * radius;
		vertices[topVertexBeginIndex + i].uvw = V2(spinPercent, 0);
		vertices[topVertexBeginIndex + i].normal = V3(0, 1, 0);

		vertices[bottomVertexBeginIndex + i].position = V3::down() * radius;
		vertices[bottomVertexBeginIndex + i].uvw = V2(spinPercent, 1);
		vertices[bottomVertexBeginIndex + i].normal = V3(0, -1, 0);

		vIndex += 2;
	}

	// Setup triangles =====================================================================

	uint tIndex = 0;
	for (uint i = 0; i < step - 2; ++i)
	{
		for (uint j = 0; j < slice; ++j)
		{
			uint vIdx = j + i * slice;

			uint i0 = vIdx;

			uint i1 = i0 + 1;
			if (j == slice - 1)
				i1 = i * slice;

			uint i2 = i1 + slice;

			uint i3 = i0 + slice;

			triangles[tIndex]._0 = i0;
			triangles[tIndex]._1 = i1;
			triangles[tIndex]._2 = i2;

			triangles[tIndex + 1]._0 = i0;
			triangles[tIndex + 1]._1 = i2;
			triangles[tIndex + 1]._2 = i3;

			tIndex += 2;
		}
	}

	for (uint i = 0; i < slice; ++i)
	{
		uint topVIdx = i;
		uint bottomVIdx = slice * (step - 2) + i;

		uint t0 = topVIdx;
		uint t1 = topVertexBeginIndex + i;
		uint t2 = topVIdx + 1;
		if (i == slice - 1)
			t2 = 0;

		uint b0 = bottomVIdx;
		uint b1 = bottomVIdx + 1;
		uint b2 = bottomVertexBeginIndex + i;
		if (i == slice - 1)
			b1 = slice * (step - 2);

		triangles[tIndex]._0 = t0;
		triangles[tIndex]._1 = t1;
		triangles[tIndex]._2 = t2;

		triangles[tIndex + 1]._0 = b0;
		triangles[tIndex + 1]._1 = b1;
		triangles[tIndex + 1]._2 = b2;

		tIndex += 2;
	}

	// =====================================================================================

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, numVertices, triangles, numTriangles);

	SafeDeleteArray(vertices);
	SafeDeleteArray(triangles);

	return vi;
}

VI* PrimitiveVI::CreateCapsule(float radius, float halfHeight, uint slice, uint capStep)
{
	if (slice < 3)
		slice = 3;

	uint step = capStep * 2;

	if (step < 2)
		step = 2;

	slice += 1;

	uint numVertices = slice * step + slice * 2;
	uint numTriangles = slice * (step + 1) * 2;

	Vertex* vertices = new Vertex[numVertices]{};
	TriangleIndex* triangles = new TriangleIndex[numTriangles]{};

	// Setup vertices =====================================================================

	SphericalYaw spherical;
	spherical.Radius = radius;

	uint vIndex = 0;
	for (uint i = 1; i <= step; ++i)
	{
		uint k = i;
		if (i > step / 2)
			k -= 1;

		float heightPercent = float(k) / float(step);
		spherical.PhiAngle = 180.0f * heightPercent;

		for (uint j = 0; j < slice; ++j)
		{
			float spinPercent = (float(j) / float(slice - 1));
			spherical.ThetaAngle = 360.0f * spinPercent;

			V3 cartesian = spherical.cartesian;

			vertices[vIndex].position = cartesian;

			if (i <= step / 2)
				vertices[vIndex].position += V3::up() * halfHeight;
			else
				vertices[vIndex].position += V3::down() * halfHeight;

			float v = 1 - (vertices[vIndex].position.y + halfHeight + radius) / (2.0f * (halfHeight + radius));

			vertices[vIndex].uvw = V2(spinPercent, v);
			vertices[vIndex].normal = cartesian.normalized;
			++vIndex;
		}
	}

	uint topVertexBeginIndex = vIndex;
	uint bottomVertexBeginIndex = vIndex + slice;

	for (uint i = 0; i < slice; ++i)
	{
		float spinPercent = (float(i) / float(slice - 1));

		vertices[topVertexBeginIndex + i].position = V3::up() * (radius + halfHeight);
		vertices[topVertexBeginIndex + i].uvw = V2(spinPercent, 0);
		vertices[topVertexBeginIndex + i].normal = V3(0, 1, 0);

		vertices[bottomVertexBeginIndex + i].position = V3::down() * (radius + halfHeight);
		vertices[bottomVertexBeginIndex + i].uvw = V2(spinPercent, 1);
		vertices[bottomVertexBeginIndex + i].normal = V3(0, -1, 0);

		vIndex += 2;
	}

	// Setup triangles =====================================================================

	uint tIndex = 0;
	for (uint i = 0; i <= step - 2; ++i)
	{
		for (uint j = 0; j < slice; ++j)
		{
			uint vIdx = j + i * slice;

			uint i0 = vIdx;

			uint i1 = i0 + 1;
			if (j == slice - 1)
				i1 = i * slice;

			uint i2 = i1 + slice;

			uint i3 = i0 + slice;

			triangles[tIndex]._0 = i0;
			triangles[tIndex]._1 = i1;
			triangles[tIndex]._2 = i2;

			triangles[tIndex + 1]._0 = i0;
			triangles[tIndex + 1]._1 = i2;
			triangles[tIndex + 1]._2 = i3;

			tIndex += 2;
		}
	}

	for (uint i = 0; i < slice; ++i)
	{
		uint topVIdx = i;
		uint bottomVIdx = slice * (step - 1) + i;

		uint t0 = topVIdx;
		uint t1 = topVertexBeginIndex + i;
		uint t2 = topVIdx + 1;
		if (i == slice - 1)
			t2 = 0;

		uint b0 = bottomVIdx;
		uint b1 = bottomVIdx + 1;
		uint b2 = bottomVertexBeginIndex + i;
		if (i == slice - 1)
			b1 = slice * (step - 1);

		triangles[tIndex]._0 = t0;
		triangles[tIndex]._1 = t1;
		triangles[tIndex]._2 = t2;


		triangles[tIndex + 1]._0 = b0;
		triangles[tIndex + 1]._1 = b1;
		triangles[tIndex + 1]._2 = b2;

		tIndex += 2;
	}

	// =====================================================================================

	VI* vi = nullptr;
	HRESULT hr = CreateAndSetup(&vi, vertices, numVertices, triangles, numTriangles);

	SafeDeleteArray(vertices);
	SafeDeleteArray(triangles);

	return vi;
}

VI* PrimitiveVI::CreateCylinder(float radius, float halfHeight, uint slice)
{
	if (slice < 3)
		slice = 3;

	slice += 1;

	uint oneCircleVertice = slice;
	uint circleVertices = oneCircleVertice * 4;
	uint numVertices = circleVertices + 2;
	uint numTriangles = slice * 4;

	Vertex* vertices = new Vertex[numVertices]{};
	TriangleIndex* triangles = new TriangleIndex[numTriangles]{};

	// Setup vertices =====================================================================

	uint vIdx = 0;
	for (uint i = 0; i < slice; ++i)
	{
		uint topCapVIdx = circleVertices / 4 * 0 + i;
		uint bottomCapVIdx = circleVertices / 4 * 1 + i;
		uint topSideVIdx = circleVertices / 4 * 2 + i;
		uint bottomSideVIdx = circleVertices / 4 * 3 + i;

		float spinPercent = (float(i) / float(slice - 1));
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

		vIdx += 4;
	}

	uint topCenterVIdx = vIdx;
	uint bottomCenterVIdx = vIdx + 1;

	vIdx += 2;

	vertices[topCenterVIdx].position = V3(0, halfHeight, 0);
	vertices[bottomCenterVIdx].position = V3(0, -halfHeight, 0);
	vertices[topCenterVIdx].normal = V3::up();
	vertices[bottomCenterVIdx].normal = V3::down();
	vertices[topCenterVIdx].uvw = vertices[bottomCenterVIdx].uvw = V2(0.5, 0.5f);

	// Setup triangles =====================================================================

	uint tIndex = 0;
	for (uint i = 0; i < slice; ++i)
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

		if (i == slice - 1)
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
