#pragma once

#include <string>

ENGINE_BEGIN

#ifdef UNICODE
typedef std::wstring tstring;
#define tcout wcout
#else 
typdef std::string tstring;
#define tcout cout
#endif

struct ENGINE_API string_functions final
{
	static std::wstring str_ansi_to_wstr_utf16(const string& str_ansi);

	static std::string wstr_utf16_to_str_utf8(const wstring& wstr_utf16);

	static std::wstring str_utf8_to_wstr_utf16(const string& str_utf8);

	static std::string str_utf8_to_str_ansi(const string& str_utf8);

	static std::string str_ansi_to_str_utf8(const string& str_ansi);

	// utf16 -> ansi
	static std::string wstring_to_string(std::wstring wstr);

	// ansi -> utf16
	static std::wstring string_to_wstring(std::string str);

	template<typename ... Args>
	inline static std::string string_format(const std::string& format, Args ... args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		if (size <= 0)
		{
			throw std::runtime_error("Error during formatting.");
		}
		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1);
	}

	template<typename ... Args>
	inline static std::wstring wstring_format(const std::wstring& format, Args ... args)
	{
#pragma warning(disable : 4996)
		size_t size = _snwprintf(nullptr, 0, format.c_str(), args ...) + 1;
		if (size <= 0)
		{
			throw std::runtime_error("Error during formatting.");
		}
		std::unique_ptr<wchar_t[]> buf(new wchar_t[size]);
		_snwprintf(buf.get(), size, format.c_str(), args ...);
		return std::wstring(buf.get(), buf.get() + size - 1);
#pragma warning(default : 4996)
	}

	inline static std::string tstring_to_string(const tstring& tstr)
	{
#ifdef UNICODE
		return string_functions::wstring_to_string(tstr);
#else
		return tstr;
#endif
	}

	inline static std::wstring tstring_to_wstring(const tstring& tstr)
	{
#ifdef UNICODE
		return tstr;
#else
		return string_functions::string_to_wstring(tstr);
#endif
	}
};

#define tstring_to_string(tstr)			string_functions::tstring_to_string(tstr)
#define tstring_to_wstring(tstr)		string_functions::tstring_to_wstring(tstr)

#ifdef UNICODE
#define tstring_format(format, ...)		string_functions::wstring_format(format, __VA_ARGS__)
#define to_tstring(value)				std::to_wstring(value)
#define tcout	wcout
#define wstring_to_tstring(wstr)		wstr
#define string_to_tstring(str)			string_functions::string_to_wstring(str)
#define tstring_to_str_utf8(tstr)		string_functions::wstr_utf16_to_str_utf8(tstr)
#else
#define tstring_format(format, ...)		string_functions::string_format(format, __VA_ARGS__)
#define tcout	cout
#define to_tstring(value)				std::to_string(value)
#define wstring_to_tstring(wstr)		string_functions::wstring_to_string(wstr)
#define string_to_tstring(str)			str
#define tstring_to_str_utf8(tstr)		string_functions::str_ansi_to_str_utf8(tstr)
#endif

ENGINE_END
