#pragma once

#ifdef MATHPLUS
#define XMMATHPLUS_API //_declspec(dllexport)
#else
#define XMMATHPLUS_API //_declspec(dllimport)
#endif