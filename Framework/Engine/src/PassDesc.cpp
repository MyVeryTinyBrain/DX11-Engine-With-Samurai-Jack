#include "EnginePCH.h"
#include "PassDesc.h"
#include "VIType.h"

PassDesc::PassDesc(
	ID3DX11EffectPass* pass, ID3D11InputLayout* inputLayout, 
	const string& name, const vector<string>& sementics, 
	RenderGroup renderGroup, int renderGroupOrder,
	bool cullingFlag, bool instancingFlag, 
	bool drawShadowFlag, bool shadowPassFlag)
{
	m_pass = pass;
	m_inputLayout = inputLayout;
	m_name = name;
	m_sementics = sementics;
	m_renderGroup = renderGroup;
	m_renderGroupOrder = renderGroupOrder;
	m_isCulling = cullingFlag;
	m_isInstancing = instancingFlag;
	m_isDrawingShadow = drawShadowFlag;
	m_isShadowPass = shadowPassFlag;
}

PassDesc::~PassDesc()
{
	SafeRelease(m_inputLayout);

	SafeRelease(m_pass);
}

bool PassDesc::CreateInputElements(ID3DX11EffectPass* pass, D3D11_INPUT_ELEMENT_DESC** out_arrElements, uint* out_count)
{
	if (!pass || !out_arrElements || !out_count)
		return false;

	D3DX11_PASS_SHADER_DESC vertexShade;
	if (FAILED(pass->GetVertexShaderDesc(&vertexShade)))
		return false;

	ID3DX11EffectShaderVariable* vertexShader = vertexShade.pShaderVariable;
	if (!vertexShader->IsValid())
		return false;

	D3DX11_EFFECT_SHADER_DESC vertexShaderDesc = {};
	if (FAILED(vertexShader->GetShaderDesc(0, &vertexShaderDesc)))
		return false;

	vector<D3D11_INPUT_ELEMENT_DESC> vecElements;
	for (uint32_t i = 0; i < vertexShaderDesc.NumInputSignatureEntries; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC inputSignatureElementDesc = {};

		// D3D11_SIGNATURE_PARAMETER_DESC를 찾지 못했다면 스킵합니다.
		if (FAILED(vertexShader->GetInputSignatureElementDesc(vertexShade.ShaderIndex, i, &inputSignatureElementDesc)))
			continue;

		// 시멘틱 인덱스가 없거나 0이 아니라면 스킵합니다.
		else if (inputSignatureElementDesc.SemanticIndex != 0)
			continue;

		D3D11_INPUT_ELEMENT_DESC element = {};
		bool isValidElement = true;

		element.SemanticName = inputSignatureElementDesc.SemanticName;
		element.SemanticIndex = inputSignatureElementDesc.SemanticIndex;

		if (!strcmp(element.SemanticName, Vertex::PositionName()))
		{
			element.AlignedByteOffset = Vertex::PositionPosition();
			element.Format = Vertex::PositionFormat();
			element.InputSlotClass = Vertex::InputSlotClass();
			element.InputSlot = Vertex::InputSlot();
			element.InstanceDataStepRate = Vertex::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, Vertex::NormalName()))
		{
			element.AlignedByteOffset = Vertex::NormalPosition();
			element.Format = Vertex::NormalFormat();
			element.InputSlotClass = Vertex::InputSlotClass();
			element.InputSlot = Vertex::InputSlot();
			element.InstanceDataStepRate = Vertex::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, Vertex::TangentName()))
		{
			element.AlignedByteOffset = Vertex::TangentPosition();
			element.Format = Vertex::TangentFormat();
			element.InputSlotClass = Vertex::InputSlotClass();
			element.InputSlot = Vertex::InputSlot();
			element.InstanceDataStepRate = Vertex::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, Vertex::BiNormalName()))
		{
			element.AlignedByteOffset = Vertex::BiNormalPosition();
			element.Format = Vertex::BiNormalFormat();
			element.InputSlotClass = Vertex::InputSlotClass();
			element.InputSlot = Vertex::InputSlot();
			element.InstanceDataStepRate = Vertex::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, Vertex::ColorName()))
		{
			element.AlignedByteOffset = Vertex::ColorPosition();
			element.Format = Vertex::ColorFormat();
			element.InputSlotClass = Vertex::InputSlotClass();
			element.InputSlot = Vertex::InputSlot();
			element.InstanceDataStepRate = Vertex::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, Vertex::UVWName()))
		{
			element.AlignedByteOffset = Vertex::UVWPosition();
			element.Format = Vertex::UVWFormat();
			element.InputSlotClass = Vertex::InputSlotClass();
			element.InputSlot = Vertex::InputSlot();
			element.InstanceDataStepRate = Vertex::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, Vertex::BlendIndicesName()))
		{
			element.AlignedByteOffset = Vertex::BlendIndicesPosition();
			element.Format = Vertex::BlendIndicesFormat();
			element.InputSlotClass = Vertex::InputSlotClass();
			element.InputSlot = Vertex::InputSlot();
			element.InstanceDataStepRate = Vertex::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, Vertex::BlendWeightName()))
		{
			element.AlignedByteOffset = Vertex::BlendWeightPosition();
			element.Format = Vertex::BlendWeightFormat();
			element.InputSlotClass = Vertex::InputSlotClass();
			element.InputSlot = Vertex::InputSlot();
			element.InstanceDataStepRate = Vertex::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, InstanceData::RightName()))
		{
			element.AlignedByteOffset = InstanceData::RightPosition();
			element.Format = InstanceData::RightFormat();
			element.InputSlotClass = InstanceData::InputSlotClass();
			element.InputSlot = InstanceData::InputSlot();
			element.InstanceDataStepRate = InstanceData::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, InstanceData::UpName()))
		{
			element.AlignedByteOffset = InstanceData::UpPosition();
			element.Format = InstanceData::UpFormat();
			element.InputSlotClass = InstanceData::InputSlotClass();
			element.InputSlot = InstanceData::InputSlot();
			element.InstanceDataStepRate = InstanceData::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, InstanceData::ForwardName()))
		{
			element.AlignedByteOffset = InstanceData::ForwardPosition();
			element.Format = InstanceData::ForwardFormat();
			element.InputSlotClass = InstanceData::InputSlotClass();
			element.InputSlot = InstanceData::InputSlot();
			element.InstanceDataStepRate = InstanceData::InstanceDataStepRate();
		}
		else if (!strcmp(element.SemanticName, InstanceData::PositionName()))
		{
			element.AlignedByteOffset = InstanceData::PositionPosition();
			element.Format = InstanceData::PositionFormat();
			element.InputSlotClass = InstanceData::InputSlotClass();
			element.InputSlot = InstanceData::InputSlot();
			element.InstanceDataStepRate = InstanceData::InstanceDataStepRate();
		}
		else
		{
			isValidElement = false;
		}

		if (isValidElement)
			vecElements.push_back(element);
	}

	D3D11_INPUT_ELEMENT_DESC* elements = new D3D11_INPUT_ELEMENT_DESC[vecElements.size()]{};
	copy(vecElements.begin(), vecElements.end(), elements);

	*out_arrElements = elements;
	*out_count = uint(vecElements.size());

	return true;
}

ID3D11InputLayout* PassDesc::CreateInputLayout(Com<ID3D11Device> device, ID3DX11EffectPass* pass, string& out_name, vector<string>& out_sementics)
{
	if (!device || !pass)
		return nullptr;

	D3D11_INPUT_ELEMENT_DESC* arrElements = nullptr;
	uint elementCount = 0;
	if (!CreateInputElements(pass, &arrElements, &elementCount))
		return nullptr;

	auto ReleaseVars = [&]()
	{
		SafeDeleteArray(arrElements);
	};

	D3DX11_PASS_DESC passDesc;
	if (FAILED(pass->GetDesc(&passDesc)))
	{
		ReleaseVars();
		return nullptr;
	}

	ID3D11InputLayout* inputLayout = nullptr;
	if (FAILED(device->CreateInputLayout(
		arrElements,
		UINT(elementCount),
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&inputLayout)))
	{
		ReleaseVars();
		return nullptr;
	}

	out_name = passDesc.Name;

	out_sementics.reserve(elementCount);
	for (uint i = 0; i < elementCount; ++i)
		out_sementics.push_back(arrElements->SemanticName);

	ReleaseVars();
	return inputLayout;
}

bool PassDesc::ExtractRenderGroup(ID3DX11EffectPass* pass, RenderGroup* out_renderGroup)
{
	if (!pass || !out_renderGroup)
		return false;

	*out_renderGroup = RenderGroup::Standard;

	D3DX11_PASS_DESC passDesc = {};
	if (FAILED(pass->GetDesc(&passDesc)))
		return false;

	ID3DX11EffectStringVariable* hRenderGroup = nullptr;

	// find annotation <string rendergroup>
	for (uint32_t i = 0; i < passDesc.Annotations; ++i)
	{
		ID3DX11EffectVariable* annotation = pass->GetAnnotationByIndex(i);

		if (!annotation->IsValid())
			continue;

		D3DX11_EFFECT_VARIABLE_DESC annotationDesc = {};
		if (FAILED(annotation->GetDesc(&annotationDesc)))
			continue;

		string name = annotationDesc.Name;
		std::transform(name.begin(), name.end(), name.begin(), std::tolower);
		if (name == "rendergroup" && annotation->AsString()->IsValid())
		{
			hRenderGroup = annotation->AsString();
			break;
		}

		SafeRelease(annotation);
	}

	if (!hRenderGroup)
		return true;

	LPCSTR lpcstrRenderGroup;
	if (FAILED(hRenderGroup->GetString(&lpcstrRenderGroup)))
		return false;

	string strRenderGroup = lpcstrRenderGroup;
	std::transform(strRenderGroup.begin(), strRenderGroup.end(), strRenderGroup.begin(), std::tolower);
	if (strRenderGroup == "priority")
	{
		*out_renderGroup = RenderGroup::Priority;
	}
	else if (strRenderGroup == "standard")
	{
		*out_renderGroup = RenderGroup::Standard;
	}
	else if (strRenderGroup == "alphatest")
	{
		*out_renderGroup = RenderGroup::AlphaTest;
	}
	else if (strRenderGroup == "transparent")
	{
		*out_renderGroup = RenderGroup::Transparent;
	}
	else if (strRenderGroup == "overlay")
	{
		*out_renderGroup = RenderGroup::Overlay;
	}
	else
	{
		return false;
	}

	SafeRelease(hRenderGroup);

	return true;
}

bool PassDesc::ExtractRenderGroupOrder(ID3DX11EffectPass* pass, int* out_renderGroupOrder)
{
	if (!pass || !out_renderGroupOrder)
		return false;

	*out_renderGroupOrder = 0;

	D3DX11_PASS_DESC passDesc = {};
	if (FAILED(pass->GetDesc(&passDesc)))
		return false;

	ID3DX11EffectScalarVariable* hRenderGroupOrder = nullptr;

	// find annotation <int rendergrouporder>
	for (uint32_t i = 0; i < passDesc.Annotations; ++i)
	{
		ID3DX11EffectVariable* annotation = pass->GetAnnotationByIndex(i);

		if (!annotation->IsValid())
			continue;

		D3DX11_EFFECT_VARIABLE_DESC annotationDesc = {};
		if (FAILED(annotation->GetDesc(&annotationDesc)))
			continue;

		string name = annotationDesc.Name;
		std::transform(name.begin(), name.end(), name.begin(), std::tolower);
		if (name == "rendergrouporder" && annotation->AsScalar()->IsValid())
		{
			hRenderGroupOrder = annotation->AsScalar();
			break;
		}

		SafeRelease(annotation);
	}

	if (!hRenderGroupOrder)
		return true;

	int renderGroupOrder = 0;
	if (FAILED(hRenderGroupOrder->GetInt(&renderGroupOrder)))
		return false;

	*out_renderGroupOrder = renderGroupOrder;

	SafeRelease(hRenderGroupOrder);

	return true;
}

bool PassDesc::ExtractCullingFlag(ID3DX11EffectPass* pass, bool* out_flag)
{
	if (!pass || !out_flag)
		return false;

	*out_flag = false;

	D3DX11_PASS_DESC passDesc = {};
	if (FAILED(pass->GetDesc(&passDesc)))
		return false;

	ID3DX11EffectScalarVariable* hCullingFlag = nullptr;

	// find annotation <bool instancing>
	for (uint32_t i = 0; i < passDesc.Annotations; ++i)
	{
		ID3DX11EffectVariable* annotation = pass->GetAnnotationByIndex(i);

		if (!annotation->IsValid())
			continue;

		D3DX11_EFFECT_VARIABLE_DESC annotationDesc = {};
		if (FAILED(annotation->GetDesc(&annotationDesc)))
			continue;

		string name = annotationDesc.Name;
		std::transform(name.begin(), name.end(), name.begin(), std::tolower);
		if (name == "cull" && annotation->AsScalar()->IsValid())
		{
			hCullingFlag = annotation->AsScalar();
			break;
		}

		SafeRelease(annotation);
	}

	if (!hCullingFlag)
		return true;

	bool flag = 0;
	if (FAILED(hCullingFlag->GetBool(&flag)))
		return false;

	*out_flag = flag;

	SafeRelease(hCullingFlag);

	return true;
}

bool PassDesc::ExtractInstancingFlag(ID3DX11EffectPass* pass, bool* out_flag)
{
	if (!pass || !out_flag)
		return false;

	*out_flag = false;

	D3DX11_PASS_DESC passDesc = {};
	if (FAILED(pass->GetDesc(&passDesc)))
		return false;

	ID3DX11EffectScalarVariable* hInstancingFlag = nullptr;

	// find annotation <bool instancing>
	for (uint32_t i = 0; i < passDesc.Annotations; ++i)
	{
		ID3DX11EffectVariable* annotation = pass->GetAnnotationByIndex(i);

		if (!annotation->IsValid())
			continue;

		D3DX11_EFFECT_VARIABLE_DESC annotationDesc = {};
		if (FAILED(annotation->GetDesc(&annotationDesc)))
			continue;

		string name = annotationDesc.Name;
		std::transform(name.begin(), name.end(), name.begin(), std::tolower);
		if (name == "instancing" && annotation->AsScalar()->IsValid())
		{
			hInstancingFlag = annotation->AsScalar();
			break;
		}

		SafeRelease(annotation);
	}

	if (!hInstancingFlag)
		return true;

	bool flag = 0;
	if (FAILED(hInstancingFlag->GetBool(&flag)))
		return false;

	*out_flag = flag;

	SafeRelease(hInstancingFlag);

	return true;
}

bool PassDesc::ExtractDrawShadowFlag(ID3DX11EffectPass* pass, bool* out_flag)
{
	if (!pass || !out_flag)
		return false;

	D3DX11_PASS_DESC passDesc = {};
	if (FAILED(pass->GetDesc(&passDesc)))
		return false;

	ID3DX11EffectScalarVariable* hDrawShadowFlag = nullptr;

	// find annotation <bool drawshadow>
	for (uint32_t i = 0; i < passDesc.Annotations; ++i)
	{
		ID3DX11EffectVariable* annotation = pass->GetAnnotationByIndex(i);

		if (!annotation->IsValid())
			continue;

		D3DX11_EFFECT_VARIABLE_DESC annotationDesc = {};
		if (FAILED(annotation->GetDesc(&annotationDesc)))
			continue;

		string name = annotationDesc.Name;
		std::transform(name.begin(), name.end(), name.begin(), std::tolower);
		if (name == "drawshadow" && annotation->AsScalar()->IsValid())
		{
			hDrawShadowFlag = annotation->AsScalar();
			break;
		}

		SafeRelease(annotation);
	}

	if (!hDrawShadowFlag)
		return true;

	bool flag = 0;
	if (FAILED(hDrawShadowFlag->GetBool(&flag)))
		return false;

	*out_flag = flag;

	SafeRelease(hDrawShadowFlag);

	return true;
}

bool PassDesc::ExtractShadowPassFlag(ID3DX11EffectPass* pass, bool* out_flag)
{
	if (!pass || !out_flag)
		return false;

	D3DX11_PASS_DESC passDesc = {};
	if (FAILED(pass->GetDesc(&passDesc)))
		return false;

	ID3DX11EffectScalarVariable* hShadowPassFlag = nullptr;

	// find annotation <bool shadowpass>
	for (uint32_t i = 0; i < passDesc.Annotations; ++i)
	{
		ID3DX11EffectVariable* annotation = pass->GetAnnotationByIndex(i);

		if (!annotation->IsValid())
			continue;

		D3DX11_EFFECT_VARIABLE_DESC annotationDesc = {};
		if (FAILED(annotation->GetDesc(&annotationDesc)))
			continue;

		string name = annotationDesc.Name;
		std::transform(name.begin(), name.end(), name.begin(), std::tolower);
		if (name == "shadowpass" && annotation->AsScalar()->IsValid())
		{
			hShadowPassFlag = annotation->AsScalar();
			break;
		}

		SafeRelease(annotation);
	}

	if (!hShadowPassFlag)
		return true;

	bool flag = 0;
	if (FAILED(hShadowPassFlag->GetBool(&flag)))
		return false;

	*out_flag = flag;

	SafeRelease(hShadowPassFlag);

	return true;
}

PassDesc* PassDesc::CreatePassDesc(Com<ID3D11Device> device, ID3DX11EffectPass* pass, tstring& out_error)
{
	if (!device || !pass)
		return nullptr;

	return CreateDefaultPassDesc(device, pass);
}

PassDesc* PassDesc::CreateDefaultPassDesc(Com<ID3D11Device> device, ID3DX11EffectPass* pass)
{
	if (!device || !pass)
		return nullptr;

	RenderGroup renderGroup = RenderGroup::Standard;
	if (!ExtractRenderGroup(pass, &renderGroup))
		return nullptr;

	int renderGroupOrder = 0;
	if (!ExtractRenderGroupOrder(pass, &renderGroupOrder))
		return nullptr;

	bool cullingFlag = true;
	if (!ExtractCullingFlag(pass, &cullingFlag))
		return nullptr;

	bool instancingFlag = false;
	if (!ExtractInstancingFlag(pass, &instancingFlag))
		return nullptr;

	bool drawShadow = renderGroup == RenderGroup::Standard ? true : false;
	if (!ExtractDrawShadowFlag(pass, &drawShadow))
		return nullptr;

	bool shadowPass = false;
	if (!ExtractShadowPassFlag(pass, &shadowPass))
		return nullptr;

	string name;
	vector<string> sementics;
	ID3D11InputLayout* inputLayout = CreateInputLayout(device, pass, name, sementics);
	if (!inputLayout)
		return nullptr;

	return new PassDesc(
		pass, inputLayout, 
		name, sementics, 
		renderGroup, renderGroupOrder, 
		cullingFlag, instancingFlag, 
		drawShadow, shadowPass);
}
