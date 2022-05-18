#pragma once

ENGINE_BEGIN

class Collider;

using PhysicsQueryTypes = uint;
struct PhysicsQueryType
{
	enum : uint
	{
		None = 0,
		Collider = 1 << 0,
		Trigger = 1 << 1,
		All = Collider | Trigger,
	};
};

struct PhysicsHit
{
	V3			Point;
	V3			Normal;
	float		Distance;
	Collider*	Collider;
};

struct PhysicsRay
{
	V3			Point;
	V3			Direction;
	float		Length;
};

ENGINE_END

