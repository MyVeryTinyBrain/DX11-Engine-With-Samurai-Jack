#include "stdafx.h"
#include "TestScene.h"
#include <GraphicSystem.h>
#include <CompiledShaderDesc.h>
#include <VIBuffer.h>
#include <VITypes.h>
#include <FPSChecker.h>
#include <TechniqueDesc.h>
#include <PassDesc.h>
#include <VI.h>
#include <ResourceFactory.h>
#include <Mesh.h>
#include <RenderQueue.h>
#include <CBufferManager.h>
#include <PrimitiveVI.h>
#include <MeshRenderer.h>
#include <AssimpData.h>
#include <BuiltInResources.h>
#include <Material.h>
#include <SubResourceRef.h>
#include <SubResourceObject.h>
#include <NodeSet.h>
#include <AnimationSet.h>
#include <AnimatorSingleNode.h>
#include <AnimatorBlendNodeElement.h>
#include <AnimatorBlendNode.h>
#include "FreeCamera.h"

Scene* TestScene::Clone()
{
    return new TestScene;
}

void TestScene::OnLoad()
{
    // 스레드 내부에서 호출하려는 경우에는 미리 한 번 호출해줘야 합니다.
    system->resource->factory->CreateUnmanagedTexture2DFromFile(TEXT("../Resource/TEMP/TEMP.png"));
    thread t0(
        [&]
        {
            system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Dev/Dev.png"));
            system->resource->factory->CreateManagedMeshFromFile(TEXT("../Resource/Character/Jack/Jack.FBX"));
            system->resource->factory->CreateManagedMeshFromFile(TEXT("../Resource/Weapon/Katana/Katana.FBX"));
            system->resource->factory->CreateManagedMeshFromFile(TEXT("../Resource/Weapon/Katana/KatanaSheath.FBX"));
        });
    if (t0.joinable())
        t0.join();

    {
        GameObject* goCamera = CreateGameObject(TEXT("MainCamera"), TEXT("Camera"));
        goCamera->AddComponent<FreeCamera>();
        goCamera->transform->position = V3(0, 0, -5);
    }

    //{
    //    GameObject* goJack = CreateGameObject(TEXT("TestJack1"));
    //    goJack->transform->position = V3(3, 0, 0);
    //    goJack->transform->localEulerAngles = V3(90, 0, 0);
    //    MeshRenderer* meshRenderer = goJack->AddComponent<MeshRenderer>();
    //    meshRenderer->mesh = system->resourceManagement->Find(TEXT("../Resource/Character/Jack/Jack.FBX"));
    //}

    //{
    //    GameObject* goJack = CreateGameObject(TEXT("TestJack2"));
    //    goJack->transform->position = V3(-3, 0, 0);
    //    goJack->transform->localEulerAngles = V3(90, 0, 0);
    //    SkinnedMeshRenderer* skinnedMeshRenderer = goJack->AddComponent<SkinnedMeshRenderer>();
    //    skinnedMeshRenderer->mesh = system->resourceManagement->Find(TEXT("../Resource/Character/Jack/Jack.FBX"));
    //}

    {
        GameObject* goJack = CreateGameObject(TEXT("Jack"));
        goJack->transform->position = V3(0, 0, 0);
        goJack->transform->localEulerAngles = V3(90, 0, 0);
        SkinnedMeshRenderer* skinnedMeshRenderer = goJack->AddComponent<SkinnedMeshRenderer>();
        skinnedMeshRenderer->mesh = system->resource->Find(TEXT("../Resource/Character/Jack/Jack.FBX"));

        const vector<ModelMaterialDesc>& materials = skinnedMeshRenderer->mesh->materialDescs;
        const vector<uint>& materialIndices = skinnedMeshRenderer->mesh->materialIndices;
        skinnedMeshRenderer->SetMaterialCount(skinnedMeshRenderer->mesh->subMeshCount);

        for (uint i = 0; i < skinnedMeshRenderer->mesh->subMeshCount; ++i)
        {
            tstring texturePath;
            ResourceRef<Texture2D> texture;

            if (i < materialIndices.size())
            {
                texturePath = materials[materialIndices[i]].diffuse;
                texture = system->resource->Find(texturePath);
            }

            ResourceRef<Material> material = system->resource->factory->CreateUnmanagedMaterialByShader(system->resource->builtIn->standardShader);
            material->SetTexture("_DiffuseTexture", texture);

            skinnedMeshRenderer->SetMaterialByIndex(i, material);
        }

        GameObject* goGizmo = CreateGameObject();
        Gizmo* gizmo = goGizmo->AddComponent<Gizmo>();
        gizmo->handlingTransform = goJack->transform;
        gizmo->show = true;

        Animator* animator = goJack->AddComponent<Animator>();
        AnimatorLayer* layer = new AnimatorLayer;

        blendProperty0 = new AnimatorProperty(TEXT("blendProperty0"), AnimatorProperty::Type::FLOAT);
        triggerProperty0 = new AnimatorProperty(TEXT("triggerProperty0"), AnimatorProperty::Type::TRIGGER);
        triggerProperty1 = new AnimatorProperty(TEXT("triggerProperty1"), AnimatorProperty::Type::TRIGGER);
        triggerProperty2 = new AnimatorProperty(TEXT("triggerProperty2"), AnimatorProperty::Type::TRIGGER);
        triggerProperty3 = new AnimatorProperty(TEXT("triggerProperty3"), AnimatorProperty::Type::TRIGGER);
        layer->AddProperty(blendProperty0);
        layer->AddProperty(triggerProperty0);
        layer->AddProperty(triggerProperty1);
        layer->AddProperty(triggerProperty2);
        layer->AddProperty(triggerProperty3);

        vector<Ref<AnimatorBlendNodeElement>> elements;
        AnimatorSingleNode* node0 = AnimatorSingleNode::Create(skinnedMeshRenderer->mesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_IDL_01_Lp")), true);
        //AnimatorSingleNode* nodeWalk1 = new AnimatorSingleNode(skinnedMeshRenderer->mesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_WLK_Lp_Root")), true);
        AnimatorSingleNode* nodeRun1 = AnimatorSingleNode::Create(skinnedMeshRenderer->mesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_RUN_Lp_Root")), true);
        AnimatorBlendNodeElement* element0 = AnimatorBlendNodeElement::Create(node0, 0);
        //AnimatorBlendNodeElement* element1 = new AnimatorBlendNodeElement(nodeWalk1, 1);
        AnimatorBlendNodeElement* element2 = AnimatorBlendNodeElement::Create(nodeRun1, 1);
        elements.push_back(element0);
        //elements.push_back(element1);
        elements.push_back(element2);
        blendNode = AnimatorBlendNode::Create(TEXT("BlendNode"), elements, blendProperty0, true);
        layer->AddNode(blendNode);

        AnimatorSingleNode* nodeAttack0 = AnimatorSingleNode::Create(skinnedMeshRenderer->mesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_ATK_01Y")), false);
        AnimatorSingleNode* nodeAttack1 = AnimatorSingleNode::Create(skinnedMeshRenderer->mesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_ATK_01YY")), false);
        AnimatorSingleNode* nodeWalkSt = AnimatorSingleNode::Create(skinnedMeshRenderer->mesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_ATK_Kick")), false);
        AnimatorSingleNode* nodeRunSt = AnimatorSingleNode::Create(skinnedMeshRenderer->mesh->GetAnimationClipByName(TEXT("SK_CHR_Jack_new.ao|Jack_KT_ATK_Jump_Slashing")), false);
        layer->AddNode(nodeAttack0);
        layer->AddNode(nodeAttack1);
        layer->AddNode(nodeWalkSt);
        layer->AddNode(nodeRunSt);

        vector<AnimatorTransition::PropertyValue> propertyValues0;
        vector<AnimatorTransition::PropertyValue> propertyValues1;
        vector<AnimatorTransition::PropertyValue> propertyValues2;
        vector<AnimatorTransition::PropertyValue> propertyValues3;
        vector<AnimatorTransition::PropertyValue> propertyValuesEmpty;

        propertyValues0.push_back(AnimatorTransition::PropertyValue::Trigger(triggerProperty0));
        propertyValues1.push_back(AnimatorTransition::PropertyValue::Trigger(triggerProperty1));
        propertyValues2.push_back(AnimatorTransition::PropertyValue::Trigger(triggerProperty2));
        propertyValues3.push_back(AnimatorTransition::PropertyValue::Trigger(triggerProperty3));
        AnimatorTransition* transition0 = AnimatorTransition::Create(nullptr, nodeAttack0, propertyValues0, 0.0f, 0.1f);
        AnimatorTransition* transition1 = AnimatorTransition::Create(nullptr, nodeAttack1, propertyValues1, 0.0f, 0.1f, 0.1f);
        AnimatorTransition* transition2 = AnimatorTransition::Create(nullptr, nodeWalkSt, propertyValues2, 0.0f, 0.1f);
        AnimatorTransition* transition3 = AnimatorTransition::Create(nullptr, nodeRunSt, propertyValues3, 0.0f, 0.1f);
        AnimatorTransition* transition4 = AnimatorTransition::Create(nodeAttack0, nullptr, propertyValuesEmpty, 0.8f, 0.2f);
        AnimatorTransition* transition5 = AnimatorTransition::Create(nodeAttack1, nullptr, propertyValuesEmpty, 0.8f, 0.2f);
        AnimatorTransition* transition6 = AnimatorTransition::Create(nodeWalkSt, nullptr, propertyValuesEmpty, 0.8f, 0.2f);
        AnimatorTransition* transition7 = AnimatorTransition::Create(nodeRunSt, nullptr, propertyValuesEmpty, 0.8f, 0.2f);
        layer->AddTransition(transition0);
        layer->AddTransition(transition1);
        layer->AddTransition(transition2);
        layer->AddTransition(transition3);
        layer->AddTransition(transition4);
        layer->AddTransition(transition5);
        layer->AddTransition(transition6);
        layer->AddTransition(transition7);

        animator->AddLayer(layer);

        layer->SetRootNodeByName(TEXT("CharacterRoot"));
    }

    {
        GameObject* goKatana = CreateGameObject(TEXT("Katana"));
        goKatana->transform->position = V3(0, 0, 0);
        MeshRenderer* meshRenderer = goKatana->AddComponent<MeshRenderer>();
        meshRenderer->name = TEXT("KatanaRenderer");
        meshRenderer->mesh = system->resource->Find(TEXT("../Resource/Weapon/Katana/Katana.FBX"));

        GameObject* goGizmo = CreateGameObject();
        Gizmo* gizmo = goGizmo->AddComponent<Gizmo>();
        gizmo->handlingTransform = goKatana->transform;
        gizmo->show = true;
    }

    {
        GameObject* goKatanaScabbard = CreateGameObject(TEXT("KatanaSheath"));
        goKatanaScabbard->transform->position = V3(0, 0, 0);
        MeshRenderer* meshRenderer = goKatanaScabbard->AddComponent<MeshRenderer>();
        meshRenderer->mesh = system->resource->Find(TEXT("../Resource/Weapon/Katana/KatanaSheath.FBX"));

        GameObject* goGizmo = CreateGameObject();
        Gizmo* gizmo = goGizmo->AddComponent<Gizmo>();
        gizmo->handlingTransform = goKatanaScabbard->transform;
        gizmo->show = true;
    }

    {
        GameObject* goGround = CreateGameObject();

        goGround->transform->position = V3::down() * 0.5f;
        goGround->transform->localScale = V3(30, 1, 30);

        Rigidbody* rigidbody = goGround->AddComponent<Rigidbody>();
        rigidbody->kinematic = true;
        BoxCollider* boxCollider = goGround->AddComponent<BoxCollider>();
        boxCollider->restitution = 1.0f;
    }

    {
        GameObject* goHandlingTest = CreateGameObject();
        goHandlingTest->transform->position = V3(3, 3, 0);

        MeshRenderer* meshRenderer = goHandlingTest->AddComponent<MeshRenderer>();
        meshRenderer->mesh = system->resource->builtIn->boxMesh;
        meshRenderer->material = system->resource->builtIn->wireframeMaterial;

        GameObject* goGizmo = CreateGameObject();
        Gizmo* gizmo = goGizmo->AddComponent<Gizmo>();
        gizmo->handlingTransform = goHandlingTest->transform;
        gizmo->show = true;
    }

    //{
    //    ResourceRef<MaterialTestInst> mat = system->resourceManagement->factory->CreateManagedMaterial<MaterialTestInst>(TEXT("MaterialTestInst"));
    //    mat->diffuseTexture = system->resourceManagement->builtIn->whiteTexture;

    //    for (uint i = 0; i < 10000; ++i)
    //    {
    //        GameObject* goCapsule = CreateGameObject();
    //        float rx = float(rand() % 200 - 100);
    //        float ry = float(rand() % 200 - 100);
    //        float rz = float(rand() % 200 - 100);
    //        goCapsule->transform->position = V3(rx, ry, rz);

    //        MeshRenderer* meshRenderer = goCapsule->AddComponent<MeshRenderer>();
    //        meshRenderer->material = mat;
    //        meshRenderer->mesh = system->resourceManagement->builtIn->sphereMesh;
    //    }
    //}

    int breakpoint = 0;
}

void TestScene::OnUnload()
{
}

void TestScene::OnUpdate()
{
    GameObject* goCamera = FindGameObjectWithTag(TEXT("Camera"));

    if (system->input->GetKeyDown(Key::Space))
    {
        uint r = rand() % 3;

        GameObject* go = CreateGameObject();
        Rigidbody* rigidbody = go->AddComponent<Rigidbody>();
        go->transform->position = goCamera->transform->position;
        rigidbody->velocity = system->input->GetRayInWorldSpace().direction * 10.0f;
        rigidbody->linearDamping = 0.5f;
        rigidbody->angularDamping = 0.5f;
        rigidbody->SetInterpolateMode(Rigidbody::Interpolate::Interpolate);

		switch (r)
		{
			case 0:
			{
				BoxCollider* boxCollider = go->AddComponent<BoxCollider>();
				boxCollider->restitution = 0.1f;

				boxCollider->OnCollisionEnter += func<void(const Collision&)>(this, &TestScene::OnCollisionEnter);
			}
			break;
			case 1:
			{
				CapsuleCollider* capsuleCollider = go->AddComponent<CapsuleCollider>();
				capsuleCollider->restitution = 0.1f;

				capsuleCollider->OnCollisionEnter += func<void(const Collision&)>(this, &TestScene::OnCollisionEnter);
			}
			break;
			case 2:
			{
				SphereCollider* sphereCollider = go->AddComponent<SphereCollider>();
				sphereCollider->restitution = 0.1f;

				sphereCollider->OnCollisionEnter += func<void(const Collision&)>(this, &TestScene::OnCollisionEnter);
			}
			break;
		}
    }

    {
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Test Window", 0, ImGuiWindowFlags_AlwaysAutoResize);        

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);       
            ImGui::ColorEdit4("clear color", (float*)&system->graphic->clearColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

            if (ImGui::Button("Button"))  
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            static char InputBuf[256] = {};
            static char ReturnBuf[256] = {};
            if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                string str = string_functions::str_utf8_to_str_ansi(InputBuf);
                cout << str.c_str() << endl;
                strcpy_s(ReturnBuf, InputBuf);
            }

            ImGui::Text("Return: %s", ReturnBuf);

            ImGui::End();
        }

        {
            ImGui::Begin("Info", 0, ImGuiWindowFlags_AlwaysAutoResize);

            tstring resolutionTxt = tstring_format(TEXT("resolution: %f x %f"), system->graphic->GetWidth(), system->graphic->GetHeight());
            ImGui::Text(tstring_to_str_utf8(resolutionTxt).c_str());

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

   //     {
   //         ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize);

   //         V3 camPos = camera->transform->position;
   //         V3 camEuler = camera->transform->eulerAngles;
   //         bool perspective = camera->projection == Camera::Projection::Perspective ? true : false;
   //         float fov = camera->fov;
			//float orthoSize = camera->orthographicSize;

   //         float camPosArr[3] = { camPos.x,camPos.y,camPos.z };
   //         float camEulerArr[3] = { camEuler.x,camEuler.y,camEuler.z };
   //         ImGui::DragFloat3("Position", camPosArr, 0.1f, -1000000, +1000000);
   //         ImGui::DragFloat3("Euler angles", camEulerArr, 1.0f, -360, +360);

			//ImGui::Checkbox("Perspective", &perspective);
			//if (perspective)
			//	ImGui::SliderFloat("FOV", &fov, 0.1f, 180);
			//else
			//	ImGui::SliderFloat("Orthographic Size", &orthoSize, 0.1f, 20);

   //         camera->transform->position = V3(camPosArr[0], camPosArr[1], camPosArr[2]);
   //         camera->transform->eulerAngles = V3(camEulerArr[0], camEulerArr[1], camEulerArr[2]);

   //         camera->projection = perspective ? Camera::Projection::Perspective : Camera::Projection::Orthographic;
   //         camera->fov = fov;
   //         camera->orthographicSize = orthoSize;

   //         ImGui::End();
   //     }

        {
            ImGui::Begin("BlendNode", 0, ImGuiWindowFlags_AlwaysAutoResize);

            float position = blendProperty0->valueAsFloat;
            ImGui::SliderFloat("Blend0", &position, blendNode->minPosition, blendNode->maxPosition);
            blendProperty0->valueAsFloat = position;

            bool trigger0 = triggerProperty0->triggerState;
            ImGui::Checkbox("Trigger0", &trigger0);
            if (trigger0)
                triggerProperty0->SetTriggerState();

            bool trigger1 = triggerProperty1->triggerState;
            ImGui::Checkbox("Trigger1", &trigger1);
            if (trigger1)
                triggerProperty1->SetTriggerState();

            bool trigger2 = triggerProperty2->triggerState;
            ImGui::Checkbox("Trigger2", &trigger2);
            if (trigger2)
                triggerProperty2->SetTriggerState();

            bool trigger3 = triggerProperty3->triggerState;
            ImGui::Checkbox("Trigger3", &trigger3);
            if (trigger3)
                triggerProperty3->SetTriggerState();

            ImGui::End();
        }
    }
}

void TestScene::OnLateUpdate()
{
    GameObject* goCamera = FindGameObjectWithTag(TEXT("Camera"));

    {
        GameObject* goJack = FindGameObject(TEXT("Jack"));
        SkinnedMeshRenderer* skinnedMehsRenderer = goJack->GetComponent<SkinnedMeshRenderer>();
        Ref<NodeTransform> node = skinnedMehsRenderer->GetNodeTransformByName(TEXT("R_Hand_Weapon_cnt_tr"));

        GameObject* goKatana = FindGameObject(TEXT("Katana"));
        goKatana->transform->position = node->position;
        goKatana->transform->rotation = node->rotation;
    }

    {
        // Sword_cnt_tr
        // Scabbard_cnt_tr
        GameObject* goJack = FindGameObject(TEXT("Jack"));
        SkinnedMeshRenderer* skinnedMehsRenderer = goJack->GetComponent<SkinnedMeshRenderer>();
        Ref<NodeTransform> node = skinnedMehsRenderer->GetNodeTransformByName(TEXT("Scabbard_cnt_tr"));

        GameObject* goKatanaScabbard = FindGameObject(TEXT("KatanaSheath"));
        goKatanaScabbard->transform->position = node->position;
        goKatanaScabbard->transform->rotation = node->rotation;
    }

    if (lookCamera)
    {
        GameObject* goJack = FindGameObject(TEXT("Jack"));
        auto coms = goJack->GetComponents<SkinnedMeshRenderer>();
        for (auto& com : coms)
        {
            Ref<NodeTransform> node = com->GetNodeTransformByName(TEXT("Spine"));

            V3 dir = goCamera->transform->position - node->position;
            dir.Normalize();

            Q q = Q::FromToRotation(node->up, dir);
            node->SetRotationWithCurrentRotation(Q::RotateTowards(Q::identity(), q, 60));
        }
    }

    if (system->input->GetKeyDown(Key::Return))
        lookCamera = !lookCamera;
}

void TestScene::OnCollisionEnter(const Collision& collision)
{
    cout << "collision" << endl;
}
