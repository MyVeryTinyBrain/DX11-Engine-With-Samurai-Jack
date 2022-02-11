#include "EnginePCH.h"
#include "RenderRequestInput.h"

bool RenderRequestEssential::IsValid() const
{
	if ((uint)renderGroup >= (uint)RenderGroup::Max)
		return false;

	if (!material || !mesh)
		return false;

	size_t techniqueCount = 0;
	if (FAILED(material->GetTechniqueCount(techniqueCount)))
		return false;
	if (techniqueIndex >= techniqueCount)
		return false;

	size_t passCount = 0;
	if (FAILED(material->GetPassCount(techniqueIndex, passCount)))
		return false;
	if (passIndex >= passCount)
		return false;

	if (layerIndex >= 32)
		return false;

	return true;
}
