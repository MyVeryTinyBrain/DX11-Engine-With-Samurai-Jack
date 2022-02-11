#pragma once

ENGINE_BEGIN
class ResourceObject;
class ENGINE_API ResourceRefData final
{
public:

	// Constructors ===========================================

	explicit ResourceRefData(class ResourceObject* vp);

	explicit ResourceRefData(const ResourceRefData& other);

	// Destructor =============================================

	~ResourceRefData();

	// Operators ==============================================

	ResourceRefData& operator = (const ResourceRefData& other);

	bool operator == (const ResourceRefData& other) const;

	bool operator != (const ResourceRefData& other) const;

	// Member Variables =======================================

	size_t GetReferenceCount() const;

	_declspec(property(get = GetReferenceCount)) size_t referenceCount;

	// Member Functions =======================================

	void SetToDestroyed();

	void Reset();

	bool IsNull() const;

	ResourceObject* GetResourceObject() const;

	_declspec(property(get = IsNull)) bool isNull;

private:

	// Member Variables =======================================

	ResourceObject* m_obj = nullptr;

	size_t* m_pRefCount = nullptr;

	bool* m_pIsDestroyed = nullptr;
};
ENGINE_END