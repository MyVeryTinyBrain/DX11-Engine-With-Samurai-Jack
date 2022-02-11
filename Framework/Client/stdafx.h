#pragma once

#include "framework.h"

#include <iostream>
using namespace std;

#include <Engine.h>
using namespace Engine;

#pragma region console
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#pragma endregion