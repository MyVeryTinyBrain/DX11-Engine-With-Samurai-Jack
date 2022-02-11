#include "EnginePCH.h"
#include "CentralTimeElement.h"
#include "TimeElement.h"

CentralTimeElement::CentralTimeElement()
{
	m_updateTimeElement = new UpdateTimeElement(1.0f / 60.0f);
	SetFPS(60.0f);

	m_fixedUpdateTimeElement = new FixedUptateTimeElement(1.0f / 60.0f);
	SetFixedFPS(60.0f);

	m_programTimeElement = new ProgramTimeElement;
}

CentralTimeElement::~CentralTimeElement()
{
	SafeDelete(m_updateTimeElement);
	SafeDelete(m_fixedUpdateTimeElement);
	SafeDelete(m_programTimeElement);
}

bool CentralTimeElement::Initialize(float updateFPS, float fixedUpdateFPS)
{
	Reset();

	if (!SetFPS(updateFPS))
		return false;

	if (!SetFixedFPS(fixedUpdateFPS))
		return false;

	return true;
}

void CentralTimeElement::Reset()
{
	m_updateTimeElement->Reset();
	m_fixedUpdateTimeElement->Reset();
}

unsigned int CentralTimeElement::UpdateAccumulate()
{
	return m_updateTimeElement->Accumulate();
}

unsigned int CentralTimeElement::FixedUpdateAccumulate()
{
	return m_fixedUpdateTimeElement->Accumulate();
}

void CentralTimeElement::ProgramAccumulate()
{
	m_programTimeElement->Accumulate();
}

bool CentralTimeElement::SetFPS(float fps)
{
	if (fps == 0)
		fps = FLT_MAX;
	else if (fps < 0)
		return false;

	m_updateTimeElement->SetInterval(1.0f / fps);
	m_updateTargetFPS = fps;

	return true;
}

bool CentralTimeElement::SetFixedFPS(float fps)
{
	if (fps == 0)
		fps = FLT_MAX;
	else if (fps < 0)
		return false;

	m_fixedUpdateTimeElement->SetInterval(1.0f / fps);
	m_fixedUpdateTargetFPS = fps;

	return true;
}

float CentralTimeElement::GetFPS() const
{
	return m_updateTargetFPS;
}

float CentralTimeElement::GetFixedFPS() const
{
	return m_fixedUpdateTargetFPS;
}

void CentralTimeElement::SetTimeScale(float timeScale)
{
	m_updateTimeElement->SetTimeScale(timeScale);
	m_programTimeElement->SetTimeScale(timeScale);
}

void CentralTimeElement::SetFixedTimeScale(float timeScale)
{
	m_fixedUpdateTimeElement->SetTimeScale(timeScale);
}

float CentralTimeElement::GetTimeScale() const
{
	return m_updateTimeElement->GetTimeScale();
}

float CentralTimeElement::GetFixedTimeScale() const
{
	return m_fixedUpdateTimeElement->GetTimeScale();
}

float CentralTimeElement::GetDeltaTime() const
{
	return m_updateTimeElement->GetDeltaTime();
}

float CentralTimeElement::GetFixedDeltaTime() const
{
	return m_fixedUpdateTimeElement->GetDeltaTime();
}

float CentralTimeElement::GetUnscaledDelteTime() const
{
	return m_updateTimeElement->GetUnscaledDeltaTime();
}

float CentralTimeElement::GetUnscaledFixedDeltaTime() const
{
	return m_fixedUpdateTimeElement->GetUnscaledDeltaTime();
}

float CentralTimeElement::GetAccumulated() const
{
	return m_updateTimeElement->GetAccumulated();
}

float CentralTimeElement::GetAccumulatedSinceStartup() const
{
	return m_programTimeElement->GetAccumulated();
}

float CentralTimeElement::GetUnscaledAccumulatedSinceStartup() const
{
	return m_programTimeElement->GetUnscaledAccumulated();
}

float CentralTimeElement::GetFixedAccumulated() const
{
	return m_fixedUpdateTimeElement->GetAccumulated();
}

float CentralTimeElement::GetInterval() const
{
	return m_updateTimeElement->GetInterval();
}

float CentralTimeElement::GetFixedInterval() const
{
	return m_fixedUpdateTimeElement->GetInterval();
}
