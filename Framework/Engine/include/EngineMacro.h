#pragma once

#ifdef ENGINE
#define ENGINE_API _declspec(dllexport)
#else
#define ENGINE_API _declspec(dllimport)
#endif

#define ENGINE_BEGIN	namespace Engine {
#define ENGINE_END		}

// �޸� ��ȯ ��ũ���Դϴ�.

#define SAFE_DELETE(P)			{ if (P) { delete P; P = nullptr; } }
#define SAFE_DELETE_ARRAY(P)	{ if (P) { delete[] P; P = nullptr; } }

// ���ͷ� ���ڿ� "Text message" ���·� ���� ���̾�α׸� ����մϴ�.
// ��ũ�ο� ���� ������ ���ڿ��� ��ȯ�˴ϴ�.

#ifndef _AFX
#define ERROR_MESSAGE(TEXT_MESSAGE)			MessageBox(NULL, TEXT(TEXT_MESSAGE), TEXT("Message from Engine"), MB_OK)
#else
#define ERROR_MESSAGE(TEXT_MESSAGE)			AfxMessageBox(TEXT(TEXT_MESSAGE), MB_OK)
#endif

// ���ڿ��� ���޹޾� ���� ���̾�α׸� ����մϴ�.
// ���� �˸��� Ÿ���� ���ڿ��� �����ؾ� �մϴ�.

#ifndef _AFX
#define ERROR_MESSAGE_NT(TEXT_MESSAGE)		MessageBox(NULL, TEXT_MESSAGE, TEXT("Message from Engine"), MB_OK)
#else
#define ERROR_MESSAGE_NT(TEXT_MESSAGE)		AfxMessageBox(TEXT_MESSAGE, MB_OK)
#endif

// ���ͷ� ���ڸ� �޴� ���� ���̾�α׿� �Բ� �Լ��� �����մϴ�.

#define RETURN_FALSE_ERROR_MESSAGE(TEXT_MESSAGE)		{ ERROR_MESSAGE(TEXT_MESSAGE); return false; };
#define RETURN_NULL_ERROR_MESSAGE(TEXT_MESSAGE)			{ ERROR_MESSAGE(TEXT_MESSAGE); return nullptr; };
#define RETURN_E_FAIL_ERROR_MESSAGE(TEXT_MESSAGE)		{ ERROR_MESSAGE(TEXT_MESSAGE); return E_FAIL; };

// �˸��� Ÿ���� ���ڿ��� �޴� ���� ���̾�α׿� �Բ� �Լ��� �����մϴ�.

#define RETURN_FALSE_ERROR_MESSAGE_NT(TEXT_MESSAGE)		{ ERROR_MESSAGE_NT(TEXT_MESSAGE); return false; };
#define RETURN_NULL_ERROR_MESSAGE_NT(TEXT_MESSAGE)		{ ERROR_MESSAGE_NT(TEXT_MESSAGE); return nullptr; };
#define RETURN_E_FAIL_ERROR_MESSAGE_NT(TEXT_MESSAGE)	{ ERROR_MESSAGE_NT(TEXT_MESSAGE); return E_FAIL; };