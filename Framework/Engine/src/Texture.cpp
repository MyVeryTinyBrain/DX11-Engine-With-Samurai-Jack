#include "EnginePCH.h"
#include "Texture.h"

Texture::Texture(
	ResourceManagement* management, bool managed, const tstring& path, const tstring& groupName) :
	ResourceObject(management, managed, path, groupName)
{
}

