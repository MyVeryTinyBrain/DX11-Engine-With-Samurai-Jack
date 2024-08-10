#include "EnginePCH.h"
#include "Transform.h"
#include "GameObject.h"

#define SCALE_EPSILON		(0.001f)

void Transform::SetPosition(const V3& position)
{
	m_worldData.position = position;

	UpdateLocalTransform();
	UpdateChildWorldTransforms();
}

void Transform::SetRotation(const Q& rotation)
{
	m_worldData.rotation = rotation;
	m_worldData.eulerAngles = rotation.eulerAngles;

	UpdateLocalTransform();
	UpdateChildWorldTransforms();
}

void Transform::SetEulerAngles(const V3& eulerAngles)
{
	m_worldData.rotation = Q::Euler(eulerAngles);
	m_worldData.eulerAngles = eulerAngles;

	UpdateLocalTransform();
	UpdateChildWorldTransforms();
}

void Transform::SetLocalPosition(const V3& localPosition)
{
	if (m_localData.position == localPosition)
		return;

	m_localData.position = localPosition;

	UpdateWorldTransform();
	UpdateChildWorldTransforms();
}

void Transform::SetLocalRotation(const Q& localRotation)
{
	if (m_localData.rotation == localRotation)
		return;

	m_localData.rotation = localRotation;
	m_localData.eulerAngles = localRotation.eulerAngles;

	UpdateWorldTransform();
	UpdateChildWorldTransforms();
}

void Transform::SetLocalEulerAngles(const V3& localEulerAngles)
{
	if (m_localData.eulerAngles == localEulerAngles)
		return;

	m_localData.rotation = Q::Euler(localEulerAngles);
	m_localData.eulerAngles = localEulerAngles;

	UpdateWorldTransform();
	UpdateChildWorldTransforms();
}

void Transform::SetLocalScale(const V3& localScale)
{
	if (m_localData.scale == localScale)
		return;

	m_localData.scale = localScale;

	UpdateWorldTransform();
	UpdateChildWorldTransforms();
}

void Transform::SetLocalTransformation(const V3& position, const Q& rotation, const V3& scale)
{
	if (m_localData.position == position && 
		m_localData.rotation == rotation && 
		m_localData.scale == scale)
		return;

	m_localData.position = position;
	m_localData.rotation = rotation;
	m_localData.eulerAngles = rotation.eulerAngles;
	m_localData.scale = scale;

	UpdateWorldTransform();
	UpdateChildWorldTransforms();
}

void Transform::SetLocalTransformation(const V3& position, const V3& eulerAngles, const V3& scale)
{
	if (m_localData.position == position &&
		m_localData.eulerAngles == eulerAngles &&
		m_localData.scale == scale)
		return;

	m_localData.position = position;
	m_localData.rotation = Q::Euler(eulerAngles);
	m_localData.eulerAngles = eulerAngles;
	m_localData.scale = scale;

	UpdateWorldTransform();
	UpdateChildWorldTransforms();
}

const V3& Transform::GetPosition() const
{
	return m_worldData.position;
}

const Q& Transform::GetRotation() const
{
	return m_worldData.rotation;
}

V3 Transform::GetEulerAngles() const
{
	return m_worldData.eulerAngles;
}

const V3& Transform::GetLossyScale() const
{
	return m_worldData.scale;
}

const V3& Transform::GetLocalPosition() const
{
	return m_localData.position;
}

const Q& Transform::GetLocalRotation() const
{
	return m_localData.rotation;
}

V3 Transform::GetLocalEulerAngles() const
{
	return m_localData.eulerAngles;
}

const V3& Transform::GetLocalScale() const
{
	return m_localData.scale;
}

void Transform::SetRightDirection(const V3& right)
{
	V3 x = right.normalized;
	if (x.sqrMagnitude == 0)
	{
		x = V3::right();
	}
	SetRotation(Q::FromToRotation(V3::right(), x));
}

void Transform::SetUpDirection(const V3& up)
{
	V3 y = up.normalized;
	if (y.sqrMagnitude == 0)
	{
		y = V3::up();
	}
	SetRotation(Q::FromToRotation(V3::up(), y));
}

void Transform::SetForwardDirection(const V3& forward)
{
	V3 z = forward.normalized;
	if (z.sqrMagnitude == 0)
	{
		z = V3::forward();
	}
	V3 up = V3::up();
	SetRotation(Q::LookRotation(z, up));
}

V3 Transform::GetRightDirection() const
{
	return m_worldData.rotation.MultiplyVector(V3::right());
}

V3 Transform::GetUpDirection() const
{
	return m_worldData.rotation.MultiplyVector(V3::up());
}

V3 Transform::GetForwardDirection() const
{
	return m_worldData.rotation.MultiplyVector(V3::forward());
}

bool Transform::IsImmediateMode() const
{
	return m_immediateMode;
}

void Transform::SetImmediateMode(bool value)
{
	m_immediateMode = value;
}

const M4& Transform::GetImmediateLocalToWorldMatrix() const
{
	return m_immediateLocalToWorldMatrix;
}

void Transform::SetImmediateLocalToWorldMatrix(const M4& value)
{
	m_immediateLocalToWorldMatrix = value;
}

const M4& Transform::GetLocalToWorldMatrix() const
{
	if (m_immediateMode)
		return m_immediateLocalToWorldMatrix;
	else
		return m_localToWorld;
}

M4 Transform::GetWorldToLocalMatrix() const
{
	if (m_immediateMode)
		return m_immediateLocalToWorldMatrix.inversed;
	else
		return m_localToWorld.inversed;
}

void Transform::SetActiveSelf(bool active)
{
	if (m_activeSelf == active)
		return;

	m_activeSelf = active;

	UpdateActiveInTree();

	// ���� Ȱ��ȭ ���°� ����Ǿ����Ƿ� �Ʒ� �Լ��� ȣ���մϴ�.
	ApplyAcive();
}

void Transform::SetParent(Transform* parent)
{	
	// �� Ʈ�������� �θ�� �η��� ��� �����մϴ�.
	if (parent == this) 
		return;

	// �켱, �θ𿡼� Ż���մϴ�.
	DetachFromParent();

	if (parent)
	{
		// ��ϵǷ��� �θ� �ִٸ�
		// ���ο� �θ��� �ڽ����� ����մϴ�.
		parent->AddChild(this);
	}
}

void Transform::DetachFromParent()
{
	if (!m_parent)
		return;

	// �θ� �ִٸ� �θ𿡰Լ� Ż���մϴ�.
	m_parent->RemoveChild(this);
}

void Transform::AddChild(Transform* child)
{
	// �� Ʈ�������� �ڽ����� �η��� ��� �����մϴ�.
	if (child == this) return;
	if (!child) return;
	// =====================================================================
	// ���� ��ȯ�� �߻��� �� �ִ� ��� �θ𿡼� Ż���մϴ�.
	// A-->B-->C (ȭ��ǥ�� ����Ű�� ����� �ڽ��� ��Ÿ���ϴ�.) �� ���� ���谡 ���� ��
	// A �� C �� �ڽ��� �Ǹ� A-->B-->C-->A-->... ó�� ���� ��ȯ�� �߻��մϴ�.
	// �ش� �ڵ�� ������ ���� �����մϴ�.
	// 1. A-->B-->C
	// 2. A   B-->C
	// 3.     B-->C-->A
	// =====================================================================
	Transform* it_prevParent = this;
	Transform* it_parent = m_parent;
	while (it_parent)
	{
		if (it_parent == child)
		{
			it_parent->RemoveChild(it_prevParent);
			break;
		}
		it_prevParent = it_parent;
		it_parent = it_parent->m_parent;
	}
	// child Ʈ���������� �θ�κ��� Ż���ŵ�ϴ�. 
	child->DetachFromParent();
	// �߰��Ƿ��� child Ʈ�������� �θ� �� Ʈ���������� �����մϴ�.
	child->m_parent = this;
	// �� Ʈ�������� �ڽĿ� �߰��Ƿ��� child Ʈ�������� �߰��մϴ�.
	m_childs.push_back(child);
	// �ڽ����� �߰��� child Ʈ�������� ������Ʈ�մϴ�.
	child->UpdateLocalTransform();

	child->UpdateChildWorldTransforms();
	child->UpdateActiveInTree();
}

void Transform::RemoveChild(Transform* child)
{
	if (!child) return;
	// child Ʈ�������� �ڽ����� �������� �ʴ´ٸ� �����մϴ�.
	auto find_it = find(m_childs.begin(), m_childs.end(), child);
	if (find_it == m_childs.end()) return;
	// child Ʈ�������� �ڽ� ���Ϳ��� �����մϴ�.
	m_childs.erase(find_it);
	// child Ʈ�������� �θ� �������� �����մϴ�.
	child->m_parent = nullptr;
	// �θ𿡼� Ż��� child Ʈ�������� ������Ʈ�մϴ�.
	child->UpdateLocalTransform();

	child->UpdateChildWorldTransforms();
	child->UpdateActiveInTree();
}

Transform* Transform::GetRoot() const
{
	const Transform* t = this;

	while (t->m_parent)
	{
		t = t->m_parent;
	}

	return const_cast<Transform*>(t);
}

Transform* Transform::GetParent() const
{
	return m_parent;
}

uint Transform::GetChildCount() const
{
	return uint(m_childs.size());
}

Transform* Transform::GetChild(uint index) const
{
	return m_childs[index];
}

void Transform::LookAt(const V3& targetWorldPosition)
{
	V3 direction = targetWorldPosition - m_worldData.position;
	direction.Normalize();
	V3 currentUp = GetUpDirection();

	rotation = Q::LookRotation(direction, currentUp);
}

void Transform::LookAt(const V3& targetWorldPosition, const V3& up)
{
	V3 direction = targetWorldPosition - m_worldData.position;
	direction.Normalize();
	V3 newUp = up;

	rotation = Q::LookRotation(direction, newUp);
}

void Transform::RotateSelf(const V3& localEulerAngles)
{
	this->localEulerAngles += localEulerAngles;
}

void Transform::RotateWorld(const V3& eulerAngles)
{
	this->eulerAngles += eulerAngles;
}

void Transform::RotateAround(const V3& targetWorldPosition, const V3& axis, float angle)
{
	V3 relative = m_worldData.position - targetWorldPosition;
	float dist = relative.magnitude;
	V3 direction = relative.normalized;

	Q q = Q::AxisAngle(axis, angle);
	direction = q * direction;

	position = targetWorldPosition + direction * dist;
}

void Transform::TranslateSelf(const V3& translation)
{
	position += rotation * translation;
}

void Transform::TranslateWorld(const V3& translation)
{
	position += translation;
}

void Transform::OnDestroyed()
{
	// ===================================================================================================
	// �� Ʈ�������� �ı��Ǵ� ��쿡��
	// ������ ���ӿ�����Ʈ�� �ı��ϸ�(GameObject::Destroy()�� ȣ���� �Ͱ� �����ϴ�)
	// �θ𿡼� Ż��Ǿ�� �ϸ�
	// �ڽĵ� ���� ��� ����� �մϴ�.
	// �̶� �ڽĵ��� ��ȸ�ϸ� �ı� �Լ��� ȣ���� �־�� �ϴµ�
	// �ڽ� Ʈ�������� �θ𿡼� Ż���ϴ� �۾��� �����ϰ� �˴ϴ�.
	// �� �۾��� ��ġ�� ������ �����̳ʸ� ��ȸ�ϸ� ���ÿ� �����̳ʿ��� �����ϴ� ���� ������ �߻��� �� �ֽ��ϴ�.
	// ===================================================================================================
	DetachFromParent();
	// �θ𿡰Լ� Ż���� ��
	// ������ �� ���� �ڽĺ��� �ϳ��� �����մϴ�.
	// �̶� Destroy �Լ� ȣ��� Transform ������Ʈ�� �θ� ���Ϳ��� ���ŵǹǷ�
	// child->Destroy() �Լ��� ȣ���ϴ� �� �����ε� ���Ϳ��� ���ŵ˴ϴ�.
	while (!m_childs.empty())
	{
		auto child = m_childs.back();
		child->Destroy();
	}

	gameObject->Destroy();
}

void Transform::ApplyAcive()
{
	// �� ���ӿ�����Ʈ�� ������Ʈ�鿡�� ���º��� �˸� �Լ��� ȣ���մϴ�.
	uint count = gameObject->GetComponentCount();

	for (uint i = 0; i < count; ++i)
	{
		Component* component = gameObject->GetComponent(i);
		IComponent* iCom = component;
		
		// �ı� ������ ������Ʈ�� �Լ��� �������� �ʽ��ϴ�.
		if (component->IsShouldDestroy())
			return;

		iCom->ApplyEnableChange();
	}
}

void Transform::UpdateActiveInTree()
{
	// ����Ǳ� ���� Ʈ���� Ȱ��ȭ ���¸� ����մϴ�.
	bool beforeActiveInTree = m_activeInTree;

	if (!m_parent)
	{
		// �θ� ���ٸ� Ʈ���� �ֻ���̹Ƿ� ���� Ʈ�������� ������ ���� �ʽ��ϴ�.
		// �� ���� Ʈ���� Ȱ��ȭ ���´� ������ ���Դϴ�.
		m_activeInTree = true;
	}
	else
	{
		// �θ� �ִ� ��쿡��
		// �θ� Ȱ��ȭ �Ǿ��־�� �ϸ�, �θ��� Ʈ���� Ȱ��ȭ ���µ� ���̾�� �մϴ�.
		m_activeInTree = m_parent->m_activeInTree && m_parent->m_activeSelf;
	}

	if (m_activeInTree != beforeActiveInTree)
	{
		// Ȱ��ȭ ���°� ����Ǿ��ٸ� �Ʒ� �Լ��� ȣ���մϴ�.
		ApplyAcive();
	}

	for (auto child : m_childs)
	{
		// �ڽĵ��� Ʈ���� Ȱ��ȭ ���¸� �缳���մϴ�.
		child->UpdateActiveInTree();
	}
}

void Transform::UpdateChildWorldTransforms()
{
	for (auto child : m_childs)
	{
		child->UpdateWorldTransform();
		child->UpdateChildWorldTransforms();
	}
}

void Transform::UpdateLocalTransform()
{
	if (m_parent)
	{
		// �θ� ������ �θ��� ���� ��İ� �� Ʈ�������� ���� ����� ���մϴ�. ��� ��Ŀ��� ��ġ, ȸ��, �������� �����մϴ�.
		M4 worldTransformation = M4::SRT(m_worldData.scale, m_worldData.rotation, m_worldData.position);
		M4 localTransformation = worldTransformation * m_parent->worldToLocalMatrix;	
		m_localData.position = localTransformation.GetTranslation();
		m_localData.rotation = m_worldData.rotation * m_parent->transform->m_worldData.rotation.inversed;
		m_localData.eulerAngles = m_localData.rotation.eulerAngles;
		float sx = ((V3)localTransformation.GetRow(0)).magnitude * Sign(m_worldData.scale.x * m_parent->m_worldData.scale.x);
		float sy = ((V3)localTransformation.GetRow(1)).magnitude * Sign(m_worldData.scale.y * m_parent->m_worldData.scale.y);
		float sz = ((V3)localTransformation.GetRow(2)).magnitude * Sign(m_worldData.scale.z * m_parent->m_worldData.scale.z);
		V3 newScale = V3(sx, sy, sz);
		if (V3::Distance(m_localData.scale, newScale) > SCALE_EPSILON)
			m_localData.scale = newScale;
	}
	else
	{
		m_localData = m_worldData;
	}
	// ���ÿ��� ����� ��ȯ�ϴ� ����� ������Ʈ�մϴ�.
	M4 localTransformation = M4::SRT(m_localData.scale, m_localData.rotation, m_localData.position);
	if (m_parent)
		m_localToWorld = localTransformation * m_parent->m_localToWorld;
	else
		m_localToWorld = localTransformation;
}

void Transform::UpdateWorldTransform()
{
	// ���ÿ��� ����� ��ȯ�ϴ� ����� ������Ʈ�մϴ�.
	M4 localTransformation = M4::SRT(m_localData.scale, m_localData.rotation, m_localData.position);
	if (m_parent)
		m_localToWorld = localTransformation * m_parent->m_localToWorld;
	else
		m_localToWorld = localTransformation;

	if (m_parent)
	{
		// �θ� ������, ���ÿ��� ����� ��ȯ�ϴ� ��Ŀ��� ��ġ, ȸ��, �������� �����մϴ�.
		m_worldData.position = m_localToWorld.GetTranslation();
		m_worldData.rotation = m_localData.rotation * m_parent->m_worldData.rotation;
		m_worldData.eulerAngles = m_worldData.rotation.eulerAngles;
		float sx = ((V3)m_localToWorld.GetRow(0)).magnitude * Sign(m_localData.scale.x * m_parent->m_worldData.scale.x);
		float sy = ((V3)m_localToWorld.GetRow(1)).magnitude * Sign(m_localData.scale.y * m_parent->m_worldData.scale.y);
		float sz = ((V3)m_localToWorld.GetRow(2)).magnitude * Sign(m_localData.scale.z * m_parent->m_worldData.scale.z);
		V3 newScale = V3(sx, sy, sz);
		if (V3::Distance(m_worldData.scale, newScale) > SCALE_EPSILON)
			m_worldData.scale = newScale;
	}
	else
	{
		m_worldData = m_localData;
	}
}
