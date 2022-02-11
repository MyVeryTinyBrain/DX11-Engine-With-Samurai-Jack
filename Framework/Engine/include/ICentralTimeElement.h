#pragma once

ENGINE_BEGIN

class ENGINE_API ICentralTimeElement abstract
{
public:

	virtual ~ICentralTimeElement() = default;

	virtual bool Initialize(float updateFPS, float fixedUpdateFPS) = 0;

	virtual void Reset() = 0;

	virtual unsigned int UpdateAccumulate() = 0;

	virtual unsigned int FixedUpdateAccumulate() = 0;

	virtual void ProgramAccumulate() = 0;
};

ENGINE_END
