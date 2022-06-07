#include "stdafx.h"
#include "PlayerTestScene.h"
#include "Player.h"
#include "FreeCamera.h"
#include "EnemyBeetleDrone.h"
#include "BossAncientKing.h"
#include "Config.h"
#include "BillboardAnimation.h"
#include "EffectShockwave.h"
#include "EventSystem.h"


Scene* PlayerTestScene::Clone()
{
	return new PlayerTestScene;
}

void PlayerTestScene::OnLoad()
{
	INIT_PHYSICS_LAYER(system->physics->layerManager);

	TextureOptionDesc loadDesc = {};
	TextureOptionDesc loadMipDesc = {};
	loadMipDesc.GenerateMipmap = true;

	system->resource->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Dev/Dev.png"));
	system->resource->factory->LoadTexture2DM(loadDesc, TEXT("../Resource/Dev/Normal.png"));

	GameObject* goEventSystem = CreateGameObject();
	goEventSystem->AddComponent<EventSystem>();

	thread t0(
		[&]
		{
			LOAD_CONFIGFILES(system);
		});
	if (t0.joinable())
		t0.join();

	{
		GameObject* goMesh = CreateGameObject();
		goMesh->transform->position = V3(0, 10, 0);
		goMesh->transform->eulerAngles = V3(90, 0, 0);
		MeshRenderer* meshRenderer = goMesh->AddComponent<MeshRenderer>();
		meshRenderer->mesh = system->resource->Find(MESH_ENV_CAVE_ROCK_CYLINDERM02);
		meshRenderer->SetupStandardMaterials();

		goMesh->AddComponent<Rigidbody>()->kinematic = true;
		goMesh->AddComponent<ConvexCollider>();

		GameObject* goGizmo = CreateGameObject();
		goGizmo->transform->position = goMesh->transform->position;
		Gizmo* gizmo = goGizmo->AddComponent<Gizmo>();
		gizmo->show = true;
		gizmo->SetHandlingTransform(goMesh->transform);
	}

	{
		GameObject* go = CreateGameObject();
		go->transform->position = V3(0, 5, 0);
		go->AddComponent<Player>();
	}

	{
		GameObject* goGround = CreateGameObject(TEXT("Ground"));
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
		GameObject* goWall = CreateGameObject(TEXT("Wall"));
		goWall->transform->position = V3(50, 4.5f, 0);
		goWall->transform->localScale = V3(1, 10, 100);

		Rigidbody* rigidbody = goWall->AddComponent<Rigidbody>();
		rigidbody->kinematic = true;
		BoxCollider* boxCollider = goWall->AddComponent<BoxCollider>();
		boxCollider->friction = 1.0f;
		boxCollider->restitution = 1.0f;

		MeshRenderer* meshRenderer = goWall->AddComponent<MeshRenderer>();
		meshRenderer->mesh = system->resource->builtIn->boxMesh;
		meshRenderer->SetupStandardMaterials();
	}
	{
		GameObject* goWall = CreateGameObject(TEXT("Wall"));
		goWall->transform->position = V3(-50, 4.5f, 0);
		goWall->transform->localScale = V3(1, 10, 100);

		Rigidbody* rigidbody = goWall->AddComponent<Rigidbody>();
		rigidbody->kinematic = true;
		BoxCollider* boxCollider = goWall->AddComponent<BoxCollider>();
		boxCollider->friction = 1.0f;
		boxCollider->restitution = 1.0f;

		MeshRenderer* meshRenderer = goWall->AddComponent<MeshRenderer>();
		meshRenderer->mesh = system->resource->builtIn->boxMesh;
		meshRenderer->SetupStandardMaterials();
	}
	{
		GameObject* goWall = CreateGameObject(TEXT("Wall"));
		goWall->transform->position = V3(0, 4.5f, 50);
		goWall->transform->localScale = V3(100, 10, 1);

		Rigidbody* rigidbody = goWall->AddComponent<Rigidbody>();
		rigidbody->kinematic = true;
		BoxCollider* boxCollider = goWall->AddComponent<BoxCollider>();
		boxCollider->friction = 1.0f;
		boxCollider->restitution = 1.0f;

		MeshRenderer* meshRenderer = goWall->AddComponent<MeshRenderer>();
		meshRenderer->mesh = system->resource->builtIn->boxMesh;
		meshRenderer->SetupStandardMaterials();
	}
	{
		GameObject* goWall = CreateGameObject(TEXT("Wall"));
		goWall->transform->position = V3(0, 4.5f, -50);
		goWall->transform->localScale = V3(100, 10, 1);

		Rigidbody* rigidbody = goWall->AddComponent<Rigidbody>();
		rigidbody->kinematic = true;
		BoxCollider* boxCollider = goWall->AddComponent<BoxCollider>();
		boxCollider->friction = 1.0f;
		boxCollider->restitution = 1.0f;

		MeshRenderer* meshRenderer = goWall->AddComponent<MeshRenderer>();
		meshRenderer->mesh = system->resource->builtIn->boxMesh;
		meshRenderer->SetupStandardMaterials();
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
		GameObject* box = CreateGameObject(TEXT("Box"));
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
		GameObject* box = CreateGameObject(TEXT("Box"));
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
		GameObject* box = CreateGameObject(TEXT("Box"));
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

void PlayerTestScene::OnUnload()
{
}

void PlayerTestScene::OnUpdate()
{
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
