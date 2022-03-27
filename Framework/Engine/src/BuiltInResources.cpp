#include "EnginePCH.h"
#include "BuiltInResources.h"
#include "ResourceFactory.h"
#include "VI.h"
#include "VIBuffer.h"
#include "PrimitiveVI.h"
#include "System.h"
#include "GraphicSystem.h"
#include "ResourceManagement.h"

#include "Texture2D.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"

BuiltInResources::BuiltInResources()
{
}

BuiltInResources::~BuiltInResources()
{
	ReleaseBuiltInResources();
}

HRESULT BuiltInResources::Initialize(ResourceFactory* factory)
{
	if (!factory)
		return E_FAIL;

	m_factory = factory;

	HRESULT hr = S_OK;

	if (FAILED(hr = CreateBuiltInResources()))
	{
		ERROR_MESSAGE("Failed to setup Built-in resources");
		return hr;
	}

	return S_OK;
}

HRESULT BuiltInResources::CreateBuiltInResources()
{
	HRESULT hr = S_OK;

	if (FAILED(hr = CreateTexture2D(Color::white(), 16, 16, &m_white)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::black(), 16, 16, &m_black)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::clear(), 16, 16, &m_clear)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::red(), 16, 16, &m_red)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::green(), 16, 16, &m_green)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::blue(), 16, 16, &m_blue)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color(1, 1, 1, 0.5f), 16, 16, &m_whiteTransparent)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color(0, 0, 0, 0.5f), 16, 16, &m_blackTransparent)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(PrimitiveVI::CreateQuad(), &m_quad)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(PrimitiveVI::CreatePlane(), &m_plane)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(PrimitiveVI::CreateBox(), &m_box)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(PrimitiveVI::CreateSphere(), &m_sphere)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(PrimitiveVI::CreateCapsule(), &m_capsule)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(PrimitiveVI::CreateCylinder(), &m_cylinder)))
		return hr;

	if (FAILED(hr = CreateShader(TEXT("../Shader/Standard.fx"), &m_standardShader)))
		return hr;

	if (FAILED(hr = CreateShader(TEXT("../Shader/Color.fx"), &m_colorShader)))
		return hr;

	if (nullptr == (m_standardMaterial = m_factory->CreateUnmanagedMaterialByShader(m_standardShader->path)))
		return E_FAIL;

	if (nullptr == (m_wireframeMaterial = m_factory->CreateUnmanagedMaterialByShader(m_colorShader->path)))
		return E_FAIL;

	return S_OK;
}

void BuiltInResources::ReleaseBuiltInResources()
{
	// Texture2D ==========================================

	m_white = nullptr;

	m_black = nullptr;

	m_whiteTransparent = nullptr;

	m_blackTransparent = nullptr;

	// Mesh ===============================================

	m_quad = nullptr;

	m_plane = nullptr;

	m_box = nullptr;

	m_sphere = nullptr;

	m_capsule = nullptr;

	m_cylinder = nullptr;

	// Shader =============================================

	m_standardShader = nullptr;
	m_colorShader = nullptr;

	// Material ===========================================


}

const ResourceRef<Texture2D>& BuiltInResources::GetWhiteTexture() const
{
	return m_white;
}

const ResourceRef<Texture2D>& BuiltInResources::GetBlackTexture() const
{
	return m_black;
}

const ResourceRef<Texture2D>& BuiltInResources::GetClearTexture() const
{
	return m_clear;
}

const ResourceRef<Texture2D>& BuiltInResources::GetRedTexture() const
{
	return m_red;
}

const ResourceRef<Texture2D>& BuiltInResources::GetGreenTexture() const
{
	return m_green;
}

const ResourceRef<Texture2D>& BuiltInResources::GetBlueTexture() const
{
	return m_blue;
}

const ResourceRef<Texture2D>& BuiltInResources::GetTransparentWhiteTexture() const
{
	return m_whiteTransparent;
}

const ResourceRef<Texture2D>& BuiltInResources::GetTransparentBlackTexture() const
{
	return m_blackTransparent;
}

const ResourceRef<Mesh>& BuiltInResources::GetQuadMesh() const
{
	return m_quad;
}

const ResourceRef<Mesh>& BuiltInResources::GetPlaneMesh() const
{
	return m_plane;
}

const ResourceRef<Mesh>& BuiltInResources::GetBoxMesh() const
{
	return m_box;
}

const ResourceRef<Mesh>& BuiltInResources::GetSphereMesh() const
{
	return m_sphere;
}

const ResourceRef<Mesh>& BuiltInResources::GetCapsuleMesh() const
{
	return m_capsule;
}

const ResourceRef<Mesh>& BuiltInResources::GetCylinderMesh() const
{
	return m_cylinder;
}

const ResourceRef<Shader>& BuiltInResources::GetStandardShader() const
{
	return m_standardShader;
}

const ResourceRef<Shader>& BuiltInResources::GetColorShader() const
{
	return m_colorShader;
}

const ResourceRef<Material>& BuiltInResources::GetStandardMaterial() const
{
	return m_standardMaterial;
}

const ResourceRef<Material>& BuiltInResources::GetWireframeMaterial() const
{
	return m_wireframeMaterial;
}

System* BuiltInResources::GetSystem() const
{
	return m_factory->GetManagement()->GetSystem();
}

HRESULT BuiltInResources::CreateTexture2D(const Color& color, unsigned int width, unsigned int height, ResourceRef<Texture2D>* out_tex2D)
{
	if (nullptr == (*out_tex2D = m_factory->CreateUnmanagedTexture2D(color, width, height)))
		return E_FAIL;

	return S_OK;
}

HRESULT BuiltInResources::CreateMeshNocopyVI(VI* vi, ResourceRef<Mesh>* out_mesh)
{
	VIBuffer* viBuffer = nullptr;

	if (!vi)
		return E_FAIL;

	HRESULT hr = S_OK;

	if (FAILED(hr = VIBuffer::CreateVIBufferNocopy(
		system->graphicSystem->device, system->graphicSystem->deviceContext,
		&vi,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&viBuffer)))
	{
		SafeDelete(vi);
		return hr;
	}

	if (nullptr == (*out_mesh = m_factory->CreateUnamanagedMeshNocopy(&viBuffer)))
	{
		SafeDelete(viBuffer);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT BuiltInResources::CreateShader(const tstring& shaderPath, ResourceRef<Shader>* out_shader)
{
	if (nullptr == (*out_shader = m_factory->CreateManagedShaderFromFile(shaderPath)))
		return E_FAIL;

	return S_OK;
}
