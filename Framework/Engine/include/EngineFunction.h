#pragma once

ENGINE_BEGIN

template <typename T>
inline void SafeDelete(T*& ptr)
{
	if (ptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}

template <typename T>
inline void SafeDeleteArray(T*& arr)
{
	if (arr)
	{
		delete[] arr;
		arr = nullptr;
	}
}

template <typename T>
inline unsigned long SafeAddRef(T& instance)
{
	if (nullptr == instance)
		return 0;

	unsigned long dwRefCnt = 0;

	dwRefCnt = instance->AddRef();

	return dwRefCnt;
}

template <typename T>
inline unsigned long SafeRelease(T& instance)
{
	unsigned long dwRefCnt = 0;

	if (nullptr != instance)
	{
		dwRefCnt = instance->Release();

		instance = nullptr;
	}

	return dwRefCnt;
}

ENGINE_END
