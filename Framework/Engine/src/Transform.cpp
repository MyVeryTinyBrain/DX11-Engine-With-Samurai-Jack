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

	// 개별 활성화 상태가 변경되었으므로 아래 함수를 호출합니다.
	ApplyAcive();
}

void Transform::SetParent(Transform* parent)
{	
	// 이 트랜스폼을 부모로 두려는 경우 종료합니다.
	if (parent == this) 
		return;

	// 우선, 부모에서 탈퇴합니다.
	DetachFromParent();

	if (parent)
	{
		// 등록되려는 부모가 있다면
		// 새로운 부모의 자식으로 등록합니다.
		parent->AddChild(this);
	}
}

void Transform::DetachFromParent()
{
	if (!m_parent)
		return;

	// 부모가 있다면 부모에게서 탈퇴합니다.
	m_parent->RemoveChild(this);
}

void Transform::AddChild(Transform* child)
{
	// 이 트랜스폼을 자식으로 두려는 경우 종료합니다.
	if (child == this) return;
	if (!child) return;
	// =====================================================================
	// 무한 순환이 발생할 수 있는 경우 부모에서 탈퇴합니다.
	// A-->B-->C (화살표가 가리키는 대상은 자식을 나타냅니다.) 와 같은 관계가 있을 때
	// A 가 C 의 자식이 되면 A-->B-->C-->A-->... 처럼 무한 순환이 발생합니다.
	// 해당 코드는 다음과 같이 동작합니다.
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
	// child 트랜스폼을을 부모로부터 탈퇴시킵니다. 
	child->DetachFromParent();
	// 추가되려는 child 트랜스폼의 부모를 이 트랜스폼으로 설정합니다.
	child->m_parent = this;
	// 이 트랜스폼의 자식에 추가되려는 child 트랜스폼을 추가합니다.
	m_childs.push_back(child);
	// 자식으로 추가된 child 트랜스폼을 업데이트합니다.
	child->UpdateLocalTransform();

	child->UpdateChildWorldTransforms();
	child->UpdateActiveInTree();
}

void Transform::RemoveChild(Transform* child)
{
	if (!child) return;
	// child 트랜스폼이 자식으로 존재하지 않는다면 종료합니다.
	auto find_it = find(m_childs.begin(), m_childs.end(), child);
	if (find_it == m_childs.end()) return;
	// child 트랜스폼을 자식 벡터에서 제거합니다.
	m_childs.erase(find_it);
	// child 트랜스폼의 부모를 없음으로 설정합니다.
	child->m_parent = nullptr;
	// 부모에서 탈퇴된 child 트랜스폼을 업데이트합니다.
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
	// 이 트랜스폼이 파괴되는 경우에는
	// 부착된 게임오브젝트를 파괴하며(GameObject::Destroy()를 호출한 것과 같습니다)
	// 부모에서 탈퇴되어야 하며
	// 자식들 역시 모두 떼어내야 합니다.
	// 이때 자식들을 순회하며 파괴 함수를 호출해 주어야 하는데
	// 자식 트랜스폼도 부모에서 탈퇴하는 작업을 진행하게 됩니다.
	// 이 작업이 겹치기 때문에 컨테이너를 순회하며 동시에 컨테이너에서 제거하는 것은 문제가 발생할 수 있습니다.
	// ===================================================================================================
	DetachFromParent();
	// 부모에게서 탈퇴한 후
	// 벡터의 맨 뒤쪽 자식부터 하나씩 제거합니다.
	// 이때 Destroy 함수 호출시 Transform 컴포넌트는 부모 벡터에서 제거되므로
	// child->Destroy() 함수를 호출하는 것 만으로도 벡터에서 제거됩니다.
	while (!m_childs.empty())
	{
		auto child = m_childs.back();
		child->Destroy();
	}

	gameObject->Destroy();
}

void Transform::ApplyAcive()
{
	// 이 게임오브젝트의 컴포넌트들에게 상태변경 알림 함수를 호출합니다.
	uint count = gameObject->GetComponentCount();

	for (uint i = 0; i < count; ++i)
	{
		Component* component = gameObject->GetComponent(i);
		IComponent* iCom = component;
		
		// 파괴 예정인 컴포넌트의 함수는 실행하지 않습니다.
		if (component->IsShouldDestroy())
			return;

		iCom->ApplyEnableChange();
	}
}

void Transform::UpdateActiveInTree()
{
	// 변경되기 전의 트리내 활성화 상태를 기록합니다.
	bool beforeActiveInTree = m_activeInTree;

	if (!m_parent)
	{
		// 부모가 없다면 트리의 최상단이므로 상위 트랜스폼에 영향을 받지 않습니다.
		// 이 때의 트리내 활성화 상태는 무조건 참입니다.
		m_activeInTree = true;
	}
	else
	{
		// 부모가 있는 경우에는
		// 부모가 활성화 되어있어야 하며, 부모의 트리내 활성화 상태도 참이어야 합니다.
		m_activeInTree = m_parent->m_activeInTree && m_parent->m_activeSelf;
	}

	if (m_activeInTree != beforeActiveInTree)
	{
		// 활성화 상태가 변경되었다면 아래 함수를 호출합니다.
		ApplyAcive();
	}

	for (auto child : m_childs)
	{
		// 자식들의 트리내 활성화 상태를 재설정합니다.
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
		// 부모가 있으면 부모의 월드 행렬과 이 트랜스폼의 로컬 행렬을 곱합니다. 결과 행렬에서 위치, 회전, 스케일을 추출합니다.
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
	// 로컬에서 월드로 변환하는 행렬을 업데이트합니다.
	M4 localTransformation = M4::SRT(m_localData.scale, m_localData.rotation, m_localData.position);
	if (m_parent)
		m_localToWorld = localTransformation * m_parent->m_localToWorld;
	else
		m_localToWorld = localTransformation;
}

void Transform::UpdateWorldTransform()
{
	// 로컬에서 월드로 변환하는 행렬을 업데이트합니다.
	M4 localTransformation = M4::SRT(m_localData.scale, m_localData.rotation, m_localData.position);
	if (m_parent)
		m_localToWorld = localTransformation * m_parent->m_localToWorld;
	else
		m_localToWorld = localTransformation;

	if (m_parent)
	{
		// 부모가 있으면, 로컬에서 월드로 변환하는 행렬에서 위치, 회전, 스케일을 추출합니다.
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
