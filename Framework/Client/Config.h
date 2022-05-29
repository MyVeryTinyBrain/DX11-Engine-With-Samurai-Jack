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

// INT CONTEXT

// MAX BITS						0b00000000000000000000000000000000

#define ANIM_JUMP				0b00000000000000001

#define ANIM_ATK_KT_START		0b00000000000000010
#define ANIM_ATK_KT_END			0b00000000000000100
#define ANIM_ATK_FOOT_START		0b00000000000001000
#define ANIM_ATK_FOOT_END		0b00000000000010000

#define ANIM_ATK_HAMMER_START	0b00000000000000010
#define ANIM_ATK_HAMMER_END		0b00000000000000100

#define ANIM_ATK_LH_START		0b00000000000000010
#define ANIM_ATK_LH_END			0b00000000000000100
#define ANIM_ATK_RH_START		0b00000000000001000
#define ANIM_ATK_RH_END			0b00000000000010000

#define ANIM_ATK_LF_START		0b00000000000001000
#define ANIM_ATK_LF_END			0b00000000000010000
#define ANIM_ATK_RF_START		0b00000000000100000
#define ANIM_ATK_RF_END			0b00000000001000000

#define ANIM_ATK_LIGHT			0b00000000010000000
#define ANIM_ATK_HEAVY			0b00000000100000000
#define ANIM_ATK_BLOW			0b00000001000000000
#define ANIM_ATK_BLOWUP			0b00000010000000000
#define ANIM_ATK_KT_STING_START	0b00001000000000000
#define ANIM_ATK_KT_STING_END	0b00010000000000000
#define ANIM_ATK_BLOWDOWN		0b00100000000000000

#define ANIM_ATK_GADABLE		0b01000000000000000

// BYTE CONTEXT

#define ANIM_CAM_SHAKE			0b00000001

#define JUMP_SPEED				(11.0f)
#define AIRJUMP_SPEED			(13.0f)
#define BLOWUP_SPEED			(17.0f)
#define BLOWDOWN_SPEED			(-40.0f)
#define BLOWUP_VELOCITY			V3(0, BLOWUP_SPEED, 0)
#define BLOWDOWN_VELOCITY		V3(0, BLOWDOWN_SPEED, 0)

#define LOOP		true
#define NOLOOP		false
#define EXIT		nullptr
#define ANY			nullptr

#define MESH_JACK						TEXT("../Resource/Jack/Jack.FBX")
#define MESH_KATANA						TEXT("../Resource/Jack/Katana.FBX")
#define MESH_KATANA_SHEATH				TEXT("../Resource/Jack/KatanaSheath.FBX")
#define MESH_BEETLE_DRONE				TEXT("../Resource/BeetleDrone/BeetleDrone.FBX")
#define MESH_ANCIENT_KING				TEXT("../Resource/AncientKing/AncientKing.FBX")
#define MESH_ANCIENT_KING_HAMMER		TEXT("../Resource/AncientKing/AncientKingHammer.FBX")

#define TEX_SWORDTRAIL_GRADIENT			TEXT("../Resource/Trail/SwordTrailGradient.dds")
#define TEX_TRAIL_GRADIENT				TEXT("../Resource/Trail/TrailGradient.dds")
#define TEX_TRAIL_LINE					TEXT("../Resource/Trail/TrailLine.dds")
#define TEX_EFFECT_SPARK				TEXT("../Resource/Effect/Spark.dds")

#define TEX_ANIM_EXPLOSION				TEXT("../Resource/Flame/T_E_Explosion_03.dds")
#define TEX_ANIM_FIRE_SUBUV				TEXT("../Resource/Flame/T_E_Fire_SubUV_01.dds")
#define TEX_ANIM_FLAME01				TEXT("../Resource/Flame/T_E_Flame_01.dds")
#define TEX_ANIM_FLAME02				TEXT("../Resource/Flame/T_E_Flame_02.dds")
#define TEX_ANIM_FLAME03				TEXT("../Resource/Flame/T_E_Flame_03.dds")
#define TEX_ANIM_FLAME04				TEXT("../Resource/Flame/T_E_Flame_04.dds")

#define SHADER_TRAIL					TEXT("Trail.cso")
#define SHADER_BILLBOARD_EFFECT			TEXT("BillboardEffect.cso")
#define SHADER_BILLBOARD_ANIMATION		TEXT("BillboardAnimation.cso")

#endif