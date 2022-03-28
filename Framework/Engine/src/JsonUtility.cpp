#include "EnginePCH.h"
#include "JsonUtility.h"

string JsonUtility::Parse(const V4& value)
{
	Json::Value json;
	json[0] = value.x;
	json[1] = value.y;
	json[2] = value.z;
	json[3] = value.w;

	Json::StreamWriterBuilder builder;
	return Json::writeString(builder, json);
}

string JsonUtility::Parse(const M4& value)
{
	Json::Value json;
	json[0] = Parse(V4(value.row[0]));
	json[1] = Parse(V4(value.row[1]));
	json[2] = Parse(V4(value.row[2]));
	json[3] = Parse(V4(value.row[3]));

	Json::StreamWriterBuilder builder;
	return Json::writeString(builder, json);
}

string JsonUtility::Parse(const Color& value)
{
	V4 vector = value.value;
	return Parse(vector);
}

string JsonUtility::Parse(const Q& value)
{
	V4 vector = value.xmv;
	return Parse(vector);
}

bool JsonUtility::ToVector4(const string& str, V4& out)
{
	Json::CharReaderBuilder builder;
	unique_ptr<Json::CharReader> reader(builder.newCharReader());
	
	Json::Value json;
	string err;
	if (!reader->parse(str.c_str(), str.c_str() + str.length(), &json, &err))
		return false;

	if (json.size() != 4)
		return false;

	if (!json[0].isConvertibleTo(Json::ValueType::realValue) ||
		!json[1].isConvertibleTo(Json::ValueType::realValue) ||
		!json[2].isConvertibleTo(Json::ValueType::realValue) ||
		!json[3].isConvertibleTo(Json::ValueType::realValue))
		return false;

	out.x = json[0].asFloat();
	out.y = json[1].asFloat();
	out.z = json[2].asFloat();
	out.w = json[3].asFloat();

	return true;
}

bool JsonUtility::ToMatrix4x4(const string& str, M4& out)
{
	Json::CharReaderBuilder builder;
	unique_ptr<Json::CharReader> reader(builder.newCharReader());

	Json::Value json;
	string err;
	if (!reader->parse(str.c_str(), str.c_str() + str.length(), &json, &err))
		return false;

	if (json.size() != 4)
		return false;

	if (!json[0].isConvertibleTo(Json::ValueType::objectValue) ||
		!json[1].isConvertibleTo(Json::ValueType::objectValue) ||
		!json[2].isConvertibleTo(Json::ValueType::objectValue) ||
		!json[3].isConvertibleTo(Json::ValueType::objectValue))
		return false;

	V4 row[4];
	
	if (!ToVector4(json[0].asString(), row[0]))
		return false;

	if (!ToVector4(json[1].asString(), row[1]))
		return false;

	if (!ToVector4(json[2].asString(), row[2]))
		return false;

	if (!ToVector4(json[3].asString(), row[3]))
		return false;

	out.row[0] = row[0];
	out.row[1] = row[1];
	out.row[2] = row[2];
	out.row[3] = row[3];

	return true;
}

bool JsonUtility::ToColor(const string& str, Color& out)
{
	V4 vector;
	bool result = ToVector4(str, vector);
	if (result)
		out = vector;
	return result;
}

bool JsonUtility::ToQuaternion(const string& str, Q& out)
{
	V4 vector;
	bool result = ToVector4(str, vector);
	if (result)
		out = vector;
	return result;
}
