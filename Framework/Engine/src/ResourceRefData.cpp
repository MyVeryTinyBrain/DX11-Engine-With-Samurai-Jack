#include "EnginePCH.h"
#include "ResourceRefData.h"
#include "ResourceObject.h"

ResourceRefData::ResourceRefData(ResourceObject* vp)
{
    if (!vp)
    {
        return;
    }

    m_obj = vp;
    m_pRefCount = new size_t(0);
    m_pIsDestroyed = new bool(false);
}

ResourceRefData::ResourceRefData(const ResourceRefData& other)
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

ResourceRefData::~ResourceRefData()
{
	Reset();
}

ResourceRefData& ResourceRefData::operator=(const ResourceRefData& other)
{
    Reset();

    if (!other.m_obj) return *this;

    m_obj = other.m_obj;
    m_pRefCount = other.m_pRefCount;
    m_pIsDestroyed = other.m_pIsDestroyed;

    ++(*m_pRefCount);

    return *this;
}

bool ResourceRefData::operator==(const ResourceRefData& other) const
{
    return m_obj == other.m_obj;
}

bool ResourceRefData::operator!=(const ResourceRefData& other) const
{
    return m_obj != other.m_obj;
}

size_t ResourceRefData::GetReferenceCount() const
{
    return *m_pRefCount;
}

void ResourceRefData::SetToDestroyed()
{
    if (IsNull())
    {
        return;
    }

    (*m_pIsDestroyed) = true;
}

void ResourceRefData::Reset()
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

bool ResourceRefData::IsNull() const
{
    if (!m_obj) return true;
    else        return (*m_pIsDestroyed);
}

ResourceObject* ResourceRefData::GetResourceObject() const
{
    return m_obj;
}
