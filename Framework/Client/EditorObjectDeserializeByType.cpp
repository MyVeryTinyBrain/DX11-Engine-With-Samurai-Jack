#include "stdafx.h"
#include "EditorObjectDeserializeByType.h"

#include "EditorConfig.h"

#include "Prop.h"

#include "SerializableDirectionalLight.h"
#include "SerializablePointLight.h"
#include "SerializableSpotLight.h"

#include "SerializableBoxCollider.h"
#include "SerializableSphereCollider.h"
#include "SerializableCapsuleCollider.h"

#include "PrefabPlayer.h"
#include "PrefabBeetleDrone.h"
#include "PrefabAncientKing.h"

#include "Lava.h"

EDITOR_USE

IEditorObject* EditorObjectDeserializeByType::DeserializeByType(Scene* scene, const Json::Value& json)
{
    GameObject* go = scene->CreateGameObject();

    if (json["type"] == EDITOR_OBJECT_TYPE_PROP)
        return go->AddComponent<Prop>();

    if (json["type"] == EDITOR_OBJECT_TYPE_DIRECTIONAL_LIGHT)
        return go->AddComponent<SerializableDirectionalLight>();
    if (json["type"] == EDITOR_OBJECT_TYPE_POINT_LIGHT)
        return go->AddComponent<SerializablePointLight>();
    if (json["type"] == EDITOR_OBJECT_TYPE_SPOT_LIGHT)
        return go->AddComponent<SerializableSpotLight>();

    if (json["type"] == EDITOR_OBJECT_TYPE_BOX_COLLIDER)
        return go->AddComponent<SerializableBoxCollider>();
    if (json["type"] == EDITOR_OBJECT_TYPE_SPHERE_COLLIDER)
        return go->AddComponent<SerializableSphereCollider>();
    if (json["type"] == EDITOR_OBJECT_TYPE_CAPSULE_COLLIDER)
        return go->AddComponent<SerializableCapsuleCollider>();

    if (json["type"] == EDITOR_OBJECT_TYPE_PREFAB_PLAYER)
        return go->AddComponent<PrefabPlayer>();
    if (json["type"] == EDITOR_OBJECT_TYPE_PREFAB_BEETLEDRONE)
        return go->AddComponent<PrefabBeetleDrone>();
    if (json["type"] == EDITOR_OBJECT_TYPE_PREFAB_ANCIENTKING)
        return go->AddComponent<PrefabAncientKing>();

    if (json["type"] == EDITOR_OBJECT_TYPE_EDITABLE_LAVA)
        return go->AddComponent<Lava>();

    return nullptr;
}
