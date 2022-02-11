#pragma once

ENGINE_BEGIN

struct ENGINE_API ModelMaterialDesc
{
	// file Names

	// aiTextureType_NONE
	TCHAR none[MAX_PATH] = {};
	inline bool HasNone() const { return none[0] != 0; }

	// aiTextureType_DIFFUSE
	TCHAR diffuse[MAX_PATH] = {};
	inline bool HasDiffuse() const { return diffuse[0] != 0; }

	// aiTextureType_SPECULAR
	TCHAR specularMask[MAX_PATH] = {};
	inline bool HasSpecular() const { return specularMask[0] != 0; }

	// aiTextureType_AMBIENT
	TCHAR ambient[MAX_PATH] = {};
	inline bool HasAmbient() const { return ambient[0] != 0; }

	// aiTextureType_EMISSIVE
	TCHAR emission[MAX_PATH] = {};
	inline bool HasEmission() const { return emission[0] != 0; }

	// aiTextureType_HEIGHT
	TCHAR height[MAX_PATH] = {};
	inline bool HasHeight() const { return height[0] != 0; }

	// aiTextureType_NORMALS
	TCHAR normals[MAX_PATH] = {};
	inline bool HasNormals() const { return normals[0] != 0; }

	// aiTextureType_SHININESS
	TCHAR shininess[MAX_PATH] = {};
	inline bool HasShininess() const { return shininess[0] != 0; }

	// aiTextureType_OPACITY
	TCHAR opacity[MAX_PATH] = {};
	inline bool HasOpacity() const { return opacity[0] != 0; }

	// aiTextureType_DISPLACEMENT
	TCHAR displacement[MAX_PATH] = {};
	inline bool HasDisplacement() const { return displacement[0] != 0; }

	// aiTextureType_LIGHTMAP
	TCHAR lightmap[MAX_PATH] = {};
	inline bool HasLightmap() const { return lightmap[0] != 0; }

	// aiTextureType_REFLECTION
	TCHAR reflection[MAX_PATH] = {};
	inline bool HasReflection() const { return reflection[0] != 0; }

	// aiTextureType_BASE_COLOR
	TCHAR baseColor[MAX_PATH] = {};
	inline bool HasBaseColor() const { return baseColor[0] != 0; }

	// aiTextureType_UNKNOWN
	TCHAR unknown[MAX_PATH] = {};
	inline bool HasUnknown() const { return unknown[0] != 0; }

	uint numSucceeded = 0;
};

ENGINE_END
