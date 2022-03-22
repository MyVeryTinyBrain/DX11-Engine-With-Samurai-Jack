#pragma once

#include "Object.h"
#include "TransformData.h"
#include "Ref.h"

ENGINE_BEGIN
class Node;
class Transform;
class ENGINE_API NodeTransform : public Object
{

public:

	NodeTransform(Transform* basedTransform, NodeTransform* root, NodeTransform* parent, const Node* node);

public:

	inline bool IsRoot() const { return m_root == this; }

	inline Ref<NodeTransform> GetRoot() const { return m_root; }

	inline Ref<NodeTransform> GetParent() const { return m_parent; }

	inline uint GetChildCount() const { return uint(m_childs.size()); }

	inline Ref<NodeTransform> GetChild(uint index) const { return m_childs[index]; }

	const Node* GetNode() const;

	const M4& GetOffsetMatrix() const;

	inline const M4& GetTransformationMatrix() const { return m_transformationMatrix; }

	inline const M4& GetCombinedMatrix() const { return m_combinedMatrix; }

	inline M4 GetBoneMatrix() const { return GetOffsetMatrix() * GetCombinedMatrix(); }

	M4 GetLocalToWorldMatrix() const;

	inline M4 GetWorldToLocalMatrix() const { return GetLocalToWorldMatrix().inversed; }

	_declspec(property(get = IsRoot)) bool isRoot;
	_declspec(property(get = GetRoot)) Ref<NodeTransform> root;
	_declspec(property(get = GetParent)) Ref<NodeTransform> parernt;
	_declspec(property(get = GetChildCount)) uint childCount;
	_declspec(property(get = GetNode)) const Node* node;
	_declspec(property(get = GetOffsetMatrix)) const M4& offsetMatrix;
	_declspec(property(get = GetTransformationMatrix)) const M4& transformationMatrix;
	_declspec(property(get = GetCombinedMatrix)) const M4& combinedMatrix;
	_declspec(property(get = GetBoneMatrix)) M4 boneMatrix;
	_declspec(property(get = GetLocalToWorldMatrix)) M4 localToWorldMatrix;
	_declspec(property(get = GetWorldToLocalMatrix)) M4 worldToLocalMatrix;

public:

	V3 GetPosition() const;

	Q GetRotation() const;

	V3 GetEulerAngles() const;

	V3 GetScale() const;

	V3 GetRight() const;

	V3 GetUp() const;

	V3 GetForward() const;

	inline const V3& GetLocalPosition() const { return m_localData.position; }

	inline const Q& GetLocalRotation() const { return m_localData.rotation; }

	inline const V3& GetLocalEulerAngles() const { return m_localData.eulerAngles; }

	inline const V3& GetLocalScale() const { return m_localData.scale; }

	void SetPosition(const V3& position);

	void SetRotation(const Q& rotation);

	void SetEulerAngles(const V3& eulerAngles);

	void SetRotationWithDefaultRotation(const Q& rotation);

	void SetEulerAnglesWithDefaultRotation(const V3& eulerAngles);

	void SetRotationWithCurrentRotation(const Q& rotation);

	void SetEulerAnglesWithCurrentRotation(const V3& eulerAngles);

	void SetLocalPosition(const V3& position);

	void SetLocalRotation(const Q& rotation);

	void SetLocalEulerAngles(const V3& eulerAngles);

	void SetLocalScale(const V3& scale);

	void SetLocalTransformation(const V3& position, const Q& rotation, const V3& scale);

	_declspec(property(get = GetPosition, put = SetPosition)) const V3& position;
	_declspec(property(get = GetRotation, put = SetRotation)) const Q& rotation;
	_declspec(property(get = GetEulerAngles, put = SetEulerAngles)) const V3& eulerAngles;
	_declspec(property(get = GetScale)) const V3& scale;
	_declspec(property(get = GetRight)) V3 right;
	_declspec(property(get = GetUp)) V3 up;
	_declspec(property(get = GetForward)) V3 forward;
	_declspec(property(get = GetLocalPosition, put = SetLocalPosition)) const V3& localPosition;
	_declspec(property(get = GetLocalRotation, put = SetLocalRotation)) const Q& localRotation;
	_declspec(property(get = GetLocalEulerAngles, put = SetLocalEulerAngles)) const V3& localEulerAngles;
	_declspec(property(get = GetLocalScale, put = SetLocalScale)) const V3& localScale;

public:

	V3 GetDefaultLocalPosition() const;

	Q GetDefaultLocalRotation() const;

	V3 GetDefaultLocalEulerAngles() const;

	V3 GetDefaultLocalScale() const;

	V3 GetDefaultPosition() const;

	Q GetDefaultRotation() const;

	V3 GetDefaultEulerAngles() const;

	V3 GetDefaultScale() const;

	V3 GetDefaultRight() const;

	V3 GetDefaultUp() const;

	V3 GetDefaultForward() const;

	_declspec(property(get = GetDefaultLocalPosition)) V3 defaultLocalPosition;
	_declspec(property(get = GetDefaultLocalRotation)) Q defaultLocalRotation;
	_declspec(property(get = GetDefaultLocalEulerAngles)) V3 defaultLocalEulerAngles;
	_declspec(property(get = GetDefaultLocalScale)) V3 defaultLocalScale;
	_declspec(property(get = GetDefaultPosition)) V3 defaultPosition;
	_declspec(property(get = GetDefaultRotation)) Q defaultRotation;
	_declspec(property(get = GetDefaultEulerAngles)) V3 defaultEulerAngles;
	_declspec(property(get = GetDefaultScale)) V3 defaultScale;
	_declspec(property(get = GetDefaultRight)) V3 defaultRight;
	_declspec(property(get = GetDefaultUp)) V3 defaultUp;
	_declspec(property(get = GetDefaultForward)) V3 defaultForward;

public:

	void UpdateTransformationMatrix();

	void UpdateCombinedMatrixSelf();

	void UpdateCombinedMatricesFromRootToThis();

	void UpdateCombinedMatricesFromRoot();

private:

	Transform*					m_basedTransform = nullptr;

	NodeTransform*				m_root = nullptr;

	NodeTransform*				m_parent = nullptr;

	vector<NodeTransform*>		m_childs;

	const Node*					m_node = nullptr;

	TransformData				m_localData = {};

	M4							m_transformationMatrix = M4::identity();

	M4							m_combinedMatrix = M4::identity();

};

ENGINE_END
