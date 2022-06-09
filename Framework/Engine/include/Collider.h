#pragma once

#include "Component.h"
#include "ICollider.h"
#include "PhysicsSimulationCallbackType.h"
#include "ResourceRef.h"

ENGINE_BEGIN
class Rigidbody;
class Mesh;
class ENGINE_API Collider abstract : public Component, public ICollider
{
public:

	enum class CombineMode
	{
		Average = PxCombineMode::eAVERAGE,
		Min = PxCombineMode::eMIN,
		Max = PxCombineMode::eMAX,
		Multiply = PxCombineMode::eMULTIPLY,
	};

	enum class DebugRenderMode
	{
		None,
		Wireframe,
		Fill,
	};

protected:

	virtual void Awake() override;
	virtual void Awake(void* arg /* PxShape */) override;
	virtual void BeforePhysicsSimulation() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroyed() override;

	virtual void Render() final override { if (m_debugRenderMode != Collider::DebugRenderMode::None) DebugRender(); }
	virtual void DebugRender() = 0;

protected:

	virtual ~Collider();

public:	

	inline bool IsTrigger() const { return m_isTrigger; }
	void SetTrigger(bool value);

	inline float GetFriction() const { return m_material->getDynamicFriction(); }
	void SetFriction(float value);

	inline float GetRestitution() const { return m_material->getRestitution(); }
	inline void SetRestitution(float value) { m_material->setRestitution(value); }

	inline Collider::CombineMode GetFrictionCombineMode() const { return (Collider::CombineMode)m_material->getFrictionCombineMode(); }
	inline void SetFrictionCombineMode(Collider::CombineMode value) { m_material->setFrictionCombineMode((PxCombineMode::Enum)value); }

	inline Collider::CombineMode GetRestituitionCombineMode() const { return (Collider::CombineMode)m_material->getRestitutionCombineMode(); }
	inline void SetRestituitionCombineMode(Collider::CombineMode value) { m_material->setRestitutionCombineMode((PxCombineMode::Enum)value); }

	_declspec(property(get = IsTrigger, put = SetTrigger)) bool isTrigger;
	_declspec(property(get = GetFriction, put = SetFriction)) float friction;
	_declspec(property(get = GetRestitution, put = SetRestitution)) float restitution;
	_declspec(property(get = GetFrictionCombineMode, put = SetFrictionCombineMode)) Collider::CombineMode frictionCombineMode;
	_declspec(property(get = GetRestituitionCombineMode, put = SetRestituitionCombineMode)) Collider::CombineMode restituionCombineMode;

public:

	inline uint32 GetIgnoreLayerBits() const { return m_ignoreLayerBits; }
	void SetIngoreLayerBits(uint32 value);

	bool IsIgnoreLayerIndex(uint8 layerIndex) const;
	void SetIgnoreLayerIndex(uint layerIndex, bool ignore);

	inline uint8 GetLayerIndex() const { return m_layerIndex; }
	void SetLayerIndex(uint8 layerIndex);

	inline bool IsValid() const { return m_valid; }

	inline Collider::DebugRenderMode GetDebugRenderMode() const { return m_debugRenderMode; }
	void SetDebugRenderMode(Collider::DebugRenderMode value);

	_declspec(property(get = GetIgnoreLayerBits, put = SetIngoreLayerBits)) uint32 ignoreLayerBits;
	_declspec(property(get = GetLayerIndex, put = SetLayerIndex)) uint8 layerIndex;
	_declspec(property(get = IsValid)) bool isValid;
	_declspec(property(get = GetDebugRenderMode, put = SetDebugRenderMode)) Collider::DebugRenderMode debugRenderMode;

public:

	Rigidbody* GetRigidbody() const;
	virtual ResourceRef<Mesh> GetDebugMesh() const = 0;
	virtual M4 GetDebugRenderWorldMatrix() const = 0;

	_declspec(property(get = GetRigidbody)) Rigidbody* rigidbody;

public:

	virtual Bounds GetBounds() const = 0;

	PxGeometryHolder GetPxGeometry() const { return m_shape->getGeometry(); }

	_declspec(property(get = GetPxGeometry)) PxGeometryHolder pxGeometry;

public:

	inline virtual bool IsCCTComponent() const override { return m_isCCTComponent; }

protected:

	virtual PxGeometryHolder CreatePxGeometry(bool& out_invalid) const = 0;
	virtual void ResetShape();

private:

	void FindRigidbodyAndAttach();
	void ApplyFlags();
	void ApplyPose(bool unconditionally);
	void ApplyScale(bool unconditionally);
	void ApplyLayer();
		
private:

	virtual bool IsWorking() const override;

private:

	inline virtual PxShape* GetPxShape() const override { return m_shape; }

private:

	PxMaterial*		m_material = nullptr;
	PxShape*		m_shape = nullptr;

	bool			m_isTrigger = false;
	uint32			m_ignoreLayerBits = 0;
	uint8			m_layerIndex = 0;

	V3				m_beforeLocalPosition;
	Q				m_beforeLocalRotation;
	V3				m_beforeWorldScale;

	bool			m_valid = false;

	DebugRenderMode	m_debugRenderMode = DebugRenderMode::None;

	bool			m_isCCTComponent = false;

protected:

	Q				m_defaultRotation = Q::identity();

public:

	delegate<void(const Collision&)> OnCollisionEnter;
	delegate<void(const Collision&)> OnCollisionStay;	// Rigidbody가 Sleep 상태이면 동작하지 않습니다.
	delegate<void(const Collision&)> OnCollisionExit;
};

ENGINE_END
