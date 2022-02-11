#pragma once

ENGINE_BEGIN
struct TransformData 
{
	inline TransformData() :
		position(V3::zero()),
		rotation(Q::identity()),
		eulerAngles(V3::zero()),
		scale(V3::one())
	{
	}

	inline TransformData(const V3& position, const Q& rotation, const V3& scale) :
		position(position),
		rotation(rotation),
		eulerAngles(rotation.eulerAngles),
		scale(scale)
	{
	}

	inline TransformData(const V3& position, const V3& eulerAngles, const V3& scale) :
		position(position),
		rotation(Q::Euler(eulerAngles)),
		eulerAngles(eulerAngles),
		scale(scale)
	{
	}

	inline TransformData(const TransformData& other) :
		position(other.position),
		rotation(other.rotation),
		eulerAngles(other.eulerAngles),
		scale(other.scale)
	{
	}

	inline bool operator == (const TransformData& other)
	{
		return
			position == other.position &&
			rotation == other.rotation &&
			scale == other.scale;
	}

	inline bool operator != (const TransformData& other)
	{
		return
			position != other.position ||
			rotation != other.rotation ||
			scale != other.scale;
	}

	void UpdateRotation();

	void UpdateEulerAngles();

	V3 position;

	Q rotation;

	V3 eulerAngles;

	V3 scale;
};

ENGINE_END
