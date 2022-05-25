#pragma once

enum
{
	PhysicsLayer_Default = 0,
	PhysicsLayer_Player = 1,
	PhysicsLayer_Enemy = 2,
};

#define ANIM_ATK_KT_START		0b000000000001
#define ANIM_ATK_KT_END			0b000000000010
#define ANIM_ATK_FOOT_START		0b000000000100
#define ANIM_ATK_FOOT_END		0b000000001000

#define ANIM_ATK_LIGHT			0b000000010000
#define ANIM_ATK_HEAVY			0b000000100000
#define ANIM_ATK_BLOW			0b000001000000
#define ANIM_ATK_BLOWUP			0b000010000000
#define ANIM_JUMP				0b000100000000
#define ANIM_ATK_KT_STING_START	0b001000000000
#define ANIM_ATK_KT_STING_END	0b010000000000
#define ANIM_ATK_BLOWDOWN		0b100000000000

#define ANIM_ATK_LH_START		0b000000000001
#define ANIM_ATK_LH_END			0b000000000010
#define ANIM_ATK_RH_START		0b000000000100
#define ANIM_ATK_RH_END			0b000000001000

#define BLOWUP_VELOCITY		V3(0,17,0)
#define BLOWDOWN_VELOCITY	V3(0,-40,0)

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