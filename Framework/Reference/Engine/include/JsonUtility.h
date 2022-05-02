#pragma once

ENGINE_BEGIN

namespace JsonUtility
{
	ENGINE_API string Parse(const V4& value);
	ENGINE_API string Parse(const M4& value);
	ENGINE_API string Parse(const Color& value);
	ENGINE_API string Parse(const Q& value);

	ENGINE_API bool ToVector4(const string& str, V4& out);
	ENGINE_API bool ToMatrix4x4(const string& str, M4& out);
	ENGINE_API bool ToColor(const string& str, Color& out);
	ENGINE_API bool ToQuaternion(const string& str, Q& out);
};

ENGINE_END
