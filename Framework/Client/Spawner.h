#pragma once

#include "EditorWindow.h"

EDITOR_BEGIN

class Spawner : public EditorWindow
{
public:

	virtual void OnImGuiRender() override;

public:

	void SpawnDirectionalLight();
};

EDITOR_END
