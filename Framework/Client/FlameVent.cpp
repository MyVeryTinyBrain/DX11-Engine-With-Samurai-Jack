#include "stdafx.h"
#include "FlameVent.h"
#include "Config.h"
#include "ParticleFlame.h"
#include "EditorConfig.h"
#include "Editor.h"
#include "Player.h"

void FlameVent::Awake()
{
	IEditorObject::Awake();

	GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
	goMeshRenderer->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
	m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
	m_meshRenderer->mesh = system->resource->Find(MESH_ENV_CAVE_VENT);
	m_meshRenderer->SetupStandardMaterials();

	GameObject* goArea = CreateGameObjectToChild(transform);
	m_area = goArea->AddComponent<BoxCollider>();
	m_area->isTrigger = true;
	if (EDITOR::Editor::IsEditorMode())
	{
		m_area->debugRenderMode = Collider::DebugRenderMode::Wireframe;
	}

	m_area->extents = V3(0.5f, m_flameLength * 0.5f, 0.5f);
	m_area->transform->transform->localPosition = V3(0.0f, m_flameLength * 0.5f, 0.0f);

	if (!EDITOR::Editor::IsEditorMode())
	{
		GameObject* goLight = CreateGameObjectToChild(transform);
		goLight->transform->localPosition = V3::up();
		m_light = goLight->AddComponent<PointLight>();
		m_light->diffuse = Color::RGBA255(240, 80, 20, 255);
		m_light->range = 10.0f;
		m_light->enable = false;

		m_audioSource = gameObject->AddComponent<AudioSource>();
		m_audioSource->minDistance = 3.0f;
		m_audioSource->maxDistance = 30.0f;
	}
}

void FlameVent::Start()
{
	IEditorObject::Start();
}

void FlameVent::Update()
{
	IEditorObject::Update();

	if (Abs(m_flameLength - m_area->extents.y * 2.0f) > Epsilon)
	{
		m_area->extents = V3(0.5f, m_flameLength * 0.5f, 0.5f);
		m_area->transform->transform->localPosition = V3(0.0f, m_flameLength * 0.5f, 0.0f);
	}

	if (EDITOR::Editor::IsEditorMode())
	{
		return;
	}

	if (!m_throw)
	{
		if (m_waitTime > 0.0f)
		{
			m_waitTime -= system->time->deltaTime;
			return;
		}
		else
		{
			m_throwTime = m_throwDuration;
			m_throw = true;
			m_light->enable = true;
			m_audioSource->PlayOneshot(system->resource->Find(SOUND_FLAME_01));
		}
	}
	else
	{
		if (m_throwTime > 0.0f)
		{
			m_throwTime -= system->time->deltaTime;

			int r = rand() % 10;

			if (r > 0)
				return;

			float speed = m_flameLength / m_flameDuration;

			ParticleFlame::Create(
				gameObject->regionScene,
				transform->position + transform->up,
				m_flameDuration,
				transform->up * speed, 0.0f,
				0.0f, 0.1f, 5.0f, 0.0f, 0.9f,
				Color::white(), Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f
			);

			if (system->physics->query->OverlapTest(m_area, 1 << PhysicsLayer_Player)) {
				Player::GetInstance()->hp -= 100.0f * system->time->deltaTime;
			}
		}
		else
		{
			m_waitTime = m_delay;
			m_throw = false;
			m_light->enable = false;
		}
	}
}

void FlameVent::OnImGui()
{
	if (ImGui::TreeNode("FlameVent"))
	{
		ImGui::DragFloat("flameDuration", &m_flameDuration, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("flameLength", &m_flameLength, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("delay", &m_delay, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("throwDuration", &m_throwDuration, 0.1f, 0.0f, 100.0f);

		ImGui::TreePop();
	}
}

bool FlameVent::Raycast(ICamera* camera, V3& out_hit) const
{
	if (!m_meshRenderer->mesh)
		return false;

	M4 localToWorldMatrix = m_meshRenderer->transform->localToWorldMatrix;
	Ray ray = system->input->GetRayInWorldSpace(camera);
	if (m_meshRenderer->mesh->RaycastInWorldSpace(out_hit, ray, localToWorldMatrix, true))
	{
		return true;
	}
	else
	{
		return false;
	}
}

string FlameVent::GetType() const
{
	return EDITOR_OBJECT_TYPE_FLAME_VENT;
}

void FlameVent::OnSerialize(Json::Value& json) const
{
	json["flameDuration"] = m_flameDuration;
	json["flameLength"] = m_flameLength;
	json["delay"] = m_delay;
	json["throwDuration"] = m_throwDuration;
}

void FlameVent::OnDeserialize(const Json::Value& json)
{
	m_flameDuration = json["flameDuration"].asFloat();
	m_flameLength = json["flameLength"].asFloat();
	m_delay = json["delay"].asFloat();
	m_throwDuration = json["throwDuration"].asFloat();
}

void FlameVent::OnDeserializeInRuntime(const Json::Value& json)
{
	OnDeserialize(json);
}
