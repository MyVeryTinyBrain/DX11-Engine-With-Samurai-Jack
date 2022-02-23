#include "stdafx.h"
#include "PlayerTestScene.h"
#include "Player.h"
#include "FreeCamera.h"
#include "JackAnimator.h"
#include "MaterialTransparent.h"

Scene* PlayerTestScene::Clone()
{
	return new PlayerTestScene;
}

void PlayerTestScene::OnLoad()
{
	system->resourceManagement->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Dev/Dev.png"));
	system->resourceManagement->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Dev/Normal.png"));

	/*
	{
		thread t0(
			[&]
			{
				system->resourceManagement->factory->CreateManagedMeshFromFile(TEXT("../Resource/Character/Jack/Jack.FBX"));
				system->resourceManagement->factory->CreateManagedMeshFromFile(TEXT("../Resource/Weapon/Katana/Katana.FBX"));
				system->resourceManagement->factory->CreateManagedMeshFromFile(TEXT("../Resource/Weapon/Katana/KatanaSheath.FBX"));
			});
		if (t0.joinable())
			t0.join();

		{
			GameObject* goPlayer = CreateGameObject(TEXT("Jack"));
			goPlayer->AddComponent<Player>();
		}

#ifndef _DEBUG
		for (uint i = 0; i < 10; ++i)
		{
			for (uint j = 0; j < 2; ++j)
			{
				GameObject* goJack = CreateGameObject(TEXT("Jack"));
				goJack->transform->position = V3(2.0f + float(i * 2), 0, 2.0f + float(j * 2.0f));

				GameObject* goCharacterRender = CreateGameObjectToChild(goJack->transform);
				goCharacterRender->transform->localEulerAngles = V3(90, 180, 0);
				SkinnedMeshRenderer* characterRenderer = goCharacterRender->AddComponent<SkinnedMeshRenderer>();
				characterRenderer->mesh = system->resourceManagement->Find(TEXT("../Resource/Character/Jack/Jack.FBX"));
				goCharacterRender->AddComponent<JackAnimator>();

				Rigidbody* rigidbody = goJack->AddComponent<Rigidbody>();
				rigidbody->SetRotationLock(Rigidbody::Axis::All, true);
				rigidbody->SetInterpolateMode(Rigidbody::Interpolate::Interpolate);
				rigidbody->sleepThresholder = 5.0f;

				GameObject* goCollider = CreateGameObjectToChild(goJack->transform);
				goCollider->transform->localPosition = V3::up() * 1.0f;
				CapsuleCollider* collider = goCollider->AddComponent<CapsuleCollider>();
			}
		}
#endif

		{
			GameObject* goGround = CreateGameObject();
			goGround->transform->position = V3::down() * 1.0f;
			goGround->transform->localScale = V3(100, 1, 100);

			Rigidbody* rigidbody = goGround->AddComponent<Rigidbody>();
			rigidbody->kinematic = true;
			BoxCollider* boxCollider = goGround->AddComponent<BoxCollider>();
			boxCollider->friction = 1.0f;
			boxCollider->restitution = 1.0f;

			MeshRenderer* meshRenderer = goGround->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->boxMesh;
			meshRenderer->material->diffuseTexture = system->resourceManagement->Find(TEXT("../Resource/Dev/Dev.png"));
		}

		{
			GameObject* goDirectionalLight = CreateGameObject(TEXT("DirectionalLight"));
			goDirectionalLight->transform->position = V3(9.0f, 8.0f, -0.1f);
			goDirectionalLight->transform->forward = -V3(9.0f, 8.0f, -0.1f).normalized;
			DirectionalLight* directionalLight = goDirectionalLight->AddComponent<DirectionalLight>();
			directionalLight->enable = true;
			directionalLight->drawShadow = true;
			directionalLight->diffuse = Color(0.25f, 0.25f, 0.25f, 1.0f);
		}

		{
			GameObject* goPointLight = CreateGameObject(TEXT("PointLight"));
			goPointLight->transform->position = V3(0, 5, 0);
			PointLight* pointLight = goPointLight->AddComponent<PointLight>();
			pointLight->range = 10.0f;
			pointLight->drawShadow = true;
			pointLight->enable = true;
		}

		{
			//GameObject* goSpotLight = CreateGameObjectToChild(FindGameObject(TEXT("Camera"))->transform, TEXT("SpotLight"));
			GameObject* goSpotLight = CreateGameObject(TEXT("SpotLight"));
			goSpotLight->transform->position = V3(0, 2, -20);
			goSpotLight->transform->forward = V3::forward();
			SpotLight* spotLight = goSpotLight->AddComponent<SpotLight>();
			spotLight->range = 20.0f;
			spotLight->angle = 10.0f;
			spotLight->drawShadow = true;
			spotLight->enable = true;
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(0, 1.0f, 0);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->sphereMesh;
		}

		{
			GameObject* goPriorityRender = CreateGameObject();
			goPriorityRender->transform->eulerAngles = V3(-20, 0, 0);
			goPriorityRender->transform->localScale = V3::one() * 5;
			goPriorityRender->transform->position = V3(0, 0, -5);

			Rigidbody* rigidbody = goPriorityRender->AddComponent<Rigidbody>();
			rigidbody->kinematic = true;
			BoxCollider* boxCollider = goPriorityRender->AddComponent<BoxCollider>();
			boxCollider->friction = 100.0f;

			MeshRenderer* meshRenderer = goPriorityRender->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->boxMesh;
			meshRenderer->material->diffuseTexture = system->resourceManagement->Find(TEXT("../Resource/Dev/Dev.png"));
		}
	}
	//*/

	//*
	{
		{
			GameObject* goCamera = CreateGameObject(TEXT("Camera"));
			goCamera->transform->position = V3(0, 2, -10);
			goCamera->transform->forward = (V3(0, 2, -9) - V3(0, 2, -10)).normalized;
			FreeCamera* camera = goCamera->AddComponent<FreeCamera>();
			camera->camera->fov = 45;
		}

		{
			GameObject* goDirectionalLight = CreateGameObject(TEXT("DirectionalLight"));
			goDirectionalLight->transform->position = V3(9.0f, 8.0f, -0.1f);
			goDirectionalLight->transform->forward = -V3(9.0f, 8.0f, -0.1f).normalized;
			DirectionalLight* directionalLight = goDirectionalLight->AddComponent<DirectionalLight>();
			directionalLight->drawShadow = true;
			directionalLight->enable = true;
			directionalLight->diffuse = Color(0.25f, 0.25f, 0.25f, 1.0f);
		}

		{
			GameObject* goPointLight = CreateGameObject(TEXT("PointLight"));
			goPointLight->transform->position = V3(0, 5, 0);
			PointLight* pointLight = goPointLight->AddComponent<PointLight>();
			pointLight->range = 10.0f;
			pointLight->drawShadow = true;
			pointLight->enable = true;
		}

		{
			GameObject* goSpotLight = CreateGameObject(TEXT("SpotLight"));
			goSpotLight->transform->position = V3(0, 2, -10);
			goSpotLight->transform->forward = V3::forward();
			SpotLight* spotLight = goSpotLight->AddComponent<SpotLight>();
			spotLight->range = 30.0f;
			spotLight->angle = 40.0f;
			spotLight->intensity = 5.0f;
			spotLight->drawShadow = true;
			spotLight->enable = true;
		}

		{
			GameObject* goGround = CreateGameObject();
			goGround->transform->position = V3(0.0f, 0.5f, 0.0f);
			goGround->transform->localScale = V3(40, 1, 40);

			MeshRenderer* meshRenderer = goGround->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->boxMesh;

			ResourceRef<MaterialStandard> standardMaterial = system->resourceManagement->factory->CreateUnmanagedMaterial<MaterialStandard>();
			standardMaterial->diffuseTexture = system->resourceManagement->Find(TEXT("../Resource/Dev/Dev.png"));
			meshRenderer->material = standardMaterial;

			Rigidbody* rigidbody = goGround->AddComponent<Rigidbody>();
			rigidbody->kinematic = true;
			BoxCollider* boxCollider = goGround->AddComponent<BoxCollider>();
			boxCollider->friction = 1.0f;
			boxCollider->restitution = 1.0f;
		}

		{
			GameObject* goBox = CreateGameObject(TEXT("Box"));
			goBox->transform->position = V3(-2.0f, 1.5f, 0.0f);

			MeshRenderer* meshRenderer = goBox->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->boxMesh;
			meshRenderer->material->diffuseTexture = system->resourceManagement->Find(TEXT("../Resource/Dev/Dev.png"));
			ResourceRef<MaterialStandard> standardMaterial = meshRenderer->material;
			standardMaterial->normalMap = system->resourceManagement->Find(TEXT("../Resource/Dev/Normal.png"));

			goBox->AddComponent<Rigidbody>();
			goBox->AddComponent<BoxCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(2.0f, 1.5f, 0.0f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(-4.5f, 1.5f, +4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(+4.5f, 1.5f, +4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(+4.5f, 1.5f, -4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(-4.5f, 1.5f, -4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resourceManagement->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}
	}
	//*/
}

void PlayerTestScene::OnUnload()
{
}

void PlayerTestScene::OnUpdate()
{
	{
		ImGui::Begin("Info", 0, ImGuiWindowFlags_AlwaysAutoResize);

		tstring resolutionTxt = tstring_format(TEXT("resolution: %d x %d"), int(system->graphicSystem->GetWidth()), int(system->graphicSystem->GetHeight()));
		ImGui::Text(tstring_to_str_utf8(resolutionTxt).c_str());

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	Camera* camera = (Camera*)system->graphicSystem->cameraManager->mainCamera;
	ImGui::Begin("Camera");
	if (ImGui::CollapsingHeader("Camera"))
	{
		float Near = camera->Near;
		ImGui::SliderFloat("Near", &Near, 0.0f, camera->Far);
		camera->Near = Near;

		float Far = camera->Far;
		ImGui::SliderFloat("Far", &Far, 0.0f, 1000.0f);
		camera->Far = Far;

		float FOV = camera->fov;
		ImGui::SliderFloat("FOV", &FOV, 0.0f, 180.0f);
		camera->fov = FOV;
	}
	if(ImGui::CollapsingHeader("SSAO"))
	{
		SSAODesc ssaoDesc = camera->ssaoDesc;

		bool enable = ssaoDesc.Enable;
		ImGui::Checkbox("Enable", &enable);
		ssaoDesc.Enable = enable;

		int numSamples = (int)ssaoDesc.NumSamples;
		ImGui::SliderInt("NumSamples", &numSamples, 0, 16);
		ssaoDesc.NumSamples = (uint)numSamples;

		float transparency = ssaoDesc.Transparency;
		ImGui::SliderFloat("Transparency", &transparency, 0.0f, 1.0f);
		ssaoDesc.Transparency = transparency;

		float minZ = ssaoDesc.MinZ;
		ImGui::SliderFloat("MinZ", &minZ, 0.0f, 0.1f);
		ssaoDesc.MinZ = minZ;

		float radius = ssaoDesc.Radius;
		ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f);
		ssaoDesc.Radius = radius;

		float power = ssaoDesc.Power;
		ImGui::SliderFloat("Power", &power, 0.0f, 10.0f);
		ssaoDesc.Power = power;

		int blurNumSamples = (int)ssaoDesc.BlurNumSamples;
		ImGui::SliderInt("BlurNumSamples", &blurNumSamples, 0, 16);
		ssaoDesc.BlurNumSamples = (uint)blurNumSamples;

		float blurPixelDistance = ssaoDesc.BlurPixelDistance;
		ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 1000.0f);
		ssaoDesc.BlurPixelDistance = blurPixelDistance;

		camera->ssaoDesc = ssaoDesc;
	}
	ImGui::End();

	ImGui::Begin("Lights");
	if (ImGui::CollapsingHeader("Directional Light"))
	{
		GameObject* goDirectionalLight = FindGameObject(TEXT("DirectionalLight"));
		DirectionalLight* light = goDirectionalLight->GetComponent<DirectionalLight>();

		float eulerAngles[3] = { goDirectionalLight->transform->eulerAngles.x, goDirectionalLight->transform->eulerAngles.y, goDirectionalLight->transform->eulerAngles.z };
		ImGui::SliderFloat3("Euler Angles", eulerAngles, -180, +180);
		goDirectionalLight->transform->eulerAngles = V3(eulerAngles[0], eulerAngles[1], eulerAngles[2]);

		float intensity = light->intensity;
		ImGui::SliderFloat("Intensity", &intensity, 0, 60);
		light->intensity = intensity;

		float shadowWhiteness = light->shadowWhiteness;
		ImGui::SliderFloat("Shadow Whiteness", &shadowWhiteness, 0.0f, 1.0f);
		light->shadowWhiteness = shadowWhiteness;

		bool drawShadow = light->drawShadow;
		ImGui::Checkbox("Draw Shadow", &drawShadow);
		light->drawShadow = drawShadow;

		Color color = light->diffuse;
		ImGui::ColorPicker3("Color", (float*)&color);
		light->diffuse = color;

		Color ambient = light->ambient;
		ImGui::ColorPicker3("Ambient", (float*)&ambient);
		light->ambient = ambient;
	}
	if (ImGui::CollapsingHeader("Spot Light"))
	{
		GameObject* goSpotLight = FindGameObject(TEXT("SpotLight"));
		SpotLight* light = goSpotLight->GetComponent<SpotLight>();

		float eulerAngles[3] = { goSpotLight->transform->eulerAngles.x, goSpotLight->transform->eulerAngles.y, goSpotLight->transform->eulerAngles.z };
		ImGui::SliderFloat3("Euler Angles", eulerAngles, -180, +180);
		goSpotLight->transform->eulerAngles = V3(eulerAngles[0], eulerAngles[1], eulerAngles[2]);

		float positions[3] = { goSpotLight->transform->position.x, goSpotLight->transform->position.y, goSpotLight->transform->position.z };
		ImGui::DragFloat3("Positions", positions, 0.1f, -25, +25);
		goSpotLight->transform->position = V3(positions[0], positions[1], positions[2]);

		float angle = light->angle;
		ImGui::SliderFloat("Angle", &angle, 0, 90);
		light->angle = angle;

		float range = light->range;
		ImGui::SliderFloat("Range", &range, 0, 60);
		light->range = range;

		float intensity = light->intensity;
		ImGui::SliderFloat("Intensity", &intensity, 0, 60);
		light->intensity = intensity;

		float shadowWhiteness = light->shadowWhiteness;
		ImGui::SliderFloat("Shadow Whiteness", &shadowWhiteness, 0.0f, 1.0f);
		light->shadowWhiteness = shadowWhiteness;

		bool drawShadow = light->drawShadow;
		ImGui::Checkbox("Draw Shadow", &drawShadow);
		light->drawShadow = drawShadow;

		Color color = light->diffuse;
		ImGui::ColorPicker3("Color", (float*)&color);
		light->diffuse = color;

		Color ambient = light->ambient;
		ImGui::ColorPicker3("Ambient", (float*)&ambient);
		light->ambient = ambient;
	}
	if (ImGui::CollapsingHeader("Point Light"))
	{
		GameObject* goPointLight = FindGameObject(TEXT("PointLight"));
		PointLight* light = goPointLight->GetComponent<PointLight>();

		float positions[3] = { goPointLight->transform->position.x, goPointLight->transform->position.y, goPointLight->transform->position.z };
		ImGui::DragFloat3("Positions", positions, 0.1f, -25, +25);
		goPointLight->transform->position = V3(positions[0], positions[1], positions[2]);

		float range = light->range;
		ImGui::SliderFloat("Range", &range, 0, 60);
		light->range = range;

		float intensity = light->intensity;
		ImGui::SliderFloat("Intensity", &intensity, 0, 60);
		light->intensity = intensity;

		float shadowWhiteness = light->shadowWhiteness;
		ImGui::SliderFloat("Shadow Whiteness", &shadowWhiteness, 0.0f, 1.0f);
		light->shadowWhiteness = shadowWhiteness;

		bool drawShadow = light->drawShadow;
		ImGui::Checkbox("Draw Shadow", &drawShadow);
		light->drawShadow = drawShadow;

		Color color = light->diffuse;
		ImGui::ColorPicker3("Color", (float*)&color);
		light->diffuse = color;

		Color ambient = light->ambient;
		ImGui::ColorPicker3("Ambient", (float*)&ambient);
		light->ambient = ambient;
	}
	ImGui::End();
}

void PlayerTestScene::OnLateUpdate()
{
}
