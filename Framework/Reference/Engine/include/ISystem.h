#pragma once

ENGINE_BEGIN
struct EngineWorldDesc;
class Input;
class CentralTimeElement;
class SceneManagement;
class PhysicsSystem;
class ResourceManagement;
class GraphicSystem;
class Times;
class ImGuiSystem;
class SoundSystem;
class ENGINE_API ISystem abstract
{
public:

	virtual ~ISystem() = default;

	virtual bool Initialize(EngineWorldDesc* desc, HWND hWnd, unsigned int width, unsigned int height) = 0;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

	virtual void Release() = 0;

	// ===================================================================

	virtual Input* GetInput() const = 0;

	virtual CentralTimeElement* GetTimeElement() const = 0;

	virtual SceneManagement* GetSceneManagement() const = 0;

	virtual PhysicsSystem* GetPhysicsSystem() const = 0;

	virtual ResourceManagement* GetResourceManagement() const = 0;

	virtual GraphicSystem* GetGraphicSystem() const = 0;

	virtual ImGuiSystem* GetImGuiSystem() const = 0;

	virtual SoundSystem* GetSoundSystem() const = 0;
};

ENGINE_END
