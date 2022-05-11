#pragma once

#include "VIType.h"

ENGINE_BEGIN

struct ENGINE_API SubIndicesDesc
{
	// 인덱스 배열을 복제하여 저장합니다.
	inline SubIndicesDesc(const Index* subIncides, UINT count)
	{
		this->Count = count;
		this->SubIncides = new Index[count]{};
		memcpy(this->SubIncides, subIncides, sizeof(Index) * count);
	}

	// 인덱스 배열을 복제하지 않고 주소를 저장합니다.
	inline SubIndicesDesc(Index* subIncides, UINT count)
	{
		this->Count = count;
		this->SubIncides = subIncides;
	}

	inline SubIndicesDesc& operator = (const SubIndicesDesc& rhs)
	{
		SafeDeleteArray(SubIncides);
		this->Count = rhs.Count;
		if (rhs.SubIncides)
		{
			this->SubIncides = new Index[rhs.Count]{};
			memcpy(this->SubIncides, rhs.SubIncides, sizeof(Index) * rhs.Count);
		}
		return *this;
	}

	inline SubIndicesDesc& operator = (SubIndicesDesc&& rhs) noexcept
	{
		SafeDeleteArray(SubIncides);
		this->Count = rhs.Count;
		this->SubIncides = rhs.SubIncides;
		rhs.SubIncides = nullptr;
		return *this;
	}

	inline SubIndicesDesc(const SubIndicesDesc& rhs)
	{
		*this = rhs;
	}

	inline SubIndicesDesc(SubIndicesDesc&& rhs) noexcept
	{
		*this = rhs;
	}

	inline ~SubIndicesDesc() { SafeDeleteArray(SubIncides); }

	UINT Count = 0;

	Index* SubIncides = nullptr;
};

ENGINE_END
