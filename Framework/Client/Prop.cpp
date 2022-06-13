#include "stdafx.h"
#include "Prop.h"
#include "Editor.h"
#include "MeshSelector.h"
#include "EditorConfig.h"
#include "EditorEventSystem.h"
#include "Inspector.h"
#include "Config.h"

EDITOR_USE

void Prop::Awake()
{
	IEditorObject::Awake();

	GameObject* goMeshRenderer = CreateGameObjectToChild(transform);
	goMeshRenderer->transform->localEulerAngles = ADJUST_LOCALEULERANGLES;
	m_meshRenderer = goMeshRenderer->AddComponent<MeshRenderer>();
	m_meshRenderer->mesh = system->resource->builtIn->boxMesh;

	Editor* editor = Editor::GetInstance();
	EditorEventSystem* editorEventSystem = nullptr;
	if (editor) editorEventSystem = editor->GetEditorEventSystem();
	if (editorEventSystem) editorEventSystem->Regist(this);
}

void Prop::OnDestroyed()
{
	IEditorObject::OnDestroyed();

	Editor* editor = Editor::GetInstance();
	EditorEventSystem* editorEventSystem = nullptr;
	if (editor) editorEventSystem = editor->GetEditorEventSystem();
	if (editorEventSystem) editorEventSystem->Unregist(this);
}

void Prop::SetMesh(const tstring& path)
{
	m_meshRenderer->mesh = system->resource->Find(path);
	m_meshRenderer->SetupStandardMaterials(true);

	SetMakeShadow(m_makeShadow, true);
}

void Prop::OnImGui()
{
	if (ImGui::TreeNode("Mesh"))
	{
		if (ImGui::Button("select mesh"))
		{
			Editor* editor = Editor::GetInstance();
			MeshSelector* meshSelector = nullptr;
			if (editor) meshSelector = editor->GetMeshSelector();
			if (meshSelector)
			{
				meshSelector->Show();
			}
		}
		if (m_meshRenderer->mesh)
		{
			bool useCollider = m_collider != nullptr;
			if (ImGui::Checkbox("collider", &useCollider))
			{
				SetCollider(useCollider);
			}
			bool makeShadow = m_makeShadow;
			if (ImGui::Checkbox("makeShadow", &makeShadow))
			{
				SetMakeShadow(makeShadow, false);
			}
			string strMeshName = tstring_to_string(m_meshRenderer->mesh->name);
			ImGui::Text(strMeshName.c_str());
		}

		ImGui::TreePop();
	}
}

bool Prop::Raycast(ICamera* camera, V3& out_hit) const
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

string Prop::GetType() const
{
	return EDITOR_OBJECT_TYPE_PROP;
}

void Prop::OnSerialize(Json::Value& json) const
{
	tstring mesh = TEXT("");
	if (m_meshRenderer && m_meshRenderer->mesh)
		mesh = m_meshRenderer->mesh->path;
	json["mesh"] = tstring_to_string(mesh);
	json["collider"] = m_collider != nullptr;
	json["makeShadow"] = m_makeShadow;
}

void Prop::OnDeserialize(const Json::Value& json)
{
	tstring mesh = string_to_tstring(json["mesh"].asString());
	if (mesh == TEXT(""))
	{
		m_meshRenderer->mesh = system->resource->builtIn->boxMesh;
	}
	else
	{
		m_meshRenderer->mesh = system->resource->Find(mesh);
		m_meshRenderer->SetupStandardMaterials(true);
	}

	bool useCollider = json["collider"].asBool();
	SetCollider(useCollider);

	bool makeShadow = json["makeShadow"].asBool();
	SetMakeShadow(makeShadow, true);
}

void Prop::OnDeserializeInRuntime(const Json::Value& json)
{
	OnDeserialize(json);
}

void Prop::OnEvent(const string& msg, void* pContext)
{
	if (msg == EDITOR_EVENT_MESH_SELECTED)
	{
		Ref<IEditorObject> selected = nullptr;
		Editor* editor = Editor::GetInstance();
		Inspector* inspector = nullptr;
		if (editor) inspector = editor->GetInspector();
		if (inspector) selected = inspector->GetSelected();
		else return;

		if (selected.GetPointer() == this)
		{
			TCHAR selectedPath[MAX_PATH]; _tcscpy_s(selectedPath, (TCHAR*)pContext);
			SetMesh(selectedPath);
		}
	}
}

void Prop::SetCollider(bool value)
{
	if (value)
	{
		if (Editor::IsEditorMode())
		{
			m_collider = m_meshRenderer->gameObject->AddComponent<ConvexCollider>();
			m_collider->debugRenderMode = Collider::DebugRenderMode::Wireframe;
		}
		else
		{
			Rigidbody* rigidbody = gameObject->AddComponent<Rigidbody>();
			rigidbody->kinematic = true;
			m_collider = m_meshRenderer->gameObject->AddComponent<ConvexCollider>();
		}
	}
	else
	{
		if (m_collider)
		{
			m_collider->Destroy();
			m_collider = nullptr;
		}
	}
}

void Prop::SetMakeShadow(bool value, bool nocondition)
{
	if (!nocondition && value == m_makeShadow)
		return;

	m_makeShadow = value;

	if (m_makeShadow) m_meshRenderer->SetupStandardMaterials(true);
	else m_meshRenderer->SetupStandardNoShadowMaterials(true);
}
