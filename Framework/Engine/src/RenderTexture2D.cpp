#include "EnginePCH.h"
#include "RenderTexture2D.h"
#include "ResourceManagement.h"
#include "System.h"
#include "GraphicSystem.h"
#include "RenderTarget.h"

RenderTexture2D::RenderTexture2D(
	ResourceManagement* management, bool managed, const tstring& path, 
	RenderTarget* renderTarget) :
	Texture(management, managed, path),
	m_renderTarget(renderTarget)
{
}

RenderTexture2D::~RenderTexture2D()
{
	SafeDelete(m_renderTarget);
}

bool RenderTexture2D::Resize(unsigned int width, unsigned int height)
{
	return m_renderTarget->Resize(system->graphic->device, width, height);
}

Com<ID3D11Resource> RenderTexture2D::GetTexture() const
{
	return m_renderTarget->texture;
}

Com<ID3D11ShaderResourceView> RenderTexture2D::GetSRV() const
{
	return m_renderTarget->srv;
}

Com<ID3D11RenderTargetView> RenderTexture2D::GetRTV() const
{
	return m_renderTarget->rtv;
}

unsigned int RenderTexture2D::GetWidth() const
{
	return m_renderTarget->width;
}

unsigned int RenderTexture2D::GetHeight() const
{
	return m_renderTarget->height;
}

float RenderTexture2D::GetAspect() const
{
	return m_renderTarget->aspect;
}

ResourceRef<RenderTexture2D> RenderTexture2D::CreateRenderTexture2DM(
	ResourceManagement* management, const tstring& resourceKey, 
	unsigned int width, unsigned int height)
{
	if (!management)
		return nullptr;

	ResourceRef<ResourceObject> find = management->Find(resourceKey);
	if (find)
		return find;

	RenderTarget* renderTarget = nullptr;
	if (FAILED(RenderTarget::Create(management->system->graphic->device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &renderTarget)))
		return nullptr;

	return new RenderTexture2D(management, true, resourceKey, renderTarget);
}

ResourceRef<RenderTexture2D> RenderTexture2D::CreateRenderTexture2DUM(
	ResourceManagement* management, 
	unsigned int width, unsigned int height)
{
	if (!management)
		return nullptr;

	RenderTarget* renderTarget = nullptr;
	if (FAILED(RenderTarget::Create(management->system->graphic->device, width, height, false, DXGI_FORMAT_R32G32B32A32_FLOAT, &renderTarget)))
		return nullptr;

	return new RenderTexture2D(management, false, TEXT(""), renderTarget);
}

