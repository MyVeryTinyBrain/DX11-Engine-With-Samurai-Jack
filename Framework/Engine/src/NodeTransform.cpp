#include "EnginePCH.h"
#include "NodeTransform.h"
#include "Node.h"
#include "Transform.h"

NodeTransform::NodeTransform(Transform* basedTransform, NodeTransform* root, NodeTransform* parent, const Node* node) :
	m_basedTransform(basedTransform),
	Object(node->name),
	m_parent(parent),
	m_node(node)
{
	// Root NodeTransform이 비어있다면 이 NodeTransform을 Root NodeTransform으로 인식합니다.
	m_root = root;
	if (!m_root)
		m_root = this;

	if (m_parent)
		m_parent->m_childs.push_back(this);

	m_transformationMatrix = m_node->transformationMatrix;
	m_combinedMatrix = m_node->GetDefaultCombinedMatrix();

	m_localData.position = m_transformationMatrix.GetTranslation();
	m_localData.rotation = m_transformationMatrix.GetRotation();
	m_localData.eulerAngles = m_localData.rotation.eulerAngles;
	m_localData.scale = m_transformationMatrix.GetScale();
}

const Node* NodeTransform::GetNode() const
{
	return m_node;
}

const M4& NodeTransform::GetOffsetMatrix() const
{
	return m_node->offsetMatrix;
}

M4 NodeTransform::GetLocalToWorldMatrix() const
{
	// return GetBoneMatrix() * m_basedTransform->localToWorldMatrix;
	// return GetOffsetMatrix() * GetCombinedMatrix() * m_basedTransform->localToWorldMatrix;
	// 정점을 변환하기 위한 행렬이 아닌 경우에는 OffsetMatrix를 곱하지 않습니다.
	return GetCombinedMatrix() * m_basedTransform->localToWorldMatrix;
}

V3 NodeTransform::GetPosition() const
{
	return localToWorldMatrix.GetTranslation();
}

Q NodeTransform::GetRotation() const
{
	if (m_parent)
		return m_localData.rotation * m_parent->GetRotation();
	else
		return m_localData.rotation * m_basedTransform->GetRotation();
}

V3 NodeTransform::GetEulerAngles() const
{
	return GetRotation().eulerAngles;
}

V3 NodeTransform::GetScale() const
{
	float sx = 0, sy = 0, sz = 0;

	const M4& localToWorld = localToWorldMatrix;

	if (m_parent)
	{
		const V3& parentScale = m_parent->GetScale();
		float sx = ((V3)localToWorld.GetRow(0)).magnitude * Sign(m_localData.scale.x * parentScale.x);
		float sy = ((V3)localToWorld.GetRow(1)).magnitude * Sign(m_localData.scale.y * parentScale.y);
		float sz = ((V3)localToWorld.GetRow(2)).magnitude * Sign(m_localData.scale.z * parentScale.z);
	}
	else
	{
		float sx = ((V3)localToWorld.GetRow(0)).magnitude * Sign(m_localData.scale.x);
		float sy = ((V3)localToWorld.GetRow(1)).magnitude * Sign(m_localData.scale.y);
		float sz = ((V3)localToWorld.GetRow(2)).magnitude * Sign(m_localData.scale.z);
	}

	return V3(sx, sy, sz);
}

V3 NodeTransform::GetRight() const
{
	return GetRotation().MultiplyVector(V3::right());
}

V3 NodeTransform::GetUp() const
{
	return GetRotation().MultiplyVector(V3::up());
}

V3 NodeTransform::GetForward() const
{
	return GetRotation().MultiplyVector(V3::forward());
}

void NodeTransform::SetPosition(const V3& position)
{
	M4 worldTransformation = M4::SRT(GetScale(), GetRotation(), position);
	M4 localTransformation = worldTransformation * m_parent->worldToLocalMatrix;

	SetLocalPosition(localTransformation.GetTranslation());
}

void NodeTransform::SetRotation(const Q& rotation)
{
	SetLocalRotation(rotation * m_parent->GetRotation().inversed);
}

void NodeTransform::SetEulerAngles(const V3& eulerAngles)
{
	SetRotation(Q::Euler(eulerAngles));
}

void NodeTransform::SetRotationWithDefaultRotation(const Q& rotation)
{
	SetRotation(GetDefaultRotation() * rotation);
}

void NodeTransform::SetEulerAnglesWithDefaultRotation(const V3& eulerAngles)
{
	SetRotation(GetDefaultRotation() * Q::Euler(eulerAngles));
}

void NodeTransform::SetRotationWithCurrentRotation(const Q& rotation)
{
	SetRotation(GetRotation() * rotation);
}

void NodeTransform::SetEulerAnglesWithCurrentRotation(const V3& eulerAngles)
{
	SetRotation(GetRotation() * Q::Euler(eulerAngles));
}

void NodeTransform::SetLocalPosition(const V3& position)
{
	m_localData.position = position;

	UpdateTransformationMatrix();
}

void NodeTransform::SetLocalRotation(const Q& rotation)
{
	m_localData.rotation = rotation;
	m_localData.eulerAngles = m_localData.rotation.eulerAngles;

	UpdateTransformationMatrix();
}

void NodeTransform::SetLocalEulerAngles(const V3& eulerAngles)
{
	m_localData.rotation = Q::Euler(eulerAngles);
	m_localData.eulerAngles = eulerAngles;

	UpdateTransformationMatrix();
}

void NodeTransform::SetLocalScale(const V3& scale)
{
	m_localData.scale = scale;

	UpdateTransformationMatrix();
}

void NodeTransform::SetLocalTransformation(const V3& position, const Q& rotation, const V3& scale)
{
	m_localData.position = position;
	m_localData.rotation = rotation;
	m_localData.eulerAngles = m_localData.rotation.eulerAngles;
	m_localData.scale = scale;

	UpdateTransformationMatrix();
}

V3 NodeTransform::GetDefaultLocalPosition() const
{
	return m_node->transformationMatrix.GetTranslation();
}

Q NodeTransform::GetDefaultLocalRotation() const
{
	return m_node->transformationMatrix.GetRotation();
}

V3 NodeTransform::GetDefaultLocalEulerAngles() const
{
	return m_node->transformationMatrix.GetEulerAngles();
}

V3 NodeTransform::GetDefaultLocalScale() const
{
	return m_node->transformationMatrix.GetScale();
}

V3 NodeTransform::GetDefaultPosition() const
{
	return (m_node->GetDefaultCombinedMatrix() * m_basedTransform->localToWorldMatrix).GetTranslation();
}

Q NodeTransform::GetDefaultRotation() const
{
	return Q(m_node->GetDefaultCombinedMatrix().GetRotation()) * m_basedTransform->rotation;
}

V3 NodeTransform::GetDefaultEulerAngles() const
{
	return GetDefaultRotation().eulerAngles;
}

V3 NodeTransform::GetDefaultScale() const
{
	float sx = 0, sy = 0, sz = 0;

	M4 localToWorld = m_node->GetDefaultCombinedMatrix() * m_basedTransform->localToWorldMatrix;

	if (m_parent)
	{
		const V3& parentScale = m_parent->GetScale();
		float sx = ((V3)localToWorld.GetRow(0)).magnitude * Sign(m_localData.scale.x * parentScale.x);
		float sy = ((V3)localToWorld.GetRow(1)).magnitude * Sign(m_localData.scale.y * parentScale.y);
		float sz = ((V3)localToWorld.GetRow(2)).magnitude * Sign(m_localData.scale.z * parentScale.z);
	}
	else
	{
		float sx = ((V3)localToWorld.GetRow(0)).magnitude * Sign(m_localData.scale.x);
		float sy = ((V3)localToWorld.GetRow(1)).magnitude * Sign(m_localData.scale.y);
		float sz = ((V3)localToWorld.GetRow(2)).magnitude * Sign(m_localData.scale.z);
	}

	return V3(sx, sy, sz);
}

V3 NodeTransform::GetDefaultRight() const
{
	return GetDefaultRotation().MultiplyVector(V3::right());
}

V3 NodeTransform::GetDefaultUp() const
{
	return GetDefaultRotation().MultiplyVector(V3::up());
}

V3 NodeTransform::GetDefaultForward() const
{
	return GetDefaultRotation().MultiplyVector(V3::forward());
}

void NodeTransform::UpdateTransformationMatrix()
{
	m_transformationMatrix = M4::SRT(m_localData.scale, m_localData.rotation, m_localData.position);
}

void NodeTransform::UpdateCombinedMatrixSelf()
{
	// Parent의 CombinedMatrix가 갱신되었다고 가정하며 이 NodeTransform의 CombinedMatrix를 재설정합니다.

	if (m_parent)
	{
		m_combinedMatrix = m_transformationMatrix * m_parent->m_combinedMatrix;
	}
	else
	{
		m_combinedMatrix = m_transformationMatrix;
	}
}

void NodeTransform::UpdateCombinedMatricesFromRootToThis()
{
	// 트리의 Root부터 이 NodeTransform까지의 CombinedMatrix를 재설정합니다.

	if (m_parent)
	{
		m_parent->UpdateCombinedMatricesFromRootToThis();
		m_combinedMatrix = m_transformationMatrix * m_parent->m_combinedMatrix;
	}
	else
	{
		m_combinedMatrix = m_transformationMatrix;
	}
}

void NodeTransform::UpdateCombinedMatricesFromRoot()
{
	// 트리의 모든 NodeTransform들의 CombinedMatrix를 재설정합니다.

	if (!m_root)
		return;

	stack<NodeTransform*> s;
	s.push(m_root);

	while (!s.empty())
	{
		NodeTransform* nodeTransform = s.top();
		s.pop();

		nodeTransform->UpdateCombinedMatrixSelf();

		for (auto& child : nodeTransform->m_childs)
		{
			s.push(child);
		}
	}
}
