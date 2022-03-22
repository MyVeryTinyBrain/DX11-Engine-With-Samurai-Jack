#include "EnginePCH.h"
#include "RefData.h"
#include "Object.h"

RefData::RefData(Object* vp)
{
    if (!vp)
    {
        return;
    }

    m_obj = vp;
    m_pRefCount = new uint(0);
    m_pIsDestroyed = new bool(false);
}

RefData::RefData(const RefData& other)
{
    if (!other.m_obj)
    {
        return;
    }

    m_obj = other.m_obj;
    m_pRefCount = other.m_pRefCount;
    m_pIsDestroyed = other.m_pIsDestroyed;

    ++(*m_pRefCount);
}

RefData::~RefData()
{
	Reset();
}

RefData& RefData::operator=(const RefData& other)
{
    Reset();

    if (!other.m_obj) return *this;

    m_obj = other.m_obj;
    m_pRefCount = other.m_pRefCount;
    m_pIsDestroyed = other.m_pIsDestroyed;

    ++(*m_pRefCount);

    return *this;
}

bool RefData::operator==(const RefData& other) const
{
    return m_obj == other.m_obj;
}

bool RefData::operator!=(const RefData& other) const
{
    return m_obj != other.m_obj;
}

uint RefData::GetReferenceCount() const
{
	return *m_pRefCount;
}

void RefData::SetToDestroyed()
{
    if (IsNull())
    {
        return;
    }

    (*m_pIsDestroyed) = true;
}

void RefData::Reset()
{
    if (!m_obj)
    {
        return;
    }

    auto& refCount = *m_pRefCount;
    auto& isDestroyed = *m_pIsDestroyed;

    if (refCount == 0)
    {
        if (isDestroyed)
        {
            SafeDelete(m_pRefCount);
            SafeDelete(m_pIsDestroyed);
        }
    }
    else
    {
        --(*m_pRefCount);
    }

    m_obj = nullptr;
    m_pRefCount = nullptr;
    m_pIsDestroyed = nullptr;
}

bool RefData::IsNull() const
{
    if (!m_obj) return true;
    else        return (*m_pIsDestroyed);
}

Object* RefData::GetObject() const
{
    return m_obj;
}
