#pragma once

#pragma warning(disable : 4251)

// Libraries ==================================================================

// XmMathPlus
#include <XmMathPlus.h>
using namespace XmMathPlus;
using namespace XmMathPlus::Ex;

// Json
#include <json.h>

// PhysX4.1
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
using namespace physx;
#define PxRelease(_PxCom)			if (_PxCom) { _PxCom->release(); _PxCom = nullptr; }
#define FromPxVec3(_PxVector3)		V3(_PxVector3.x, _PxVector3.y, _PxVector3.z)
#define ToPxVec3(_V3)				PxVec3(_V3.x, _V3.y, _V3.z)
#define FromPxQuat(_PxQuaternion)	Q(_PxQuaternion.x, _PxQuaternion.y, _PxQuaternion.z, _PxQuaternion.w)
#define ToPxQuat(_Q)				PxQuat(_Q.x, _Q.y, _Q.z, _Q.w)

// DirectX11
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
using namespace DirectX;
using namespace DirectX::PackedVector;

// DirectXTex
#include <DirectXTex.h>

// Effects11
#include <d3dx11effect.h>
#include <d3dcompiler.h>

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ImGui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Standards ==================================================================

#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <typeinfo>
#include <typeindex>
#include <Windows.h>
#include <thread>
#include <condition_variable>
#include <random>
using namespace std;

// WRL
#include <wrl.h>
#include <wrl/client.h>
template <typename T>
using Com = Microsoft::WRL::ComPtr<T>;

// Project PCH ================================================================

// Memory leak
#include <crtdbg.h>
#if _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// Project
namespace Engine {};
using namespace Engine;
#include "EngineMacro.h"
#include "tstring.h"
#include "Delegate.h"
#include "EngineType.h"
#include "EngineFunction.h"

// ============================================================================
