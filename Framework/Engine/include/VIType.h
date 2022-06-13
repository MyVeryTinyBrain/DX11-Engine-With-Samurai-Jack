#pragma once

ENGINE_BEGIN

typedef unsigned int Index;

struct TriangleIndex
{
	Index _0;
	Index _1;
	Index _2;
};

struct ENGINE_API Vertex
{
	XM_CONSTEXPR inline static D3D11_INPUT_CLASSIFICATION InputSlotClass() { return D3D11_INPUT_PER_VERTEX_DATA; }
	XM_CONSTEXPR inline static uint InputSlot() { return 0u; }
	XM_CONSTEXPR inline static BOOL InstanceDataStepRate() { return FALSE; }

	V3 position;			// float3			POSITION
	XM_CONSTEXPR inline static uint32_t			PositionPosition() { return 0; }
	XM_CONSTEXPR inline static uint32_t			PositionSize() { return sizeof(V3); }
	XM_CONSTEXPR inline static const char*		PositionName() { return "POSITION"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		PositionFormat() { return DXGI_FORMAT_R32G32B32_FLOAT; }

	V3 uvw;					// float3			TEXCOORD
	XM_CONSTEXPR inline static uint32_t			UVWPosition() { return PositionPosition() + PositionSize(); }
	XM_CONSTEXPR inline static uint32_t			UVWSize() { return sizeof(V3); }
	XM_CONSTEXPR inline static const char*		UVWName() { return "TEXCOORD"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		UVWFormat() { return DXGI_FORMAT_R32G32B32_FLOAT; }

	V3 normal;				// float3			NORMAL	
	XM_CONSTEXPR inline static uint32_t			NormalPosition() { return UVWPosition() + UVWSize(); }
	XM_CONSTEXPR inline static uint32_t			NormalSize() { return sizeof(V3); }
	XM_CONSTEXPR inline static const char*		NormalName() { return "NORMAL"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		NormalFormat() { return DXGI_FORMAT_R32G32B32_FLOAT; }

	V3 tangent;				// float3			TANGENT	
	XM_CONSTEXPR inline static uint32_t			TangentPosition() { return NormalPosition() + NormalSize(); }
	XM_CONSTEXPR inline static uint32_t			TangentSize() { return sizeof(V3); }
	XM_CONSTEXPR inline static const char*		TangentName() { return "TANGENT"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		TangentFormat() { return DXGI_FORMAT_R32G32B32_FLOAT; }

	V3 biNormal;			// float3			BINORMAL
	XM_CONSTEXPR inline static uint32_t			BiNormalPosition() { return TangentPosition() + TangentSize(); }
	XM_CONSTEXPR inline static uint32_t			BiNormalSize() { return sizeof(V3); }
	XM_CONSTEXPR inline static const char*		BiNormalName() { return "BINORMAL"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		BiNormalFormat() { return DXGI_FORMAT_R32G32B32_FLOAT; }

	Color color;			// float4			COLOR	
	XM_CONSTEXPR inline static uint32_t			ColorPosition() { return BiNormalPosition() + BiNormalSize(); }
	XM_CONSTEXPR inline static uint32_t			ColorSize() { return sizeof(Color); }
	XM_CONSTEXPR inline static const char*		ColorName() { return "COLOR"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		ColorFormat() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }

	uint4 blendIndices;		// uint4			BLENDINDICES
	XM_CONSTEXPR inline static uint32_t			BlendIndicesPosition() { return ColorPosition() + ColorSize(); }
	XM_CONSTEXPR inline static uint32_t			BlendIndicesSize() { return sizeof(uint4); }
	XM_CONSTEXPR inline static const char*		BlendIndicesName() { return "BLENDINDICES"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		BlendIndicesFormat() { return DXGI_FORMAT_R32G32B32A32_UINT; }

	V4 blendWeight;			// float4			BLENDWEIGHT
	XM_CONSTEXPR inline static uint32_t			BlendWeightPosition() { return BlendIndicesPosition() + BlendIndicesSize(); }
	XM_CONSTEXPR inline static uint32_t			BlendWeightSize() { return sizeof(V4); }
	XM_CONSTEXPR inline static const char*		BlendWeightName() { return "BLENDWEIGHT"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		BlendWeightFormat() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }
};

struct ENGINE_API InstanceData
{
	XM_CONSTEXPR inline static D3D11_INPUT_CLASSIFICATION InputSlotClass() { return D3D11_INPUT_PER_INSTANCE_DATA; }
	XM_CONSTEXPR inline static uint InputSlot() { return 1u; }
	XM_CONSTEXPR inline static BOOL InstanceDataStepRate() { return TRUE; }

	V4 right;				// float4			INSTANCE_RIGHT
	XM_CONSTEXPR inline static uint32_t			RightPosition() { return 0; }
	XM_CONSTEXPR inline static uint32_t			RightSize() { return sizeof(V4); }
	XM_CONSTEXPR inline static const char*		RightName() { return "INSTANCE_RIGHT"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		RightFormat() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }

	V4 up;					// float4			INSTANCE_UP
	XM_CONSTEXPR inline static uint32_t			UpPosition() { return RightPosition() + RightSize(); }
	XM_CONSTEXPR inline static uint32_t			UpSize() { return sizeof(V4); }
	XM_CONSTEXPR inline static const char*		UpName() { return "INSTANCE_UP"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		UpFormat() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }

	V4 forward;				// float4			INSTANCE_FORWARD
	XM_CONSTEXPR inline static uint32_t			ForwardPosition() { return UpPosition() + UpSize(); }
	XM_CONSTEXPR inline static uint32_t			ForwardSize() { return sizeof(V4); }
	XM_CONSTEXPR inline static const char*		ForwardName() { return "INSTANCE_FORWARD"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		ForwardFormat() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }

	V4 position;			// float4			INSTANCE_POSITION
	XM_CONSTEXPR inline static uint32_t			PositionPosition() { return ForwardPosition() + ForwardSize(); }
	XM_CONSTEXPR inline static uint32_t			PositionSize() { return sizeof(V4); }
	XM_CONSTEXPR inline static const char*		PositionName() { return "INSTANCE_POSITION"; }
	XM_CONSTEXPR inline static DXGI_FORMAT		PositionFormat() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }

	XM_CONSTEXPR inline static const char*		InstanceDataName() { return "INSTANCE_DATA"; }

	Real4 instanceData0;	// float4			INSTANCE_DATA0 or INSTNACE_DATA
	XM_CONSTEXPR inline static uint32_t			InstanceData0Position() { return PositionPosition() + PositionSize(); }
	XM_CONSTEXPR inline static uint32_t			InstanceData0Size() { return sizeof(V4); }
	XM_CONSTEXPR inline static UINT				InstanceData0Index() { return 0; }
	XM_CONSTEXPR inline static DXGI_FORMAT		InstanceData0Format() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }

	Real4 instanceData1;	// float4			INSTANCE_DATA1
	XM_CONSTEXPR inline static uint32_t			InstanceData1Position() { return InstanceData0Position() + InstanceData0Size(); }
	XM_CONSTEXPR inline static uint32_t			InstanceData1Size() { return sizeof(V4); }
	XM_CONSTEXPR inline static UINT				InstanceData1Index() { return 1; }
	XM_CONSTEXPR inline static DXGI_FORMAT		InstanceData1Format() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }

	Real4 instanceData2;	// float4			INSTANCE_DATA2
	XM_CONSTEXPR inline static uint32_t			InstanceData2Position() { return InstanceData1Position() + InstanceData1Size(); }
	XM_CONSTEXPR inline static uint32_t			InstanceData2Size() { return sizeof(V4); }
	XM_CONSTEXPR inline static UINT				InstanceData2Index() { return 2; }
	XM_CONSTEXPR inline static DXGI_FORMAT		InstanceData2Format() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }

	Real4 instanceData3;	// float4			INSTANCE_DATA3
	XM_CONSTEXPR inline static uint32_t			InstanceData3Position() { return InstanceData2Position() + InstanceData2Size(); }
	XM_CONSTEXPR inline static uint32_t			InstanceData3Size() { return sizeof(V4); }
	XM_CONSTEXPR inline static UINT				InstanceData3Index() { return 3; }
	XM_CONSTEXPR inline static DXGI_FORMAT		InstanceData3Format() { return DXGI_FORMAT_R32G32B32A32_FLOAT; }
};

enum class PrimitiveTopology
{
	Triangles = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
	Lines = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
	LineStrip = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
	Points = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
};

struct ENGINE_API IndicesStream
{
	uint Count = 0;
	Index* Indices = nullptr;

	inline IndicesStream() {}

	inline explicit IndicesStream(Index* indices, uint count)
	{
		this->Indices = indices;
		this->Count = count;
	}

	inline IndicesStream& operator = (const IndicesStream& rhs)
	{
		SafeDeleteArray(this->Indices);
		this->Indices = new Index[rhs.Count];
		memcpy(this->Indices, rhs.Indices, sizeof(Index) * rhs.Count);
		this->Count = rhs.Count;
		return *this;
	}

	inline IndicesStream& operator = (IndicesStream&& rhs) noexcept
	{
		SafeDeleteArray(this->Indices);
		this->Indices = rhs.Indices;
		this->Count = rhs.Count;
		rhs.Indices = nullptr;
		return *this;
	}

	inline IndicesStream(const IndicesStream& rhs)
	{
		*this = rhs;
	}

	inline IndicesStream(IndicesStream&& rhs) noexcept
	{
		*this = rhs;
	}

	inline ~IndicesStream()
	{
		SafeDeleteArray(Indices);
	}
};

ENGINE_END

