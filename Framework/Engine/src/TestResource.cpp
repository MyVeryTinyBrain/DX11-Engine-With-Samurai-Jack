#include "EnginePCH.h"
#include "TestResource.h"

TestResource::TestResource(ResourceManagement* management, const tstring& path) :
	ResourceObject(management, path)
{
}

TestResource::TestResource(ResourceManagement* management, const tstring& path, const tstring& groupName) :
	ResourceObject(management, groupName, path)
{
}

TestResource::TestResource(const tstring& path) :
	ResourceObject(path)
{
}

TestResource::TestResource() :
	ResourceObject()
{
}

TestResource::~TestResource()
{
	if (isUnmanaged)
		cout << "delete unmanaged" << endl;
	else
		cout << "delete managed" << endl;
}
