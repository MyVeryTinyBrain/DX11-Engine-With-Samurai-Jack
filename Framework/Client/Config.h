#pragma once
#ifndef __GAME__CONFIG__
#define __GAME__CONFIG__

enum
{
	PhysicsLayer_Default = 0,
	PhysicsLayer_Player = 1,
	PhysicsLayer_Enemy = 2,
	PhysicsLayer_Particle = 7,
};

inline void INIT_PHYSICS_LAYER(PhysicsLayerManager* layerManager)
{
	layerManager->SetCollision(PhysicsLayer_Default, PhysicsLayer_Player, true);
	layerManager->SetCollision(PhysicsLayer_Default, PhysicsLayer_Enemy, true);
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

#define MESH_JACK							TEXT("../Resource/Jack/Jack.FBX")
#define MESH_KATANA							TEXT("../Resource/Jack/Katana.FBX")
#define MESH_KATANA_SHEATH					TEXT("../Resource/Jack/KatanaSheath.FBX")
#define MESH_BEETLE_DRONE					TEXT("../Resource/BeetleDrone/BeetleDrone.FBX")
#define MESH_ANCIENT_KING					TEXT("../Resource/AncientKing/AncientKing.FBX")
#define MESH_ANCIENT_KING_HAMMER			TEXT("../Resource/AncientKing/AncientKingHammer.FBX")
#define MESH_ASHI							TEXT("../Resource/Ashi/Ashi.FBX")
#define MESH_ASHI_SWORD						TEXT("../Resource/Ashi/AshiSword.FBX")
#define MESH_CYILNDER_THIN_TWOSIDES			TEXT("../Resource/Cylinder_Thin_Twosides/Cylinder_Thin_Twosides.FBX")
#define MESH_WAVE							TEXT("../Resource/Wave/Wave.FBX")
#define MESH_DOME							TEXT("../Resource/Dome/Dome.FBX")
#define MESH_ELECTRIC_01					TEXT("../Resource/Electric/electric01.fbx")
#define MESH_ELECTRIC_02					TEXT("../Resource/Electric/electric02.fbx")
#define MESH_ELECTRIC_03					TEXT("../Resource/Electric/electric03.fbx")
#define MESH_ELECTRIC_04					TEXT("../Resource/Electric/electric04.fbx")
#define MESH_ELECTRIC_05					TEXT("../Resource/Electric/electric05.fbx")
#define MESH_ELECTRIC_06					TEXT("../Resource/Electric/electric06.fbx")
#define MESH_ELECTRIC_07					TEXT("../Resource/Electric/electric07.fbx")
#define MESH_ENV_CAVE_BOSSFLOOR				TEXT("../Resource/Environment/Cave/BossFloor.fbx")
#define MESH_ENV_CAVE_ROCK_CYLINDERL02		TEXT("../Resource/Environment/Cave/RockCylinderL02.fbx")
#define MESH_ENV_CAVE_ROCK_CYLINDERM02		TEXT("../Resource/Environment/Cave/RockCylinderM02.fbx")
#define MESH_ENV_CAVE_SHUTTER_DOORFLOOR		TEXT("../Resource/Environment/Cave/ShutterDoor_Floor.fbx")
#define MESH_ENV_CAVE_TUNNEL_CEILING		TEXT("../Resource/Environment/Cave/Tunnel_Ceiling.fbx")
#define MESH_ENV_CAVE_TUNNEL_DIVIDER		TEXT("../Resource/Environment/Cave/Tunnel_Divider.fbx")
#define MESH_ENV_CAVE_TUNNEL_DOME			TEXT("../Resource/Environment/Cave/Tunnel_Dome.fbx")
#define MESH_ENV_CAVE_TUNNEL_FLOOR			TEXT("../Resource/Environment/Cave/Tunnel_Floor.fbx")
#define MESH_ENV_CAVE_CLIFF_L				TEXT("../Resource/Environment/Cave/CaveCliffL.fbx")
#define MESH_ENV_CAVE_CLIFF_M				TEXT("../Resource/Environment/Cave/CaveCliffM.fbx")
#define MESH_ENV_CAVE_CLIIF_S				TEXT("../Resource/Environment/Cave/CaveCliffS.fbx")
#define MESH_ENV_CAVE_FLOOR_L				TEXT("../Resource/Environment/Cave/CaveFloorL.fbx")
#define MESH_ENV_CAVE_WALLDOORFRAME			TEXT("../Resource/Environment/Cave/CaveWallDoorFrame.fbx")
#define MESH_ENV_CAVE_WALL_L				TEXT("../Resource/Environment/Cave/CaveWallL.fbx")
#define MESH_ENV_CAVE_WALL_L_ENTRANCE		TEXT("../Resource/Environment/Cave/CaveWallLEntrance.fbx")
#define MESH_ENV_CAVE_WALL_M				TEXT("../Resource/Environment/Cave/CaveWallM.fbx")
#define MESH_ENV_CAVE_WALL_PILLAR			TEXT("../Resource/Environment/Cave/CaveWallPillar.fbx")
#define MESH_ENV_CAVE_WALL_S				TEXT("../Resource/Environment/Cave/CaveWallS.fbx")
#define MESH_ENV_CAVE_WALL_S_ENTRANCE		TEXT("../Resource/Environment/Cave/CaveWallSEntrance.fbx")
#define MESH_ENV_CAVE_TRAPWALL				TEXT("../Resource/Environment/Cave/TrapWall.fbx")
#define MESH_ENV_CAVE_FLOOR_DMG01			TEXT("../Resource/Environment/Cave/FloorDmg01.fbx")
#define MESH_ENV_CAVE_FLOOR_DMG02			TEXT("../Resource/Environment/Cave/FloorDmg02.fbx")
#define MESH_ENV_CAVE_FLOOR_DMG03			TEXT("../Resource/Environment/Cave/FloorDmg03.fbx")
#define MESH_ENV_CAVE_FLOOR_DMG05			TEXT("../Resource/Environment/Cave/FloorDmg05.fbx")
#define MESH_ENV_CAVE_FLOOR_DMG06			TEXT("../Resource/Environment/Cave/FloorDmg06.fbx")
#define MESH_ENV_CAVE_FLOOR_DMG_CORNER01	TEXT("../Resource/Environment/Cave/FloorDmgCorner01.fbx")
#define MESH_ENV_CAVE_FLOOR_DMG_CORNER02	TEXT("../Resource/Environment/Cave/FloorDmgCorner02.fbx")
#define MESH_ENV_CAVE_FLOOR_DOME			TEXT("../Resource/Environment/Cave/FloorDome.fbx")
#define MESH_ENV_CAVE_FLOOR_SLOPE01			TEXT("../Resource/Environment/Cave/FloorSlop01.fbx")
#define MESH_ENV_CAVE_FLOOR_SLOPE02			TEXT("../Resource/Environment/Cave/FloorSlop02.fbx")
#define MESH_ENV_CAVE_PILLAR01				TEXT("../Resource/Environment/Cave/Pillar01.fbx")
#define MESH_ENV_CAVE_PILLAR02				TEXT("../Resource/Environment/Cave/Pillar02.fbx")
#define MESH_ENV_CAVE_ROCK01				TEXT("../Resource/Environment/Cave/Rock01.fbx")
#define MESH_ENV_CAVE_ROCK02				TEXT("../Resource/Environment/Cave/Rock02.fbx")
#define MESH_ENV_CAVE_ROCK03				TEXT("../Resource/Environment/Cave/Rock03.fbx")
#define MESH_ENV_CAVE_ROCK04				TEXT("../Resource/Environment/Cave/Rock04.fbx")
#define MESH_ENV_CAVE_ROCK05				TEXT("../Resource/Environment/Cave/Rock05.fbx")
#define MESH_ENV_CAVE_ROCK06				TEXT("../Resource/Environment/Cave/Rock06.fbx")
#define MESH_ENV_CAVE_ROCK07				TEXT("../Resource/Environment/Cave/Rock07.fbx")
#define MESH_ENV_CAVE_ROCK08				TEXT("../Resource/Environment/Cave/Rock08.fbx")
#define MESH_ENV_CAVE_CEILING				TEXT("../Resource/Environment/Cave/CaveCeiling.fbx")
#define MESH_ENV_CAVE_VENT					TEXT("../Resource/Environment/Cave/Vent.fbx")
#define MESH_ENV_TRAPFLOORL					TEXT("../Resource/Environment/Cave/TrapFloorL.fbx")
#define MESH_ENV_TRAPFLOORM01				TEXT("../Resource/Environment/Cave/TrapFloorM01.fbx")
#define MESH_ENV_TRAPFLOORM02				TEXT("../Resource/Environment/Cave/TrapFloorM02.fbx")
#define MESH_ENV_TRAPFLOORM03				TEXT("../Resource/Environment/Cave/TrapFloorM03.fbx")
#define MESH_ENV_TRAPFLOORS01				TEXT("../Resource/Environment/Cave/TrapFloorS01.fbx")
#define MESH_ENV_TRAPFLOORS02				TEXT("../Resource/Environment/Cave/TrapFloorS02.fbx")
#define MESH_ENV_TRAPFLOORS03				TEXT("../Resource/Environment/Cave/TrapFloorS03.fbx")
#define MESH_ENV_TRAPFLOORS04				TEXT("../Resource/Environment/Cave/TrapFloorS04.fbx")
#define MESH_ENV_TRAPFLOORS05				TEXT("../Resource/Environment/Cave/TrapFloorS05.fbx")
#define MESH_ENV_TRAPFLOORS06				TEXT("../Resource/Environment/Cave/TrapFloorS06.fbx")
#define MESH_ENV_CRYSTAL					TEXT("../Resource/Environment/Crystal/Crystal.fbx")

// Textures

#define TEX_GRADIENT_TO_RIGHT				TEXT("../Resource/Gradient/ToRight.dds")
#define TEX_GRADIENT_CENTER					TEXT("../Resource/Gradient/Center.dds")
#define TEX_EFFECT_SPARK_DOT				TEXT("../Resource/Effect/Spark_Dot.dds")
#define TEX_EFFECT_SPARK_SPREAD				TEXT("../Resource/Effect/Spark_Spread.dds")
#define TEX_EFFECT_ELECTRIC_H				TEXT("../Resource/Effect/Electric_H.dds")
#define TEX_EFFECT_ELECTRIC_V				TEXT("../Resource/Effect/Electric_V.dds")
#define TEX_EFFECT_FLASH					TEXT("../Resource/Effect/T_E_Flash_05.dds")
#define TEX_EFFECT_RING01					TEXT("../Resource/Effect/T_E_Ring_01.dds")
#define TEX_EFFECT_RING02					TEXT("../Resource/Effect/T_E_Ring_02.dds")
#define TEX_EFFECT_RING_DISTORTION			TEXT("../Resource/Effect/T_E_Ring_04.dds")
#define TEX_EFFECT_GLOW						TEXT("../Resource/Effect/Glow.dds")
#define TEX_NOISE_01						TEXT("../Resource/Noise/Noise01.dds")
#define TEX_NOISE_02						TEXT("../Resource/Noise/Noise02.dds")
#define TEX_NOISE_03						TEXT("../Resource/Noise/Noise03.dds")
#define TEX_DUST_PARTICLE_01				TEXT("../Resource/Dust/DustParticle01.dds")
#define TEX_DUST_PARTICLE_02				TEXT("../Resource/Dust/DustParticle02.dds")
#define TEX_DUST_PARTICLE_03				TEXT("../Resource/Dust/DustParticle03.dds")
#define TEX_DUST_PARTICLE_04				TEXT("../Resource/Dust/DustParticle04.dds")
#define TEX_FIRE_01							TEXT("../Resource/Fire/Fire01.dds")
#define TEX_FIRE_02							TEXT("../Resource/Fire/Fire02.dds")
#define TEX_FIRE_03							TEXT("../Resource/Fire/Fire03.dds")
#define TEX_FLAME_01						TEXT("../Resource/Fire/Flame01.dds")

// Sounds

#define SOUND_GUARD_01						TEXT("../Resource/Sound/Guard/Guard01.wav")
#define SOUND_GUARD_02						TEXT("../Resource/Sound/Guard/Guard02.wav")
#define SOUND_KATANA_SWING_LIGHT01			TEXT("../Resource/Sound/KatanaSwing/Light01.wav")
#define SOUND_KATANA_SWING_LIGHT02			TEXT("../Resource/Sound/KatanaSwing/Light02.wav")
#define SOUND_KATANA_SWING_LIGHT03			TEXT("../Resource/Sound/KatanaSwing/Light03.wav")
#define SOUND_KATANA_SWING_HEAVY01			TEXT("../Resource/Sound/KatanaSwing/Heavy01.wav")
#define SOUND_KATANA_SWING_HEAVY02			TEXT("../Resource/Sound/KatanaSwing/Heavy02.wav")
#define SOUND_KATANA_SWING_HEAVY03			TEXT("../Resource/Sound/KatanaSwing/Heavy03.wav")
#define SOUND_KATANA_SWING_HEAVY04			TEXT("../Resource/Sound/KatanaSwing/Heavy04.wav")
#define SOUND_MELEE_SWING_01				TEXT("../Resource/Sound/MeleeSwing/MeleeSwing01.wav")
#define SOUND_BEHAVIOR_FOOTSTEP_01			TEXT("../Resource/Sound/Behavior/Footstep01.wav")
#define SOUND_BEHAVIOR_FOOTSTEP_02			TEXT("../Resource/Sound/Behavior/Footstep02.wav")
#define SOUND_BEHAVIOR_FOOTSTEP_03			TEXT("../Resource/Sound/Behavior/Footstep03.wav")
#define SOUND_BEHAVIOR_FOOTSTEP_04			TEXT("../Resource/Sound/Behavior/Footstep04.wav")
#define SOUND_BEHAVIOR_ROLL					TEXT("../Resource/Sound/Behavior/Roll.wav")
#define SOUND_BEHAVIOR_ARMOR				TEXT("../Resource/Sound/Behavior/Armor.wav")
#define SOUND_CRASH_01						TEXT("../Resource/Sound/Crash/Crash01.wav")
#define SOUND_CRASH_02						TEXT("../Resource/Sound/Crash/Crash02.wav")
#define SOUND_BIG_SWING_01					TEXT("../Resource/Sound/BigSwing/BigSwing01.wav")
#define SOUND_DAMAGE_01						TEXT("../Resource/Sound/Damage/Damage01.wav")
#define SOUND_DAMAGE_02						TEXT("../Resource/Sound/Damage/Damage02.wav")
#define SOUND_DAMAGE_03						TEXT("../Resource/Sound/Damage/Damage03.wav")
#define SOUND_ELECTRIC_01					TEXT("../Resource/Sound/Electric/Electric01.wav")
#define SOUND_ELECTRIC_02					TEXT("../Resource/Sound/Electric/Electric02.wav")
#define SOUND_FLAME_01						TEXT("../Resource/Sound/Flame/Flame01.wav")
#define SOUND_SWING_01						TEXT("../Resource/Sound/Swing/Swing01.wav")
#define SOUND_SWING_02						TEXT("../Resource/Sound/Swing/Swing02.wav")

// Shaders

#define SHADER_BILLBOARD_EFFECT				TEXT("BillboardEffect.cso")
#define SHADER_BILLBOARD_SPRITESHEET		TEXT("BillboardSpritesheet.cso")
#define SHADER_INSTANCE_UNLIT				TEXT("InstanceUnlit.cso")
#define SHADER_TRAIL						TEXT("Trail.cso")
#define SHADER_SHOCKWAVE					TEXT("Shockwave.cso")
#define SHADER_WAVEBEAM						TEXT("WaveBeam.cso")
#define SHADER_RING							TEXT("Ring.cso")
#define SHADER_SPARK						TEXT("Spark.cso")
#define SHADER_LAVA							TEXT("Lava.cso")
#define SHADER_FIRE							TEXT("Fire.cso")

// Event

#define EVENT_ENEMY_DIE						"EVENT_ENEMY_DIE"
#define EVENT_FIGHT_START					"EVENT_FIGHT_START"
#define EVENT_FIGHT_END						"EVENT_FIGHT_END"

// ADJUST

#define ADJUST_LOCALEULERANGLES				V3(90, 180, 0)
#define ADJUST_PLAYER_LOCALPOSITION			V3(0, -1, 0)
#define ADJUST_BEETLEDRONE_LOCALPOSITION	V3(0, -1, 0)
#define ADJUST_ANCIENTKING_LOCALPOSITION	V3(0, -3.05f, 0)
#define ADJUST_ANCIENTKING_LOCALSCALE		(V3::one() * 1.5f)
#define ADJUST_ASHI_LOCALPOSITION			V3(0, -1.6f, 0)
#define ADJUST_ASHI_LOCALSCALE				(V3::one() * 1.5f)

// TAG

#define TAG_FIGHT_COLLIDER					TEXT("FightCollider")
#define TAG_NEXTSCENE_TRIGGER				TEXT("NextSceneTrigger")

inline void LOAD_MESHES(System* system)
{
	system->resource->factory->LoadMeshM(MESH_JACK);
	system->resource->factory->LoadMeshM(MESH_KATANA);
	system->resource->factory->LoadMeshM(MESH_KATANA_SHEATH);
	system->resource->factory->LoadMeshM(MESH_BEETLE_DRONE);
	system->resource->factory->LoadMeshM(MESH_ANCIENT_KING);
	system->resource->factory->LoadMeshM(MESH_ANCIENT_KING_HAMMER);
	system->resource->factory->LoadMeshM(MESH_ASHI);
	system->resource->factory->LoadMeshM(MESH_ASHI_SWORD);
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
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_BOSSFLOOR);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK_CYLINDERL02);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK_CYLINDERM02);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_SHUTTER_DOORFLOOR);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_TUNNEL_CEILING);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_TUNNEL_DIVIDER);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_TUNNEL_DOME);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_TUNNEL_FLOOR);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_CLIFF_L);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_CLIFF_M);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_CLIIF_S);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_L);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_WALLDOORFRAME);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_WALL_L);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_WALL_L_ENTRANCE);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_WALL_M);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_WALL_PILLAR);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_WALL_S);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_WALL_S_ENTRANCE);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_TRAPWALL);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_DMG01);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_DMG02);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_DMG03);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_DMG05);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_DMG06);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_DMG_CORNER01);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_DMG_CORNER02);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_DOME);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_SLOPE01);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_FLOOR_SLOPE02);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_PILLAR01);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_PILLAR02);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK01);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK02);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK03);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK04);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK05);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK06);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK07);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_ROCK08);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_CEILING);
	system->resource->factory->LoadMeshM(MESH_ENV_CAVE_VENT);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORL);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORM01);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORM02);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORM03);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORS01);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORS02);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORS03);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORS04);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORS05);
	system->resource->factory->LoadMeshM(MESH_ENV_TRAPFLOORS06);
	system->resource->factory->LoadMeshM(MESH_ENV_CRYSTAL);
}

inline void LOAD_SOUNDS(System* system)
{
	system->resource->factory->LoadAudioClipM(SOUND_GUARD_01);
	system->resource->factory->LoadAudioClipM(SOUND_GUARD_02);
	system->resource->factory->LoadAudioClipM(SOUND_KATANA_SWING_LIGHT01);
	system->resource->factory->LoadAudioClipM(SOUND_KATANA_SWING_LIGHT02);
	system->resource->factory->LoadAudioClipM(SOUND_KATANA_SWING_LIGHT03);
	system->resource->factory->LoadAudioClipM(SOUND_KATANA_SWING_HEAVY01);
	system->resource->factory->LoadAudioClipM(SOUND_KATANA_SWING_HEAVY02);
	system->resource->factory->LoadAudioClipM(SOUND_KATANA_SWING_HEAVY03);
	system->resource->factory->LoadAudioClipM(SOUND_KATANA_SWING_HEAVY04);
	system->resource->factory->LoadAudioClipM(SOUND_MELEE_SWING_01);
	system->resource->factory->LoadAudioClipM(SOUND_BEHAVIOR_FOOTSTEP_01);
	system->resource->factory->LoadAudioClipM(SOUND_BEHAVIOR_FOOTSTEP_02);
	system->resource->factory->LoadAudioClipM(SOUND_BEHAVIOR_FOOTSTEP_03);
	system->resource->factory->LoadAudioClipM(SOUND_BEHAVIOR_FOOTSTEP_04);
	system->resource->factory->LoadAudioClipM(SOUND_BEHAVIOR_ROLL);
	system->resource->factory->LoadAudioClipM(SOUND_BEHAVIOR_ARMOR);
	system->resource->factory->LoadAudioClipM(SOUND_CRASH_01);
	system->resource->factory->LoadAudioClipM(SOUND_CRASH_02);
	system->resource->factory->LoadAudioClipM(SOUND_BIG_SWING_01);
	system->resource->factory->LoadAudioClipM(SOUND_DAMAGE_01);
	system->resource->factory->LoadAudioClipM(SOUND_DAMAGE_02);
	system->resource->factory->LoadAudioClipM(SOUND_DAMAGE_03);
	system->resource->factory->LoadAudioClipM(SOUND_ELECTRIC_01);
	system->resource->factory->LoadAudioClipM(SOUND_ELECTRIC_02);
	system->resource->factory->LoadAudioClipM(SOUND_FLAME_01);
	system->resource->factory->LoadAudioClipM(SOUND_SWING_01);
	system->resource->factory->LoadAudioClipM(SOUND_SWING_02);
}

inline void LOAD_CONFIGFILES(System* system)
{
	LOAD_MESHES(system);
	LOAD_SOUNDS(system);
	TextureOptionDesc loadDesc = {};
	TextureOptionDesc loadMipDesc = {};
	loadMipDesc.GenerateMipmap = true;
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_GRADIENT_TO_RIGHT);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_GRADIENT_CENTER);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_SPARK_DOT);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_SPARK_SPREAD);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_ELECTRIC_H);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_ELECTRIC_V);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_FLASH);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_RING01);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_RING02);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_RING_DISTORTION);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_EFFECT_GLOW);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_NOISE_01);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_NOISE_02);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_NOISE_03);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_DUST_PARTICLE_01);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_DUST_PARTICLE_02);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_DUST_PARTICLE_03);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_DUST_PARTICLE_04);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_FIRE_01);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_FIRE_02);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_FIRE_03);
	system->resource->factory->LoadTexture2DM(loadDesc, TEX_FLAME_01);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_BILLBOARD_EFFECT);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_BILLBOARD_SPRITESHEET);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_INSTANCE_UNLIT);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_TRAIL);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_SHOCKWAVE);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_WAVEBEAM);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_RING);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_SPARK);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_LAVA);
	system->resource->factory->LoadShaderFromBinaryFolderM(SHADER_FIRE);
}

#endif