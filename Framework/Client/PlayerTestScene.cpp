#include "stdafx.h"
#include "PlayerTestScene.h"
#include "Player.h"
#include "FreeCamera.h"
#include "JackAnimator.h"

Scene* PlayerTestScene::Clone()
{
	return new PlayerTestScene;
}

void PlayerTestScene::OnLoad()
{
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Dev/Dev.png"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Dev/Normal.png"));

	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Stone 03/Stone03_Base Color.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Stone 03/Stone03_Ambient Occlusion.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Stone 03/Stone03_Normal.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Stone 03/Stone03_Specular Level.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Stone 03/Stone03_Roughness.jpg"));

	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Ground Stones/GroundStones_Ambient Occlusion.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Ground Stones/GroundStones_BaseColor.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Ground Stones/GroundStones_Normal.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Ground Stones/GroundStones_Roughness.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Ground Stones/GroundStones_Specular Level.jpg"));

	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Painted/WoodPlanksPainted_ambient_occlusion.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Painted/WoodPlanksPainted_basecolor.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Painted/WoodPlanksPainted_normal.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Painted/WoodPlanksPainted_glossiness_1.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Painted/WoodPlanksPainted_roughness.jpg"));

	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Wall/Wood_wall_ambient_occlusion.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Wall/Wood_wall_basecolor.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Wall/Wood_wall_glossiness.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Wall/Wood_wall_normal.jpg"));
	system->resource->factory->CreateManagedTexture2DFromFile(TEXT("../Resource/Wood Wall/Wood_wall_roughness.jpg"));


	/*
	{
		thread t0(
			[&]
			{
				system->resource->factory->CreateManagedMeshFromFile(TEXT("../Resource/Character/Jack/Jack.FBX"));
				system->resource->factory->CreateManagedMeshFromFile(TEXT("../Resource/Weapon/Katana/Katana.FBX"));
				system->resource->factory->CreateManagedMeshFromFile(TEXT("../Resource/Weapon/Katana/KatanaSheath.FBX"));
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
				characterRenderer->mesh = system->resource->Find(TEXT("../Resource/Character/Jack/Jack.FBX"));
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
			meshRenderer->mesh = system->resource->builtInResources->boxMesh;
			meshRenderer->material->SetTexture("_DiffuseTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));
			meshRenderer->material->SetTexture("_ReflectionTexture", system->resource->builtInResources->whiteTexture);
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
			meshRenderer->mesh = system->resource->builtInResources->sphereMesh;
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
			meshRenderer->mesh = system->resource->builtInResources->boxMesh;
			meshRenderer->material->SetTexture("_DiffuseTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));
		}
	}
	/*/
	
	//*
	{
		{
			GameObject* goCamera = CreateGameObject(TEXT("Camera"));
			goCamera->transform->position = V3(0, 2, -10);
			goCamera->transform->forward = (V3(0, 2, -9) - V3(0, 2, -10)).normalized;
			FreeCamera* camera = goCamera->AddComponent<FreeCamera>();
			camera->camera->fov = 45;
			camera->camera->postProcessingState = false;
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

		//{
		//	ResourceRef<Texture> texture[6];
		//	texture[0] = system->resourceManagement->factory->CreateUnmanagedTexture2DFromFile(TEXT("../Resource/Skybox/top.png"));
		//	texture[1] = system->resourceManagement->factory->CreateUnmanagedTexture2DFromFile(TEXT("../Resource/Skybox/bottom.png"));
		//	texture[2] = system->resourceManagement->factory->CreateUnmanagedTexture2DFromFile(TEXT("../Resource/Skybox/left.png"));
		//	texture[3] = system->resourceManagement->factory->CreateUnmanagedTexture2DFromFile(TEXT("../Resource/Skybox/right.png"));
		//	texture[4] = system->resourceManagement->factory->CreateUnmanagedTexture2DFromFile(TEXT("../Resource/Skybox/forward.png"));
		//	texture[5] = system->resourceManagement->factory->CreateUnmanagedTexture2DFromFile(TEXT("../Resource/Skybox/back.png"));

		//	ResourceRef<Shader> priority = system->resourceManagement->factory->CreateManagedShaderFromBinaryFolder(TEXT("Priority.cso"));
		//	ResourceRef<Material> material[6];
		//	for (uint i = 0; i < 6; ++i)
		//	{
		//		material[i] = system->resourceManagement->factory->CreateUnmanagedMaterialByShader(priority);
		//		material[i]->SetTexture("_Texture", texture[i]);
		//	}

		//	GameObject* goSky[6];
		//	for (uint i = 0; i < 6; ++i)
		//	{
		//		goSky[i] = CreateGameObject();
		//		MeshRenderer* mr = goSky[i]->AddComponent<MeshRenderer>();
		//		mr->mesh = system->resourceManagement->builtInResources->quadMesh;
		//		mr->material = material[i];
		//	}

		//	goSky[0]->transform->position = 0.25f * V3::up();
		//	goSky[1]->transform->position = 0.25f * V3::down();
		//	goSky[2]->transform->position = 0.25f * V3::left();
		//	goSky[3]->transform->position = 0.25f * V3::right();
		//	goSky[4]->transform->position = 0.25f * V3::forward();
		//	goSky[5]->transform->position = 0.25f * V3::back();

		//	goSky[0]->transform->forward = -V3::up();
		//	goSky[1]->transform->forward = -V3::down();
		//	goSky[2]->transform->forward = -V3::left();
		//	goSky[3]->transform->forward = -V3::right();
		//	goSky[4]->transform->forward = -V3::forward();
		//	goSky[5]->transform->forward = -V3::back();
		//}

		{
			ResourceRef<Texture> texture = system->resource->factory->CreateUnmanagedTexture2DFromFile(TEXT("../Resource/Skybox/skybox.png"));

			//ResourceRef<Shader> skybox = system->resource->factory->CreateManagedShaderFromFile(TEXT("../Shader/Skybox.fx"));
			ResourceRef<Shader> skybox = system->resource->factory->CreateManagedShaderFromBinaryFolder(TEXT("Skybox.cso"));
			ResourceRef<Material> material;
			material = system->resource->factory->CreateUnmanagedMaterialByShader(skybox);
			material->SetTexture("_Texture", texture);

			GameObject* goSky;
			goSky = CreateGameObject();
			MeshRenderer* mr = goSky->AddComponent<MeshRenderer>();
			mr->mesh = system->resource->builtInResources->skyboxMesh;
			mr->material = material;
		}

		{
			GameObject* goGround = CreateGameObject();
			goGround->transform->position = V3(0.0f, 0.5f, 0.0f);
			goGround->transform->localScale = V3(40, 1, 40);

			MeshRenderer* meshRenderer = goGround->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtInResources->boxMesh;

			ResourceRef<Material> standardMaterial = system->resource->factory->CopyUnmanagedMaterial(system->resource->builtInResources->standardMaterial);
			standardMaterial->SetTexture("_DiffuseTexture", system->resource->Find(TEXT("../Resource/Wood Wall/Wood_wall_basecolor.jpg")));
			standardMaterial->SetTexture("_ReflectionTexture", system->resource->Find(TEXT("../Resource/Wood Wall/Wood_wall_ambient_occlusion.jpg")));
			standardMaterial->SetTexture("_OcclusionTexture", system->resource->Find(TEXT("../Resource/Wood Wall/Wood_wall_ambient_occlusion.jpg")));
			standardMaterial->SetTexture("_NormalMapTexture", system->resource->Find(TEXT("../Resource/Wood Wall/Wood_wall_normal.jpg")));
			standardMaterial->SetFloat("_ReflectionTransparency", 0.0f);
			standardMaterial->SetFloat("_SpecularTransparency", 0.5f);
			standardMaterial->SetFloat("_SpecularPower", 20.0f); 

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
			meshRenderer->mesh = system->resource->builtInResources->boxMesh;
			meshRenderer->material->SetTexture("_DiffuseTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));
			meshRenderer->material->SetTexture("_NormalMapTexture", system->resource->Find(TEXT("../Resource/Dev/Normal.png")));

			meshRenderer->material->SetTexture("_DiffuseTexture", system->resource->Find(TEXT("../Resource/Ground Stones/GroundStones_BaseColor.jpg")));
			meshRenderer->material->SetTexture("_SpecularMapTexture", system->resource->Find(TEXT("../Resource/Ground Stones/GroundStones_Specular Level.jpg")));
			meshRenderer->material->SetTexture("_OcclusionTexture", system->resource->Find(TEXT("../Resource/Ground Stones/GroundStones_Ambient Occlusion.jpg")));
			meshRenderer->material->SetTexture("_NormalMapTexture", system->resource->Find(TEXT("../Resource/Ground Stones/GroundStones_Normal.jpg")));
			meshRenderer->material->SetFloat("_SpecularTransparency", 0.5f);
			meshRenderer->material->SetFloat("_SpecularPower", 20.0f);

			//ResourceRef<Material> m = Material::CreateUnmanagedMaterialFromJson(system->resourceManagement, TEXT("../Resource/test.material"));
			//meshRenderer->material = m;

			goBox->AddComponent<Rigidbody>();
			goBox->AddComponent<BoxCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(2.0f, 1.5f, 0.0f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		//{
		//	m_sphere[0] = CreateGameObject(TEXT("Sphere"));

		//	MeshRenderer* meshRenderer = m_sphere[0]->AddComponent<MeshRenderer>();
		//	meshRenderer->mesh = system->resource->builtInResources->sphereMesh;

		//	m_trailRenderer[0] = m_sphere[0]->AddComponent<TrailRenderer>();

		//	m_rigidbody = m_sphere[0]->AddComponent<Rigidbody>();
		//	m_rigidbody->sleepThresholder = 1000;
		//	m_sphere[0]->AddComponent<SphereCollider>();

		//	ResourceRef<Shader> shader = system->resource->factory->CreateManagedShaderFromBinaryFolder(TEXT("Trail.cso"));
		//	ResourceRef<Material> material = system->resource->factory->CreateUnmanagedMaterialByShader(shader);
		//	material->SetTexture("_NormalMapTexture", system->resource->Find(TEXT("../Resource/Dev/Normal.png")));

		//	m_trailRenderer[0]->material = material;

		//	m_sphere[0]->activeSelf = true;
		//}

		//{
		//	m_sphere[1] = CreateGameObject(TEXT("Sphere"));

		//	MeshRenderer* meshRenderer = m_sphere[1]->AddComponent<MeshRenderer>();
		//	meshRenderer->mesh = system->resource->builtInResources->sphereMesh;

		//	m_trailRenderer[1] = m_sphere[1]->AddComponent<TrailRenderer>();

		//	m_rigidbody = m_sphere[1]->AddComponent<Rigidbody>();
		//	m_rigidbody->sleepThresholder = 1000;
		//	m_sphere[1]->AddComponent<SphereCollider>();

		//	ResourceRef<Shader> shader = system->resource->factory->CreateManagedShaderFromBinaryFolder(TEXT("Trail.cso"));
		//	ResourceRef<Material> material = system->resource->factory->CreateUnmanagedMaterialByShader(shader);
		//	material->SetTexture("_NormalMapTexture", system->resource->Find(TEXT("../Resource/Dev/Normal.png")));

		//	m_trailRenderer[1]->material = material;

		//	m_sphere[1]->activeSelf = true;
		//}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(-4.5f, 1.5f, +4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(+4.5f, 1.5f, +4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(+4.5f, 1.5f, -4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtInResources->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(-4.5f, 1.5f, -4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtInResources->sphereMesh;

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
	if (m_sphere[0])
	{
		V3 spin = V3(cos(system->time->accumulatedSinceStartup), 0.0f, sin(system->time->accumulatedSinceStartup));
		m_sphere[0]->transform->position = V3::up() * 2.0f + spin * 6.0f;
		m_trailRenderer[0]->AddPosition(m_sphere[0]->transform->position);
	}

	if (m_sphere[1])
	{
		V3 spin = V3(cos(system->time->accumulatedSinceStartup), 0.0f, sin(system->time->accumulatedSinceStartup));
		m_sphere[1]->transform->position = V3::up() * 2.0f + spin * 6.0f + V3(0, 0, -8.0f);
		m_trailRenderer[1]->AddPosition(m_sphere[1]->transform->position);
	}

	{
		ImGui::Begin("Info", 0, ImGuiWindowFlags_AlwaysAutoResize);

		tstring resolutionTxt = tstring_format(TEXT("resolution: %d x %d"), int(system->graphic->GetWidth()), int(system->graphic->GetHeight()));
		ImGui::Text(tstring_to_str_utf8(resolutionTxt).c_str());

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	Camera* camera = (Camera*)system->graphic->cameraManager->mainCamera;
	ImGui::Begin("Camera");
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::PushID("Camera");

		float Near = camera->Near;
		ImGui::SliderFloat("Near", &Near, 0.0f, camera->Far);
		camera->Near = Near;

		float Far = camera->Far;
		ImGui::SliderFloat("Far", &Far, 0.0f, 1000.0f);
		camera->Far = Far;

		float FOV = camera->fov;
		ImGui::SliderFloat("FOV", &FOV, 0.0f, 180.0f);
		camera->fov = FOV;

		bool postProcessing = camera->postProcessingState;
		ImGui::Checkbox("Enable", &postProcessing);
		camera->postProcessingState = postProcessing;

		ImGui::PopID();
	}
	if(ImGui::CollapsingHeader("SSAO"))
	{
		ImGui::PushID("SSAO");

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
		ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 2000.0f);
		ssaoDesc.BlurPixelDistance = blurPixelDistance;

		camera->ssaoDesc = ssaoDesc;

		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("SSR"))
	{
		ImGui::PushID("SSR");

		SSRDesc ssrDesc = camera->ssrDesc;

		bool enable = ssrDesc.Enable;
		ImGui::Checkbox("Enable", &enable);
		ssrDesc.Enable = enable;

		bool blurEnable = ssrDesc.BlurEnable;
		ImGui::Checkbox("BlurEnable", &blurEnable);
		ssrDesc.BlurEnable = blurEnable;

		const char* items[] = { "Default", "InvDepth", "Depth" };
		int blurType = (int)ssrDesc.BlurType;
		ImGui::Combo("BlurType", &blurType, items, 3);
		ssrDesc.BlurType = (BlurType)blurType;

		int numSamples = (int)ssrDesc.NumSamples;
		ImGui::SliderInt("NumSamples", &numSamples, 0, 256);
		ssrDesc.NumSamples = (uint)numSamples;

		int blurNumSamples = (int)ssrDesc.BlurNumSamples;
		ImGui::SliderInt("BlurNumSamples", &blurNumSamples, 0, 16);
		ssrDesc.BlurNumSamples = (uint)blurNumSamples;

		float step = ssrDesc.Step;
		ImGui::SliderFloat("Step", &step, 0.0f, 0.4f);
		ssrDesc.Step = step;

		float thickness = ssrDesc.Thickness;
		ImGui::SliderFloat("Thickness", &thickness, 0.0f, 1.0f);
		ssrDesc.Thickness = thickness;

		float bias = ssrDesc.Bias;
		ImGui::SliderFloat("Bias", &bias, 0.0f, 1.0f);
		ssrDesc.Bias = bias;

		float blurPixelDistance = ssrDesc.BlurPixelDistance;
		ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 1000.0f);
		ssrDesc.BlurPixelDistance = blurPixelDistance;

		camera->ssrDesc = ssrDesc;

		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("DOF"))
	{
		ImGui::PushID("DOF");

		DOFDesc dofDesc = camera->dofDesc;

		bool enable = dofDesc.Enable;
		ImGui::Checkbox("Enable", &enable);
		dofDesc.Enable = enable;

		int blurNumSamples = (int)dofDesc.BlurNumSamples;
		ImGui::SliderInt("BlurNumSamples", &blurNumSamples, 0, 16);
		dofDesc.BlurNumSamples = (uint)blurNumSamples;

		float minZ = dofDesc.MinZ;
		ImGui::SliderFloat("MinZ", &minZ, 0.0f, 100.0f);
		dofDesc.MinZ = minZ;

		float rangeZ = dofDesc.RangeZ;
		ImGui::SliderFloat("RangeZ", &rangeZ, 0.0f, 100.0f);
		dofDesc.RangeZ = rangeZ;

		float power = dofDesc.Power;
		ImGui::SliderFloat("Power", &power, 0.0f, 100.0f);
		dofDesc.Power = power;

		float blurPixelDistance = dofDesc.BlurPixelDistance;
		ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 1000.0f);
		dofDesc.BlurPixelDistance = blurPixelDistance;

		camera->dofDesc = dofDesc;

		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("Fog"))
	{
		ImGui::PushID("Fog");

		FogDesc fogDesc = camera->fogDesc;

		bool enable = fogDesc.Enable;
		ImGui::Checkbox("Enable", &enable);
		fogDesc.Enable = enable;

		const char* items[] = { "Distance", "Z" };
		int type = (int)fogDesc.Type;
		ImGui::Combo("Type", &type, items, 2);
		fogDesc.Type = (FogType)type;

		float minZ = fogDesc.MinZ;
		ImGui::SliderFloat("MinZ", &minZ, 0.0f, 100.0f);
		fogDesc.MinZ = minZ;

		float rangeZ = fogDesc.RangeZ;
		ImGui::SliderFloat("RangeZ", &rangeZ, 0.0f, 100.0f);
		fogDesc.RangeZ = rangeZ;

		Color color = fogDesc.Color;
		ImGui::ColorPicker4("Color", (float*)&color);
		fogDesc.Color = color;

		camera->fogDesc = fogDesc;

		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("Bloom"))
	{
		ImGui::PushID("Bloom");

		BloomDesc bloomDesc = camera->bloomDesc;

		bool enable = bloomDesc.Enable;
		ImGui::Checkbox("Enable", &enable);
		bloomDesc.Enable = enable;

		const char* items[] = { "Add", "Mix" };
		int type = (int)bloomDesc.Type;
		ImGui::Combo("Type", &type, items, 2);
		bloomDesc.Type = (BloomType)type;

		int blurNumSamples = (int)bloomDesc.BlurNumSamples;
		ImGui::SliderInt("BlurNumSamples", &blurNumSamples, 0, 16);
		bloomDesc.BlurNumSamples = (uint)blurNumSamples;

		float intensity = bloomDesc.Intensity;
		ImGui::SliderFloat("Intensity", &intensity, 0.0f, 100.0f);
		bloomDesc.Intensity = intensity;

		float threshold = bloomDesc.Threshold;
		ImGui::SliderFloat("Threshold", &threshold, 0.0f, 1.0f);
		bloomDesc.Threshold = threshold;

		float blurPixelDistance = bloomDesc.BlurPixelDistance;
		ImGui::SliderFloat("BlurPixelDistance", &blurPixelDistance, 0.0f, 100.0f);
		bloomDesc.BlurPixelDistance = blurPixelDistance;

		camera->bloomDesc = bloomDesc;

		ImGui::PopID();
	}	
	if (ImGui::CollapsingHeader("ChromaticAberration"))
	{
		ImGui::PushID("ChromaticAberration");

		ChromaticAberrationDesc chromaticAberrationDesc = camera->chromaticAberrationDesc;

		bool enable = chromaticAberrationDesc.Enable;
		ImGui::Checkbox("Enable", &enable);
		chromaticAberrationDesc.Enable = enable;

		ImGui::SliderFloat3("Blend", (float*)&chromaticAberrationDesc.Blend, 0.0f, 1.0f);

		ImGui::SliderFloat3("Offset", (float*)&chromaticAberrationDesc.Offset, 0.0f, 50.0f);

		ImGui::SliderFloat3("Angle", (float*)&chromaticAberrationDesc.Angle, 0.0f, 360.0f);

		camera->chromaticAberrationDesc = chromaticAberrationDesc;

		ImGui::PopID();
	}
	ImGui::End();

	ImGui::Begin("Lights");
	if (ImGui::CollapsingHeader("Directional Light"))
	{
		ImGui::PushID("Directional Light");

		GameObject* goDirectionalLight = FindGameObject(TEXT("DirectionalLight"));
		DirectionalLight* light = goDirectionalLight->GetComponent<DirectionalLight>();

		V3 eulerAngles = goDirectionalLight->transform->eulerAngles;
		ImGui::SliderFloat3("Euler Angles", (float*)&eulerAngles, -180, +180);
		goDirectionalLight->transform->eulerAngles = eulerAngles;

		float intensity = light->intensity;
		ImGui::SliderFloat("Intensity", &intensity, 0, 60);
		light->intensity = intensity;

		float shadowWhiteness = light->shadowWhiteness;
		ImGui::SliderFloat("Shadow Whiteness", &shadowWhiteness, 0.0f, 1.0f);
		light->shadowWhiteness = shadowWhiteness;

		float shadowBias = light->shadowBias;
		ImGui::SliderFloat("Shadow Bias", &shadowBias, 0.0f, 1.0f);
		light->shadowBias = shadowBias;

		bool drawShadow = light->drawShadow;
		ImGui::Checkbox("Draw Shadow", &drawShadow);
		light->drawShadow = drawShadow;

		Color color = light->diffuse;
		ImGui::ColorPicker3("Color", (float*)&color);
		light->diffuse = color;

		Color ambient = light->ambient;
		ImGui::ColorPicker3("Ambient", (float*)&ambient);
		light->ambient = ambient;

		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("Spot Light"))
	{
		ImGui::PushID("Spot Light");

		GameObject* goSpotLight = FindGameObject(TEXT("SpotLight"));
		SpotLight* light = goSpotLight->GetComponent<SpotLight>();

		V3 eulerAngles = goSpotLight->transform->eulerAngles;
		ImGui::SliderFloat3("Euler Angles", (float*)&eulerAngles, -180, +180);
		goSpotLight->transform->eulerAngles = eulerAngles;

		ImGui::DragFloat3("Positions", (float*)&goSpotLight->transform->position, 0.1f, -25, +25);

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

		float shadowBias = light->shadowBias;
		ImGui::SliderFloat("Shadow Bias", &shadowBias, 0.0f, 1.0f);
		light->shadowBias = shadowBias;

		bool drawShadow = light->drawShadow;
		ImGui::Checkbox("Draw Shadow", &drawShadow);
		light->drawShadow = drawShadow;

		Color color = light->diffuse;
		ImGui::ColorPicker3("Color", (float*)&color);
		light->diffuse = color;

		Color ambient = light->ambient;
		ImGui::ColorPicker3("Ambient", (float*)&ambient);
		light->ambient = ambient;

		ImGui::PopID();
	}
	if (ImGui::CollapsingHeader("Point Light"))
	{
		ImGui::PushID("Point Light");

		GameObject* goPointLight = FindGameObject(TEXT("PointLight"));
		PointLight* light = goPointLight->GetComponent<PointLight>();

		ImGui::DragFloat3("Positions", (float*)&goPointLight->transform->position, 0.1f, -25, +25);

		float range = light->range;
		ImGui::SliderFloat("Range", &range, 0, 60);
		light->range = range;

		float intensity = light->intensity;
		ImGui::SliderFloat("Intensity", &intensity, 0, 60);
		light->intensity = intensity;

		float shadowWhiteness = light->shadowWhiteness;
		ImGui::SliderFloat("Shadow Whiteness", &shadowWhiteness, 0.0f, 1.0f);
		light->shadowWhiteness = shadowWhiteness;

		float shadowBias = light->shadowBias;
		ImGui::SliderFloat("Shadow Bias", &shadowBias, 0.0f, 1.0f);
		light->shadowBias = shadowBias;

		bool drawShadow = light->drawShadow;
		ImGui::Checkbox("Draw Shadow", &drawShadow);
		light->drawShadow = drawShadow;

		Color color = light->diffuse;
		ImGui::ColorPicker3("Color", (float*)&color);
		light->diffuse = color;

		Color ambient = light->ambient;
		ImGui::ColorPicker3("Ambient", (float*)&ambient);
		light->ambient = ambient;

		ImGui::PopID();
	}
	ImGui::End();
}

void PlayerTestScene::OnLateUpdate()
{
}
