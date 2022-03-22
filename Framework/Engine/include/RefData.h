#pragma once

ENGINE_BEGIN
class Object;
class ENGINE_API RefData final
{
public:

	// Constructors ===========================================

	explicit RefData(class Object* vp);

	explicit RefData(const RefData& other);

	// Destructor =============================================

	~RefData();

	// Operators ==============================================

	RefData& operator = (const RefData& other);

	bool operator == (const RefData& other) const;

	bool operator != (const RefData& other) const;

	// Member Variables =======================================

	uint GetReferenceCount() const;

	_declspec(property(get = GetReferenceCount)) uint referenceCount;

	// Member Functions =======================================

	void SetToDestroyed();

	void Reset();

	bool IsNull() const;

	Object* GetObject() const;

	_declspec(property(get = IsNull)) bool isNull;

private:

	// Member Variables =======================================

	Object* m_obj = nullptr;

	uint* m_pRefCount = nullptr;

	bool* m_pIsDestroyed = nullptr;
};
ENGINE_END