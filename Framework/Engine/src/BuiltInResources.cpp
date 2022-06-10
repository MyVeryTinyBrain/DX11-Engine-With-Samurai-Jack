#include "EnginePCH.h"
#include "BuiltInResources.h"
#include "ResourceFactory.h"
#include "VI.h"
#include "VIBuffer.h"
#include "VIUtility.h"
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

	if (FAILED(hr = CreateTexture2D(Color::white(), 2, 2, TEXT("white"), &m_white)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::gray(), 2, 2, TEXT("gray"), &m_gray)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::black(), 2, 2, TEXT("black"), &m_black)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::clear(), 2, 2, TEXT("clear"), &m_clear)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::red(), 2, 2, TEXT("red"), &m_red)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::green(), 2, 2, TEXT("green"), &m_green)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color::blue(), 2, 2, TEXT("blue"), &m_blue)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color(1, 1, 1, 0.5f), 2, 2, TEXT("white_transparent"), &m_whiteTransparent)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color(0, 0, 0, 0.5f), 2, 2, TEXT("black_transparent"), &m_blackTransparent)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color(0.5f, 0.5f, 1.0f, 1.0f), 2, 2, TEXT("normal"), &m_normal)))
		return hr;

	if (FAILED(hr = CreateTexture2D(Color(0.3f, 0.3f, 0.3f, 1.0f), 2, 2, TEXT("roughness"), &m_roughness)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(VIUtility::CreateQuad(), &m_quad)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(VIUtility::CreatePlane(), &m_plane)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(VIUtility::CreateBox(), &m_box)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(VIUtility::CreateSkyBox(), &m_skybox)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(VIUtility::CreateSphere(), &m_sphere)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(VIUtility::CreateCapsule(), &m_capsule)))
		return hr;

	if (FAILED(hr = CreateMeshNocopyVI(VIUtility::CreateCylinder(), &m_cylinder)))
		return hr;

	if (FAILED(hr = CreateShader(TEXT("Standard.cso"), &m_standardShader)))
		return hr;

	if (FAILED(hr = CreateShader(TEXT("StandardInstancing.cso"), &m_standardInstancingShader)))
		return hr;

	if (FAILED(hr = CreateShader(TEXT("Color.cso"), &m_colorShader)))
		return hr;

	if (FAILED(hr = CreateShader(TEXT("Wireframe.cso"), &m_wireframeShader)))
		return hr;

	if (FAILED(hr = CreateShader(TEXT("Unlit.cso"), &m_unlitShader)))
		return hr;

	if (nullptr == (m_standardMaterial = m_factory->CreateMaterialByShaderUM(m_standardShader)))
		return E_FAIL;

	if (nullptr == (m_whiteColorMaterial = m_factory->CreateMaterialByShaderUM(m_colorShader)))
		return E_FAIL;

	if (nullptr == (m_greenTransparentColorMaterial = m_factory->CreateMaterialByShaderUM(m_colorShader)))
		return E_FAIL;
	m_greenTransparentColorMaterial->SetColor("_Color", Color(0.0f, 1.0f, 0.0f, 0.1f));

	if (nullptr == (m_blueTransparentColorMaterial = m_factory->CreateMaterialByShaderUM(m_colorShader)))
		return E_FAIL;
	m_blueTransparentColorMaterial->SetColor("_Color", Color(0.0f, 0.0f, 1.0f, 0.1f));
	
	if (nullptr == (m_greenWireframeMaterial = m_factory->CreateMaterialByShaderUM(m_wireframeShader)))
		return E_FAIL;

	if (nullptr == (m_blueWireframeMaterial = m_factory->CreateMaterialByShaderUM(m_wireframeShader)))
		return E_FAIL;
	m_blueWireframeMaterial->SetColor("_Color", Color::blue());

	if (nullptr == (m_whiteUnlitMaterial = m_factory->CreateMaterialByShaderUM(m_unlitShader)))
		return E_FAIL;

	return S_OK;
}

void BuiltInResources::ReleaseBuiltInResources()
{
}

const ResourceRef<Texture2D>& BuiltInResources::GetWhiteTexture() const
{
	return m_white;
}

const ResourceRef<Texture2D>& BuiltInResources::GetGrayTexture() const
{
	return m_gray;
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

const ResourceRef<Texture2D>& BuiltInResources::GetNormalTexture() const
{
	return m_normal;
}

const ResourceRef<Texture2D>& BuiltInResources::GetRoughnessTexture() const
{
	return m_roughness;
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

const ResourceRef<Mesh>& BuiltInResources::GetSkyBoxMesh() const
{
	return m_skybox;
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

const ResourceRef<Shader>& BuiltInResources::GetStandardInstancingShader() const
{
	return m_standardInstancingShader;
}

const ResourceRef<Shader>& BuiltInResources::GetColorShader() const
{
	return m_colorShader;
}

const ResourceRef<Shader>& BuiltInResources::GetWireframeShader() const
{
	return m_wireframeShader;
}

const ResourceRef<Shader>& BuiltInResources::GetUnlitShader() const
{
	return m_unlitShader;
}

const ResourceRef<Material>& BuiltInResources::GetStandardMaterial() const
{
	return m_standardMaterial;
}

const ResourceRef<Material>& BuiltInResources::GetWhiteColorMaterial() const
{
	return m_whiteColorMaterial;
}

const ResourceRef<Material>& BuiltInResources::GetGreenTransparentColorMaterial() const
{
	return m_greenTransparentColorMaterial;
}

const ResourceRef<Material>& BuiltInResources::GetBlueTransparentColorMaterial() const
{
	return m_blueTransparentColorMaterial;
}

const ResourceRef<Material>& BuiltInResources::GetGreenWireframeMaterial() const
{
	return m_greenWireframeMaterial;
}

const ResourceRef<Material>& BuiltInResources::GetBlueWireframeMaterial() const
{
	return m_blueWireframeMaterial;
}

const ResourceRef<Material>& BuiltInResources::GetWhiteUnlitMaterial() const
{
	return m_whiteUnlitMaterial;
}

System* BuiltInResources::GetSystem() const
{
	return m_factory->GetManagement()->GetSystem();
}

HRESULT BuiltInResources::CreateTexture2D(const Color& color, unsigned int width, unsigned int height, tstring resourceKey, ResourceRef<Texture2D>* out_tex2D)
{
	TextureCreateDesc desc = {};
	desc.Color = color;
	desc.Size = uint2(width, height);
	desc.SetColor = true;
	desc.ColorSize = TextureCreationColorSize::_32;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (nullptr == (*out_tex2D = m_factory->CreateTexture2DM(desc, resourceKey)))
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
		system->graphic->device, system->graphic->deviceContext,
		&vi,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		D3D11_USAGE_IMMUTABLE, 0, 0,
		&viBuffer)))
	{
		SafeDelete(vi);
		return hr;
	}

	if (nullptr == (*out_mesh = m_factory->CreateMeshNocopyUM(&viBuffer)))
	{
		SafeDelete(viBuffer);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT BuiltInResources::CreateShader(const tstring& shaderPath, ResourceRef<Shader>* out_shader)
{
	if (nullptr == (*out_shader = m_factory->LoadShaderFromBinaryFolderM(shaderPath)))
		return E_FAIL;

	return S_OK;
}
