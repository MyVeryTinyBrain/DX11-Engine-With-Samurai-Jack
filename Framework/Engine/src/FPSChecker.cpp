#include "EnginePCH.h"
#include "FPSChecker.h"
#include "System.h"
#include "Times.h"

void FPSChecker::Update()
{
	m_accumulated += system->time->unscaledDeltaTime;
	++m_fps;

	if (m_accumulated > 1.0f)
	{
		cout << m_fps << endl;
		m_fps = 0;
		m_accumulated = 0;
	}
}
