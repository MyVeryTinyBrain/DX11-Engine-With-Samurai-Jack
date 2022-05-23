#include "stdafx.h"
#include "PlayerTestScene.h"
#include "Player.h"
#include "FreeCamera.h"
#include "SoundTest.h"
#include "EnemyBeetleDrone.h"
#include "Config.h"

Scene* PlayerTestScene::Clone()
{
	return new PlayerTestScene;
}

void PlayerTestScene::OnLoad()
{
	system->physics->layerManager->SetCollision(PhysicsLayer_Default, PhysicsLayer_Player, true);
	system->physics->layerManager->SetCollision(PhysicsLayer_Default, PhysicsLayer_Enemy, true);
	system->physics->layerManager->SetCollision(PhysicsLayer_Player, PhysicsLayer_Enemy, true);

	TextureOptionDesc loadDesc = {};
	TextureOptionDesc loadMipDesc = {};
	loadMipDesc.GenerateMipmap = true;

	system->resource->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Dev/Dev.png"));
	system->resource->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Dev/Normal.png"));

	{
		GameObject* goLine = CreateGameObject();
		goLine->transform->position = V3(0, 5, -5);
		LineRenderer* line = goLine->AddComponent<LineRenderer>();

		for (int i = 0; i <= 50; ++i)
		{
			float p = float(i) / 50.0f;
			float r = p * TAU;
			float x = Cos(r);
			float z = Sin(r);
			line->AddPoint(V3(x, 0, z) * 3.0f);
		}

		line->isLocalSpace = true;
		line->alignment = LineRenderer::Alignment::Local;
	}

	//*
	{
		thread t0(
			[&]
			{
				system->resource->factory->LoadMeshM(TEXT("../Resource/Jack/Jack.FBX"));
				system->resource->factory->LoadMeshM(TEXT("../Resource/Weapon/Katana/Katana.FBX"));
				system->resource->factory->LoadMeshM(TEXT("../Resource/Weapon/Katana/KatanaSheath.FBX"));
				system->resource->factory->LoadMeshM(TEXT("../Resource/BeetleDrone/BeetleDrone.FBX"));
			});
		if (t0.joinable())
			t0.join();

		{
			GameObject* goEnemy = CreateGameObject();
			goEnemy->transform->position = V3(0, 5, 5);
			goEnemy->AddComponent<EnemyBeetleDrone>();
		}

		{
			GameObject* go = CreateGameObject();
			go->transform->position = V3(0, 5, 0);
			go->AddComponent<Player>();
		}

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
			meshRenderer->mesh = system->resource->builtIn->boxMesh;
			meshRenderer->material->SetTexture("_AlbedoTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));
			meshRenderer->material->SetTexture("_ReflectionTexture", system->resource->builtIn->whiteTexture);
		}

		{
			GameObject* goDirectionalLight = CreateGameObject(TEXT("DirectionalLight"));
			goDirectionalLight->transform->position = V3(9.0f, 8.0f, -0.1f);
			goDirectionalLight->transform->forward = -V3(9.0f, 8.0f, -0.1f).normalized;
			DirectionalLight* directionalLight = goDirectionalLight->AddComponent<DirectionalLight>();
			directionalLight->enable = true;
			directionalLight->drawShadow = true;
			directionalLight->diffuse = Color(1, 1, 1, 1);
			directionalLight->ambient = Color(0.1f, 0.1f, 0.1f, 1.0f);
		}

		{
			GameObject* goPointLight = CreateGameObject(TEXT("PointLight"));
			goPointLight->transform->position = V3(0, 5, 0);
			PointLight* pointLight = goPointLight->AddComponent<PointLight>();
			pointLight->range = 10.0f;
			pointLight->drawShadow = true;
			pointLight->enable = true;
			pointLight->intensity = 0;
		}

		{
			GameObject* goSpotLight = CreateGameObject(TEXT("SpotLight"));
			goSpotLight->transform->position = V3(0, 2, -20);
			goSpotLight->transform->forward = V3::forward();
			SpotLight* spotLight = goSpotLight->AddComponent<SpotLight>();
			spotLight->range = 20.0f;
			spotLight->angle = 10.0f;
			spotLight->drawShadow = true;
			spotLight->enable = true;
			spotLight->intensity = 0;
		}

		{
			GameObject* box = CreateGameObject();
			box->transform->eulerAngles = V3(-20, 0, 10);
			box->transform->localScale = V3::one() * 5;
			box->transform->position = V3(0, 0, -5);

			Rigidbody* rigidbody = box->AddComponent<Rigidbody>();
			rigidbody->kinematic = true;
			BoxCollider* boxCollider = box->AddComponent<BoxCollider>();
			boxCollider->friction = 100.0f;

			MeshRenderer* meshRenderer = box->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->boxMesh;
			meshRenderer->material->SetTexture("_AlbedoTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));
		}

		{
			GameObject* box = CreateGameObject();
			box->transform->eulerAngles = V3(0, 0, 0);
			box->transform->localScale = V3::one() * 5;
			box->transform->position = V3(20, 0, -5);

			Rigidbody* rigidbody = box->AddComponent<Rigidbody>();
			rigidbody->kinematic = true;
			BoxCollider* boxCollider = box->AddComponent<BoxCollider>();
			boxCollider->friction = 100.0f;

			MeshRenderer* meshRenderer = box->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->boxMesh;
			meshRenderer->material->SetTexture("_AlbedoTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));
		}

		{
			GameObject* box = CreateGameObject();
			box->transform->eulerAngles = V3(45, 0, 0);
			box->transform->localScale = V3::one() * 5;
			box->transform->position = V3(-20, -1.0f, -5);

			Rigidbody* rigidbody = box->AddComponent<Rigidbody>();
			rigidbody->kinematic = true;
			BoxCollider* boxCollider = box->AddComponent<BoxCollider>();
			boxCollider->friction = 100.0f;

			MeshRenderer* meshRenderer = box->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->boxMesh;
			meshRenderer->material->SetTexture("_AlbedoTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));
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

		//for (uint i = 0; i < 1000; ++i)
		//{
		//	int range = 100;
		//	int range2 = range * 2;

		//	int x = (rand() % range2) - range;
		//	int z = (rand() % range2) - range;

		//	float r = float(rand() % 255) / 255.0f;
		//	float g = float(rand() % 255) / 255.0f;
		//	float b = float(rand() % 255) / 255.0f;

		//	GameObject* goPointLight = CreateGameObject();
		//	goPointLight->transform->position = V3(x, 5, z);
		//	PointLight* pointLight = goPointLight->AddComponent<PointLight>();
		//	pointLight->range = 10.0f;
		//	pointLight->drawShadow = false;
		//	pointLight->enable = true;
		//	pointLight->diffuse = Color(r, g, b, 1.0f);
		//}

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
		//	texture[0] = system->resourceManagement->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Skybox/top.png"));
		//	texture[1] = system->resourceManagement->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Skybox/bottom.png"));
		//	texture[2] = system->resourceManagement->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Skybox/left.png"));
		//	texture[3] = system->resourceManagement->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Skybox/right.png"));
		//	texture[4] = system->resourceManagement->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Skybox/forward.png"));
		//	texture[5] = system->resourceManagement->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Skybox/back.png"));

		//	ResourceRef<Shader> priority = system->resourceManagement->factory->CreateShaderFromBinaryFolderM(TEXT("Priority.cso"));
		//	ResourceRef<Material> material[6];
		//	for (uint i = 0; i < 6; ++i)
		//	{
		//		material[i] = system->resourceManagement->factory->CreateMaterialByShaderUM(priority);
		//		material[i]->SetTexture("_Texture", texture[i]);
		//	}

		//	GameObject* goSky[6];
		//	for (uint i = 0; i < 6; ++i)
		//	{
		//		goSky[i] = CreateGameObject();
		//		MeshRenderer* mr = goSky[i]->AddComponent<MeshRenderer>();
		//		mr->mesh = system->resourceManagement->builtIn->quadMesh;
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

		//{
		//	ResourceRef<Texture> texture = system->resource->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Skybox/skybox.png"));

		//	//ResourceRef<Shader> skybox = system->resource->factory->CreateManagedShaderFromFile(TEXT("../Shader/Skybox.fx"));
		//	ResourceRef<Shader> skybox = system->resource->factory->LoadShaderFromBinaryFolderM(TEXT("Skybox.cso"));
		//	ResourceRef<Material> material;
		//	material = system->resource->factory->CreateMaterialByShaderUM(skybox);
		//	material->SetTexture("_Texture", texture);

		//	GameObject* goSky;
		//	goSky = CreateGameObject();
		//	MeshRenderer* mr = goSky->AddComponent<MeshRenderer>();
		//	mr->mesh = system->resource->builtIn->skyboxMesh;
		//	mr->material = material;
		//}

		{
			GameObject* goGround = CreateGameObject();
			goGround->transform->position = V3(0.0f, 0.5f, 0.0f);
			goGround->transform->localScale = V3(200, 1, 200);

			MeshRenderer* meshRenderer = goGround->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->boxMesh;

			ResourceRef<Material> standardMaterial = system->resource->factory->CopyMaterialUM(system->resource->builtIn->standardMaterial);
			standardMaterial->SetTexture("_AlbedoTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));
			standardMaterial->SetTexture("_ReflectionTexture", system->resource->builtIn->whiteTexture);

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
			meshRenderer->mesh = system->resource->builtIn->boxMesh;
			meshRenderer->material->SetTexture("_AlbedoTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));

			goBox->AddComponent<Rigidbody>();
			goBox->AddComponent<BoxCollider>();
		}

		{
			GameObject* goBox = CreateGameObject(TEXT("Box"));
			goBox->transform->position = V3(6.0f, 0.0f, 0.0f);
			goBox->transform->localScale = V3(6, 6, 6);
			goBox->transform->eulerAngles = V3(50, 0, 0);

			MeshRenderer* meshRenderer = goBox->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->boxMesh;
			meshRenderer->material->SetTexture("_AlbedoTexture", system->resource->Find(TEXT("../Resource/Dev/Dev.png")));

			goBox->AddComponent<Rigidbody>()->kinematic = true;
			goBox->AddComponent<BoxCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(2.0f, 1.5f, 0.0f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		//{
		//	m_sphere[0] = CreateGameObject(TEXT("Sphere"));

		//	MeshRenderer* meshRenderer = m_sphere[0]->AddComponent<MeshRenderer>();
		//	meshRenderer->mesh = system->resource->builtIn->sphereMesh;

		//	m_katanaTrailRenderer[0] = m_sphere[0]->AddComponent<TrailRenderer>();

		//	m_rigidbody = m_sphere[0]->AddComponent<Rigidbody>();
		//	m_rigidbody->sleepThresholder = 1000;
		//	m_sphere[0]->AddComponent<SphereCollider>();

		//	ResourceRef<Shader> shader = system->resource->factory->LoadShaderFromBinaryFolderM(TEXT("Trail.cso"));
		//	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
		//	material->SetTexture("_NormalMapTexture", system->resource->Find(TEXT("../Resource/Dev/Normal.png")));

		//	m_katanaTrailRenderer[0]->material = material;

		//	m_sphere[0]->activeSelf = true;
		//}

		//{
		//	m_sphere[1] = CreateGameObject(TEXT("Sphere"));

		//	MeshRenderer* meshRenderer = m_sphere[1]->AddComponent<MeshRenderer>();
		//	meshRenderer->mesh = system->resource->builtIn->sphereMesh;

		//	m_katanaTrailRenderer[1] = m_sphere[1]->AddComponent<TrailRenderer>();

		//	m_rigidbody = m_sphere[1]->AddComponent<Rigidbody>();
		//	m_rigidbody->sleepThresholder = 1000;
		//	m_sphere[1]->AddComponent<SphereCollider>();

		//	ResourceRef<Shader> shader = system->resource->factory->LoadShaderFromBinaryFolderM(TEXT("Trail.cso"));
		//	ResourceRef<Material> material = system->resource->factory->CreateMaterialByShaderUM(shader);
		//	material->SetTexture("_NormalMapTexture", system->resource->Find(TEXT("../Resource/Dev/Normal.png")));

		//	m_katanaTrailRenderer[1]->material = system->resource->Find(TEXT("TrailMaterial"));

		//	m_sphere[1]->activeSelf = true;
		//}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(-4.5f, 1.5f, +4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(+4.5f, 1.5f, +4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(+4.5f, 1.5f, -4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->sphereMesh;

			goSphere->AddComponent<Rigidbody>();
			goSphere->AddComponent<SphereCollider>();
		}

		{
			GameObject* goSphere = CreateGameObject(TEXT("Sphere"));
			goSphere->transform->position = V3(-4.5f, 1.5f, -4.5f);

			MeshRenderer* meshRenderer = goSphere->AddComponent<MeshRenderer>();
			meshRenderer->mesh = system->resource->builtIn->sphereMesh;

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
	}

	if (m_sphere[1])
	{
		V3 spin = V3(cos(system->time->accumulatedSinceStartup), 0.0f, sin(system->time->accumulatedSinceStartup));
		m_sphere[1]->transform->position = V3::up() * 2.0f + spin * 6.0f + V3(0, 0, -8.0f);
	}

	{
		ImGui::Begin("Info", 0, ImGuiWindowFlags_AlwaysAutoResize);

		tstring resolutionTxt = tstring_format(TEXT("resolution: %d x %d"), int(system->graphic->GetWidth()), int(system->graphic->GetHeight()));
		ImGui::Text(tstring_to_str_utf8(resolutionTxt).c_str());

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
#ifdef _DEBUG
		ImGui::Text("Debug Mode");
#else
		ImGui::Text("Release Mode");
#endif

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

		float resolutionScale = ssrDesc.ResolutionScale;
		ImGui::SliderFloat("ResolutionScale", &resolutionScale, 0.0f, 1.0f);
		ssrDesc.ResolutionScale = resolutionScale;

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

		bool drawVolumetric = light->volumetricLightState;
		ImGui::Checkbox("Draw Volumetric", &drawVolumetric);
		light->volumetricLightState = drawVolumetric;

		int volumetricLightNumSamples = light->volumetricLightNumSamples;
		ImGui::SliderInt("Volumetric Num Samples", &volumetricLightNumSamples, 0, 200);
		light->volumetricLightNumSamples = volumetricLightNumSamples;

		float volumetricLightIntensity = light->volumetricLightIntensity;
		ImGui::SliderFloat("Volumetric Intensity", &volumetricLightIntensity, 0.0f, 5.0f);
		light->volumetricLightIntensity = volumetricLightIntensity;

		float volumetricLightPower = light->volumetricLightPower;
		ImGui::SliderFloat("Volumetric Power", &volumetricLightPower, 0.0f, 5.0f);
		light->volumetricLightPower = volumetricLightPower;

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

		bool drawVolumetric = light->volumetricLightState;
		ImGui::Checkbox("Draw Volumetric", &drawVolumetric);
		light->volumetricLightState = drawVolumetric;

		int volumetricLightNumSamples = light->volumetricLightNumSamples;
		ImGui::SliderInt("Volumetric Num Samples", &volumetricLightNumSamples, 0, 200);
		light->volumetricLightNumSamples = volumetricLightNumSamples;

		float volumetricLightIntensity = light->volumetricLightIntensity;
		ImGui::SliderFloat("Volumetric Intensity", &volumetricLightIntensity, 0.0f, 5.0f);
		light->volumetricLightIntensity = volumetricLightIntensity;

		float volumetricLightPower = light->volumetricLightPower;
		ImGui::SliderFloat("Volumetric Power", &volumetricLightPower, 0.0f, 5.0f);
		light->volumetricLightPower = volumetricLightPower;

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
