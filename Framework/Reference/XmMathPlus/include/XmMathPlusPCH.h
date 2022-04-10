#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>
using namespace DirectX;

#include <memory>
#include <cmath>
#include <math.h>

#include <algorithm>
#include <vector>

#include "XmMathPlusMacro.h"
#include "Type.h"
#include "Real.h"
#include "Ex.h"
#include "Ex2.h"

#if defined(XM_CONST)
#undef XM_CONST
#endif
#if defined(XM_CONSTEXPR)
#undef XM_CONSTEXPR
#endif
#if defined(_MSC_VER) && (_MSC_FULL_VER < 190023506)
#define XM_CONST const
#define XM_CONSTEXPR
#else
#define XM_CONST constexpr
#define XM_CONSTEXPR constexpr
#endif