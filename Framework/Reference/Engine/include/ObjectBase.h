#pragma once

#ifndef __ENGINE_OBJECT_BASE_H__
#define __ENGINE_OBJECT_BASE_H__

ENGINE_BEGIN
class ENGINE_API ObjectBase abstract
{
public:
	
	ObjectBase();

	ObjectBase(const tstring& name);

	virtual ~ObjectBase() = default;

	void SetName(const tstring& name);

	const tstring& GetName() const;

	_declspec(property(get = GetName, put = SetName)) const tstring& name;

private:

	tstring m_name = TEXT("");
};
ENGINE_END

#endif
