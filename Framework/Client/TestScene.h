#pragma once

#include <AnimatorBlendNodeElement.h>
#include <AnimatorBlendNode.h>
#include <AnimatorProperty.h>

ENGINE_BEGIN
class VIBuffer;
class MeshRenderer;
ENGINE_END

struct CBUFFER_STRUCT
{
	M4 world;
};

class TestCom;
class TestScene : public Scene
{
	virtual Scene* Clone() override;
	virtual void OnLoad() override;
	virtual void OnUnload() override;
	virtual void OnUpdate() override;
	virtual void OnLateUpdate() override;
	vector<Renderer*> m_renderers;
	Ref<AnimatorProperty> blendProperty0;
	Ref<AnimatorProperty> triggerProperty0;
	Ref<AnimatorProperty> triggerProperty1;
	Ref<AnimatorProperty> triggerProperty2;
	Ref<AnimatorProperty> triggerProperty3;
	Ref<AnimatorBlendNode> blendNode;

	void OnCollisionEnter(const Collision& collision);

	bool lookCamera = false;
};

