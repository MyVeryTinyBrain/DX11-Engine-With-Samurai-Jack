#include "EnginePCH.h"
#include "Times.h"
#include "CentralTimeElement.h"

Times::Times(CentralTimeElement* time)
{
	m_timeElement = time;
}

bool Times::SetFPS(float fps)
{
	return m_timeElement->SetFPS(fps);
}

bool Times::SetFixedFPS(float fps)
{
	return m_timeElement->SetFixedFPS(fps);
}

float Times::GetFPS() const
{
	return m_timeElement->GetFPS();
}

float Times::GetFixedFPS() const
{
	return m_timeElement->GetFixedFPS();
}

void Times::SetTimeScale(float timeScale)
{
	m_timeElement->SetTimeScale(timeScale);
}

void Times::SetFixedTimeScale(float timeScale)
{
	m_timeElement->SetFixedTimeScale(timeScale);
}

float Times::GetTimeScale() const
{
	return m_timeElement->GetTimeScale();
}

float Times::GetFixedTimeScale() const
{
	return m_timeElement->GetFixedTimeScale();
}

float Times::GetDeltaTime() const
{
	return m_timeElement->GetDeltaTime();
}

float Times::GetFixedDeltaTime() const
{
	return m_timeElement->GetFixedDeltaTime();
}

float Times::GetUnscaledDelteTime() const
{
	return m_timeElement->GetUnscaledDelteTime();
}

float Times::GetUnscaledFixedDeltaTime() const
{
	return m_timeElement->GetUnscaledFixedDeltaTime();
}

float Times::GetAccumulated() const
{
	return m_timeElement->GetAccumulated();
}

float Times::GetAccumulatedSinceStartup() const
{
	return m_timeElement->GetAccumulatedSinceStartup();
}

float Times::GetUnsclaedAccumulatedSinceStartup() const
{
	return m_timeElement->GetUnscaledAccumulatedSinceStartup();
}

float Times::GetFixedAccumulated() const
{
	return m_timeElement->GetFixedAccumulated();
}

float Times::GetInterval() const
{
	return m_timeElement->GetInterval();
}

float Times::GetFixedInterval() const
{
	return m_timeElement->GetFixedInterval();
}
