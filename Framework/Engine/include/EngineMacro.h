#pragma once

#ifdef ENGINE
#define ENGINE_API _declspec(dllexport)
#else
#define ENGINE_API _declspec(dllimport)
#endif

#define ENGINE_BEGIN	namespace Engine {
#define ENGINE_END		}

// 메모리 반환 매크로입니다.

#define SAFE_DELETE(P)			{ if (P) { delete P; P = nullptr; } }
#define SAFE_DELETE_ARRAY(P)	{ if (P) { delete[] P; P = nullptr; } }

// 리터럴 문자열 "Text message" 형태로 오류 다이얼로그를 출력합니다.
// 매크로에 의해 윈도우 문자열로 변환됩니다.

#ifndef _AFX
#define ERROR_MESSAGE(TEXT_MESSAGE)			MessageBox(NULL, TEXT(TEXT_MESSAGE), TEXT("Message from Engine"), MB_OK)
#else
#define ERROR_MESSAGE(TEXT_MESSAGE)			AfxMessageBox(TEXT(TEXT_MESSAGE), MB_OK)
#endif

// 문자열을 전달받아 오류 다이얼로그를 출력합니다.
// 직접 알맞은 타입의 문자열을 전달해야 합니다.

#ifndef _AFX
#define ERROR_MESSAGE_NT(TEXT_MESSAGE)		MessageBox(NULL, TEXT_MESSAGE, TEXT("Message from Engine"), MB_OK)
#else
#define ERROR_MESSAGE_NT(TEXT_MESSAGE)		AfxMessageBox(TEXT_MESSAGE, MB_OK)
#endif

// 리터럴 문자를 받는 오류 다이얼로그와 함께 함수를 종료합니다.

#define RETURN_FALSE_ERROR_MESSAGE(TEXT_MESSAGE)		{ ERROR_MESSAGE(TEXT_MESSAGE); return false; };
#define RETURN_NULL_ERROR_MESSAGE(TEXT_MESSAGE)			{ ERROR_MESSAGE(TEXT_MESSAGE); return nullptr; };
#define RETURN_E_FAIL_ERROR_MESSAGE(TEXT_MESSAGE)		{ ERROR_MESSAGE(TEXT_MESSAGE); return E_FAIL; };

// 알맞은 타입의 문자열을 받는 오류 다이얼로그와 함께 함수를 종료합니다.

#define RETURN_FALSE_ERROR_MESSAGE_NT(TEXT_MESSAGE)		{ ERROR_MESSAGE_NT(TEXT_MESSAGE); return false; };
#define RETURN_NULL_ERROR_MESSAGE_NT(TEXT_MESSAGE)		{ ERROR_MESSAGE_NT(TEXT_MESSAGE); return nullptr; };
#define RETURN_E_FAIL_ERROR_MESSAGE_NT(TEXT_MESSAGE)	{ ERROR_MESSAGE_NT(TEXT_MESSAGE); return E_FAIL; };