#pragma once

ENGINE_BEGIN

class ICamera;

enum class LightType : unsigned int
{
	Directional = 0, 
	Point = 1, 
	Spot = 2,
	Max,
};

struct LightDesc
{
	bool		DrawShadow;
	LightType	Type;
	float		ShadowWhiteness;
	float		Intensity;
	float		Range; // (0~Infinite)
	float		Angle; // (0~90)
	float		Near;
	float		Far;
	V4			Position;
	V4			Direction;
	Color		Diffuse;
	M4			ViewMatrix[6];
	M4			ProjectionMatrix[6];
};

class ENGINE_API ILight abstract
{
public:

	virtual ~ILight() = default;

public:

	virtual bool IsWorking() const = 0;

	virtual bool IsDrawShadow() const = 0;

	virtual float GetIntensity() const = 0;

	virtual const Color& GetDiffuse() const = 0;

	virtual float GetShadowWhiteness() const = 0;
	
public:

	virtual LightType GetLightType() const = 0;

	virtual uint GetProjectionCount() const = 0;

	virtual void GetBoundingHolders(ICamera* camera, BoundingHolder* out_arrBoundingHolder) const = 0;

	virtual LightDesc GetLightDesc(ICamera* camera) const = 0;

	virtual bool ContainsInCamera(ICamera* camera) const = 0;

};

ENGINE_END
