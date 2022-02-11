#pragma once

#include "Component.h"
#include "TransformData.h"

ENGINE_BEGIN
class ENGINE_API Transform final : public Component
{
public:

	void SetPosition(const V3& position);

	void SetRotation(const Q& rotation);

	void SetEulerAngles(const V3& eulerAngles);

	void SetLocalPosition(const V3& localPosition);

	void SetLocalRotation(const Q& localRotation);

	void SetLocalEulerAngles(const V3& localEulerAngles);

	void SetLocalScale(const V3& localScale);

	void SetLocalTransformation(const V3& position, const Q& rotation, const V3& scale);

	void SetLocalTransformation(const V3& position, const V3& eulerAngles, const V3& scale);

	const V3& GetPosition() const;

	const Q& GetRotation() const;

	V3 GetEulerAngles() const;

	const V3& GetLossyScale() const;

	const V3& GetLocalPosition() const;

	const Q& GetLocalRotation() const;

	V3 GetLocalEulerAngles() const;

	const V3& GetLocalScale() const;

	void SetRightDirection(const V3& right);

	void SetUpDirection(const V3& up);

	void SetForwardDirection(const V3& forward);

	V3 GetRightDirection() const;

	V3 GetUpDirection() const;

	V3 GetForwardDirection() const;

	bool IsImmediateMode() const;

	void SetImmediateMode(bool value);

	const M4& GetImmediateLocalToWorldMatrix() const;

	void SetImmediateLocalToWorldMatrix(const M4& value);

	_declspec(property(get = GetPosition, put = SetPosition)) const V3& position;
	_declspec(property(get = GetRotation, put = SetRotation)) const Q& rotation;
	_declspec(property(get = GetEulerAngles, put = SetEulerAngles)) V3 eulerAngles;
	_declspec(property(get = GetLossyScale)) const V3& lossyScale;
	_declspec(property(get = GetLocalPosition, put = SetLocalPosition)) const V3& localPosition;
	_declspec(property(get = GetLocalRotation, put = SetLocalRotation)) const Q& localRotation;
	_declspec(property(get = GetLocalEulerAngles, put = SetLocalEulerAngles)) V3 localEulerAngles;
	_declspec(property(get = GetLocalScale, put = SetLocalScale)) const V3& localScale;
	_declspec(property(get = GetRightDirection, put = SetRightDirection)) V3 right;
	_declspec(property(get = GetUpDirection, put = SetUpDirection)) V3 up;
	_declspec(property(get = GetForwardDirection, put = SetForwardDirection)) V3 forward;
	_declspec(property(get = IsImmediateMode, put = SetImmediateMode)) bool immediateMode;
	_declspec(property(get = GetImmediateLocalToWorldMatrix, put = SetImmediateLocalToWorldMatrix)) const M4& immediateMatrix;

public:

	const M4& GetLocalToWorldMatrix() const;

	M4 GetWorldToLocalMatrix() const;

	_declspec(property(get = GetLocalToWorldMatrix)) M4 localToWorldMatrix;
	_declspec(property(get = GetWorldToLocalMatrix)) const M4& worldToLocalMatrix;

public:

	void SetActiveSelf(bool active);

	inline bool IsActiveSelf() const { return m_activeSelf; }

	inline bool IsActiveInHierarchy() const { return m_activeSelf&& m_activeInTree; }

	void SetParent(Transform* parent);

	void DetachFromParent();

	void AddChild(Transform* child);

	void RemoveChild(Transform* child);

	Transform* GetRoot() const;

	Transform* GetParent() const;

	size_t GetChildCount() const;

	Transform* GetChild(size_t index) const;

	_declspec(property(get = IsActiveSelf, put = SetActiveSelf)) bool activeSelf;
	_declspec(property(get = IsActiveInHierarchy)) bool activeInHierarchy;
	_declspec(property(get = GetRoot)) Transform* root;
	_declspec(property(get = GetParent, put = SetParent)) Transform* parent;
	_declspec(property(get = GetChildCount)) size_t childCount;

public:

	void LookAt(const V3& targetWorldPosition);

	void LookAt(const V3& targetWorldPosition, const V3& up);

	void RotateSelf(const V3& localEulerAngles);

	void RotateWorld(const V3& eulerAngles);

	void RotateAround(const V3& targetWorldPosition, const V3& axis, float angle);

	void TranslateSelf(const V3& translation);

	void TranslateWorld(const V3& translation);

private:

	virtual void OnDestroyed();

	void ApplyAcive();

	void UpdateActiveInTree();

	void UpdateChildWorldTransforms();

	void UpdateLocalTransform();

	void UpdateWorldTransform();

private:

	TransformData					m_localData;

	TransformData					m_worldData;

	M4								m_localToWorld = M4::identity();

	Transform*						m_parent = nullptr;

	vector<Transform*>				m_childs;

	bool							m_activeSelf = true;

	bool							m_activeInTree = true;

	// It's sets to return specified local to world matrix
	bool							m_immediateMode = false;

	// It's valid value on actived immediate mode
	M4								m_immediateLocalToWorldMatrix = M4::identity();
};
ENGINE_END
