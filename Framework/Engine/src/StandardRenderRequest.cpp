#include "EnginePCH.h"
#include "StandardRenderRequest.h"
#include "IRenderCuller.h"
#include "Material.h"

HRESULT StandardRenderRequest::IsRenderable(IRenderCuller* iCamera, bool* out_result)
{
	if (!iCamera || !out_result)
		return E_FAIL;

	*out_result = true;

	return S_OK;
}

HRESULT StandardRenderRequest::Render(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	if (!device || !deviceContext)
		return E_FAIL;

	if (!m_material)
		return E_FAIL;

	HRESULT hr = S_OK;

	if (FAILED(hr = m_material->SetInputLayout(m_techniqueIndex, m_passIndex)))
		return hr;

	if (FAILED(hr = m_material->ApplyPass(m_techniqueIndex, m_passIndex)))
		return hr;

	return S_OK;
}
