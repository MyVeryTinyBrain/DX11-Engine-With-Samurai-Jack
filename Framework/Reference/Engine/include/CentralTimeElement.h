#pragma once

#include "ICentralTimeElement.h"

ENGINE_BEGIN
class ENGINE_API CentralTimeElement final : public ICentralTimeElement
{
public:

	CentralTimeElement();

	virtual ~CentralTimeElement();

private:

	virtual bool Initialize(float updateFPS, float fixedUpdateFPS) final override;

	virtual void Reset() final override;

	virtual unsigned int UpdateAccumulate() final override;

	virtual unsigned int FixedUpdateAccumulate() final override;

	virtual void ProgramAccumulate() override;

public:

	bool SetFPS(float fps);

	bool SetFixedFPS(float fps);

	float GetFPS() const;

	float GetFixedFPS() const;
	
	void SetTimeScale(float timeScale);

	void SetFixedTimeScale(float timeScale);

	float GetTimeScale() const;

	float GetFixedTimeScale() const;

	float GetDeltaTime() const;

	float GetFixedDeltaTime() const;

	float GetUnscaledDelteTime() const;

	float GetUnscaledFixedDeltaTime() const;

	float GetAccumulated() const;

	float GetAccumulatedSinceStartup() const;

	float GetUnscaledAccumulatedSinceStartup() const;

	float GetFixedAccumulated() const;

	float GetInterval() const;

	float GetFixedInterval() const;

	_declspec(property(get = GetDeltaTime)) float deltaTime;
	_declspec(property(get = GetFixedDeltaTime)) float fixedDeltaTime;
	_declspec(property(get = GetTimeScale, put = SetTimeScale)) float timeScale;
	_declspec(property(get = GetFixedTimeScale, put = SetFixedTimeScale)) float fixedTimeScale;

private:

	float						m_updateTargetFPS;

	class TimeElement*			m_updateTimeElement;

	float						m_fixedUpdateTargetFPS;

	class TimeElement*			m_fixedUpdateTimeElement;

	class ProgramTimeElement*	m_programTimeElement;
};
ENGINE_END
