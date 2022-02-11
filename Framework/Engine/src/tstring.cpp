#include "EnginePCH.h"
#include "tstring.h"
#include <atlconv.h>
#include <string>
#include <locale>
#include <codecvt>

std::wstring STRING_FUNCTIONS::str_ansi_to_wstr_utf16(const string& str_ansi)
{
	int size = MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str_ansi.c_str(),
		int(str_ansi.length()), nullptr, 0);
	std::wstring utf16_wstr(size_t(size), '\0');
	MultiByteToWideChar(CP_ACP, MB_COMPOSITE, str_ansi.c_str(),
		int(str_ansi.length()), &utf16_wstr[0], size);

	return utf16_wstr;
}

std::string STRING_FUNCTIONS::wstr_utf16_to_str_utf8(const wstring& wstr_utf16)
{
	int utf8_size = WideCharToMultiByte(CP_UTF8, 0, wstr_utf16.c_str(),
		int(wstr_utf16.length()), nullptr, 0,
		nullptr, nullptr);
	std::string utf8_str(size_t(utf8_size), '\0');
	WideCharToMultiByte(CP_UTF8, 0, wstr_utf16.c_str(),
		int(wstr_utf16.length()), &utf8_str[0], utf8_size,
		nullptr, nullptr);

	return utf8_str;
}

std::wstring STRING_FUNCTIONS::str_utf8_to_wstr_utf16(const string& str_utf8)
{
	wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> convert;
	u16string utf16 = convert.from_bytes(str_utf8);
	wstring wstr(utf16.begin(), utf16.end());
	return wstr;
}

std::string STRING_FUNCTIONS::str_utf8_to_str_ansi(const string& str_utf8)
{
	wstring wstr_utf16 = str_utf8_to_wstr_utf16(str_utf8);
	return wstring_to_string(wstr_utf16);
}

std::string STRING_FUNCTIONS::str_ansi_to_str_utf8(const string& str_ansi)
{
	std::wstring wstr_utf16 = str_ansi_to_wstr_utf16(str_ansi);
	std::string str_utf8 = wstr_utf16_to_str_utf8(wstr_utf16);

	return str_utf8;
}

std::string STRING_FUNCTIONS::wstring_to_string(std::wstring wstr)
{
	USES_CONVERSION;
	return std::string(W2A(wstr.c_str()));
}

std::wstring STRING_FUNCTIONS::string_to_wstring(std::string str)
{
	USES_CONVERSION;
	return std::wstring(A2W(str.c_str()));
}
