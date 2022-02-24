#pragma once

ENGINE_BEGIN

class ICamera;
class DepthStencil;

enum class LightType : unsigned int
{
	Directional = 0, 
	Point = 1, 
	Spot = 2,
	Max,
};

struct LightDesc
{
	BOOL		DrawShadow;
	LightType	Type;
	uint		DepthSize;
	float		ShadowWhiteness;
	float		Intensity;
	float		Range; // (0~Infinite)
	float		Angle; // (0~90)
	float		Near;
	float		Far;
	V4			Position;
	V4			Direction;
	Color		Diffuse;
	Color		Ambient;
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

	virtual const Color& GetAmbient() const = 0;

	virtual float GetShadowWhiteness() const = 0;
	
public:

	virtual LightType GetLightType() const = 0;

	virtual uint GetProjectionCount() const = 0;

	virtual void GetBoundingHolders(ICamera* camera, BoundingHolder* out_arrBoundingHolder) const = 0;

	virtual LightDesc GetLightDesc(ICamera* camera) const = 0;

	virtual bool ContainsInCamera(ICamera* camera) const = 0;

public:

	virtual uint GetDepthSize() const = 0;

	virtual void GetDepthes(DepthStencil** out_depthes) const = 0;

	virtual void ClearDepthes() = 0;

};

ENGINE_END
