#include "stdafx.h"
#include "BeetleSpawner.h"
#include "EditorConfig.h"
#include "Config.h"
#include "Editor.h"
#include "EventSystem.h"
#include "EnemyBeetleDrone.h"

void BeetleSpawner::Awake()
{
	IEditorObject::Awake();

	EventSystem::RegistListener(this);
	
	if (EDITOR::Editor::IsEditorMode())
	{
		m_meshRenderer = gameObject->AddComponent<MeshRenderer>();
		m_meshRenderer->mesh = system->resource->builtIn->sphereMesh;
		m_meshRenderer->material = system->resource->builtIn->whiteColorMaterial;
	}

	GameObject* goArea = CreateGameObjectToChild(transform);
	m_area = goArea->AddComponent<BoxCollider>();
	m_area->isTrigger = true;

	if (EDITOR::Editor::IsEditorMode())
	{
		m_area->debugRenderMode = Collider::DebugRenderMode::Wireframe;
	}
}

void BeetleSpawner::Start()
{
	IEditorObject::Start();
}

void BeetleSpawner::Update()
{
	IEditorObject::Update();

	if (EDITOR::Editor::IsEditorMode())
		return;

	if (!m_spawn)
	{
		if (system->physics->query->OverlapTest(m_area, 1 << PhysicsLayer_Player, PhysicsQueryType::Collider))
		{
			m_spawn = true;
			m_numLeft = m_numSpawn;
			EventSystem::Notify(EVENT_FIGHT_START, nullptr);
		}
	}
	else if(m_numLeft > 0)
	{
		m_spawnLeftTime -= system->time->deltaTime;

		if (m_spawnLeftTime < 0.0f)
		{
			m_spawnLeftTime = m_spawnDuration;

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> rdX(-m_area->extents.x, m_area->extents.x);
			std::uniform_real_distribution<float> rdY(-m_area->extents.y, m_area->extents.y);
			std::uniform_real_distribution<float> rdZ(-m_area->extents.z, m_area->extents.z);
			float rx = rdX(gen);
			float ry = rdY(gen);
			float rz = rdZ(gen);
			V3 position = V3(rx, ry, rz) + m_area->transform->position;

			GameObject* goBeetle = CreateGameObject();
			goBeetle->transform->position = position;
			EnemyBeetleDrone* enemy = goBeetle->AddComponent<EnemyBeetleDrone>();
			enemy->Appear();
			m_enemies.push_back(enemy);

			--m_numLeft;
		}
	}
}

void BeetleSpawner::OnDestroyed()
{
	IEditorObject::OnDestroyed();

	EventSystem::UnregistListener(this);
}

void BeetleSpawner::OnImGui()
{
	if (ImGui::TreeNode("BeetleSpawner"))
	{
		ImGui::DragInt("numSpawn", &m_numSpawn, 0, 20);
		ImGui::DragFloat("spawnDuration", &m_spawnDuration, 0.0f, 10.0f);

		// area local position
		V3 areaLocalPosition = m_area->transform->localPosition;
		ImGui::DragFloat3("areaLocalPosition", (float*)&areaLocalPosition, 0.1f);
		m_area->transform->localPosition = areaLocalPosition;

		// extents
		V3 extents = m_area->extents;
		ImGui::DragFloat3("extents", (float*)&extents, 0.1f);
		m_area->extents = extents;

		ImGui::TreePop();
	}
}

bool BeetleSpawner::Raycast(ICamera* camera, V3& out_hit) const
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

string BeetleSpawner::GetType() const
{
	return EDITOR_OBJECT_TYPE_BEETLE_SPAWNER;
}

void BeetleSpawner::OnSerialize(Json::Value& json) const
{
	json["numSpawn"] = m_numSpawn;
	json["spawnDuration"] = m_spawnDuration;
	json["areaLocalPosition"] = JsonUtility::Parse(m_area->transform->localPosition);
	json["extents"] = JsonUtility::Parse(m_area->extents);
}

void BeetleSpawner::OnDeserialize(const Json::Value& json)
{
	m_numSpawn = json["numSpawn"].asInt();
	m_spawnDuration = json["spawnDuration"].asFloat();

	V4 areaLocalPosition;
	JsonUtility::ToVector4(json["areaLocalPosition"].asString(), areaLocalPosition);
	m_area->transform->localPosition = areaLocalPosition;

	V4 extents;
	JsonUtility::ToVector4(json["extents"].asString(), extents);
	m_area->extents = extents;
}

void BeetleSpawner::OnDeserializeInRuntime(const Json::Value& json)
{
	OnDeserialize(json);
}

void BeetleSpawner::OnEvent(const string& msg, void* pContext)
{
	if (msg == EVENT_ENEMY_DIE)
	{
		Enemy* enemy = (Enemy*)pContext;
		
		bool finded = false;
		for (auto& spawned : m_enemies)
		{
			if (enemy == spawned)
			{
				finded = true;
				break;
			}
		}

		if (finded)
		{
			++m_numDead;

			if (m_numDead == m_numSpawn)
			{
				EventSystem::Notify(EVENT_FIGHT_END, nullptr);
				gameObject->Destroy();
			}
		}
	}
}
