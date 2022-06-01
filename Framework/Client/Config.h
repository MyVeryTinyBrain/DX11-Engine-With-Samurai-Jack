#pragma once
#ifndef __GAME__CONFIG__
#define __GAME__CONFIG__

enum
{
	PhysicsLayer_Default = 0,
	PhysicsLayer_Player = 1,
	PhysicsLayer_Enemy = 2,
	PhysicsLayer_VirtualEnemy = 3,
	PhysicsLayer_Particle = 7,
};

inline void INIT_PHYSICS_LAYER(PhysicsLayerManager* layerManager)
{
	layerManager->SetCollision(PhysicsLayer_Default, PhysicsLayer_Player, true);
	layerManager->SetCollision(PhysicsLayer_Default, PhysicsLayer_Enemy, true);
	layerManager->SetCollision(PhysicsLayer_Default, PhysicsLayer_VirtualEnemy, true);
	layerManager->SetCollision(PhysicsLayer_Default, PhysicsLayer_Particle, true);
	layerManager->SetCollision(PhysicsLayer_Player, PhysicsLayer_Enemy, true);
	layerManager->SetCollision(PhysicsLayer_Enemy, PhysicsLayer_Enemy, true);
}

#define JUMP_SPEED				(11.0f)
#define AIRJUMP_SPEED			(13.0f)
#define BLOWUP_SPEED			(17.0f)
#define BLOWDOWN_SPEED			(-40.0f)
#define BLOWUP_VELOCITY			V3(0, BLOWUP_SPEED, 0)
#define BLOWDOWN_VELOCITY		V3(0, BLOWDOWN_SPEED, 0)

// Animator defines

#define LOOP		true
#define NOLOOP		false
#define EXIT		nullptr
#define ANY			nullptr

// Meshes

#define MESH_JACK						TEXT("../Resource/Jack/Jack.FBX")
#define MESH_KATANA						TEXT("../Resource/Jack/Katana.FBX")
#define MESH_KATANA_SHEATH				TEXT("../Resource/Jack/KatanaSheath.FBX")
#define MESH_BEETLE_DRONE				TEXT("../Resource/BeetleDrone/BeetleDrone.FBX")
#define MESH_ANCIENT_KING				TEXT("../Resource/AncientKing/AncientKing.FBX")
#define MESH_ANCIENT_KING_HAMMER		TEXT("../Resource/AncientKing/AncientKingHammer.FBX")
#define MESH_CYILNDER_THIN_TWOSIDES		TEXT("../Resource/Cylinder_Thin_Twosides/Cylinder_Thin_Twosides.FBX")
#define MESH_WAVE						TEXT("../Resource/Wave/Wave.FBX")
#define MESH_DOME						TEXT("../Resource/Dome/Dome.FBX")
#define MESH_ELECTRIC_01				TEXT("../Resource/Electric/electric01.fbx")
#define MESH_ELECTRIC_02				TEXT("../Resource/Electric/electric02.fbx")
#define MESH_ELECTRIC_03				TEXT("../Resource/Electric/electric03.fbx")
#define MESH_ELECTRIC_04				TEXT("../Resource/Electric/electric04.fbx")
#define MESH_ELECTRIC_05				TEXT("../Resource/Electric/electric05.fbx")
#define MESH_ELECTRIC_06				TEXT("../Resource/Electric/electric06.fbx")
#define MESH_ELECTRIC_07				TEXT("../Resource/Electric/electric07.fbx")

// Textures

#define TEX_GRADIENT_TO_RIGHT			TEXT("../Resource/Gradient/ToRight.dds")
#define TEX_GRADIENT_CENTER				TEXT("../Resource/Gradient/Center.dds")
#define TEX_EFFECT_SPARK				TEXT("../Resource/Effect/Spark.dds")
#define TEX_EFFECT_ELECTRIC_H			TEXT("../Resource/Effect/Electric_H.dds")
#define TEX_EFFECT_ELECTRIC_V			TEXT("../Resource/Effect/Electric_V.dds")
#define TEX_EFFECT_FLASH				TEXT("../Resource/Effect/T_E_Flash_05.dds")
#define TEX_EFFECT_RING01				TEXT("../Resource/Effect/T_E_Ring_01.dds")
#define TEX_EFFECT_RING02				TEXT("../Resource/Effect/T_E_Ring_02.dds")
#define TEX_EFFECT_RING_DISTORTION		TEXT("../Resource/Effect/T_E_Ring_04.dds")
#define TEX_NOISE_0						TEXT("../Resource/Noise/Noise01.dds")

// Shaders

#define SHADER_TRAIL					TEXT("Trail.cso")
#define SHADER_BILLBOARD_EFFECT			TEXT("BillboardEffect.cso")
#define SHADER_BILLBOARD_ANIMATION		TEXT("BillboardAnimation.cso")
#define SHADER_SHOCKWAVE				TEXT("Shockwave.cso")
#define SHADER_WAVEBEAM					TEXT("WaveBeam.cso")
#define SHADER_RING						TEXT("Ring.cso")

inline void LOAD_CONFIGFILES(System* system)
{
	TextureOptionDesc loadDesc = {};
	TextureOptionDesc loadMipDesc = {};
	loadMipDesc.GenerateMipmap = true;
	system->resource->factory->LoadMeshM(MESH_JACK);
	system->resource->factory->LoadMeshM(MESH_KATANA);
	system->resource->factory->LoadMeshM(MESH_KATANA_SHEATH);
	system->resource->factory->LoadMeshM(MESH_BEETLE_DRONE);
	system->resource->factory->LoadMeshM(MESH_ANCIENT_KING);
	system->resource->factory->LoadMeshM(MESH_ANCIENT_KING_HAMMER);
	system->resource->factory->LoadMeshM(MESH_CYILNDER_THIN_TWOSIDES);
	system->resource->factory->LoadMeshM(MESH_WAVE);
	system->resource->factory->LoadMeshM(MESH_DOME);
	system->resource->factory->LoadMeshM(MESH_ELECTRIC_01);
	system->resource->factory->LoadMeshM(MESH_ELECTRIC_02);
	system->resource->factory->LoadMeshM(MESH_ELECTRIC_03);
	system->resource->factory->LoadMeshM(MESH_ELECTRIC_04);
	system->resource->factory->LoadMeshM(MESH_ELECTRIC_05);
	system->resource->factory->LoadMeshM(MESH_ELECTRIC_06);
	system->resource->factory->LoadMeshM(MESH_ELECTRIC_07);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_GRADIENT_TO_RIGHT);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_GRADIENT_CENTER);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_SPARK);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_ELECTRIC_H);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_ELECTRIC_V);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_FLASH);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_RING01);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_RING02);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_RING_DISTORTION);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_NOISE_0);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_TRAIL);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_BILLBOARD_EFFECT);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_BILLBOARD_ANIMATION);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_SHOCKWAVE);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_WAVEBEAM);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_RING);
}

#endif