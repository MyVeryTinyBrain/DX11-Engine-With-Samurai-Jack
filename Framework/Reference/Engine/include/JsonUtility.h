#pragma once

ENGINE_BEGIN

struct ENGINE_API JsonUtility final
{
	static string Parse(const V4& value);
	static string Parse(const M4& value);
	static string Parse(const Color& value);
	static string Parse(const Q& value);

	static bool ToVector4(const string& str, V4& out);
	static bool ToMatrix4x4(const string& str, M4& out);
	static bool ToColor(const string& str, Color& out);
	static bool ToQuaternion(const string& str, Q& out);
};

ENGINE_END
