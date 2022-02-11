#pragma once

ENGINE_BEGIN
class ENGINE_API Times final
{
public:

	Times(class CentralTimeElement* time);

	~Times() = default;

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

	float GetUnsclaedAccumulatedSinceStartup() const;

	float GetFixedAccumulated() const;

	float GetInterval() const;

	float GetFixedInterval() const;

	_declspec(property(get = GetAccumulatedSinceStartup)) float accumulatedSinceStartup;
	_declspec(property(get = GetUnsclaedAccumulatedSinceStartup)) float unscaledAccumulatedSinceStartup;
	_declspec(property(get = GetDeltaTime)) float deltaTime;
	_declspec(property(get = GetFixedDeltaTime)) float fixedDeltaTime;
	_declspec(property(get = GetUnscaledDelteTime)) float unscaledDeltaTime;
	_declspec(property(get = GetUnscaledFixedDeltaTime)) float unscaledFixedDeltaTime;
	_declspec(property(get = GetTimeScale, put = SetTimeScale)) float timeScale;
	_declspec(property(get = GetFixedTimeScale, put = SetFixedTimeScale)) float fixedTimeScale;

private:

	class CentralTimeElement* m_timeElement = nullptr;
};

ENGINE_END
