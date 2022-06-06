#pragma once

#include "IInput.h"
#include "Key.h"

// 업데이트 사이에서의 키 입력을 저장합니다.
// KeyDown 및 KeyUp 이 동시에 활성화 될 수 있음을 인지해야 합니다.

ENGINE_BEGIN

class ICamera;

class ENGINE_API Input : public IInput
{
public:

	Input();

	virtual ~Input() = default;

private:

	virtual bool Initialize(System* system) final override;

	virtual void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) final override;

	virtual void End() final override;

	virtual void Fetch() final override;

	virtual void SetEnable(bool value) final override;

public:

	bool GetKey(const Key& key);

	const bool& GetKeyDown(const Key& key);

	const bool& GetKeyUp(const Key& key);

	const float& GetMouseWheelDelta();

	// x: [0 ~ monitor width]
	// y: [1 ~ monitor height]
	// z: 0
	V3 GetMousePositionInScreen() const;

	// x: [0 ~ client width]
	// y: [0 ~ client height]
	// z: 0
	V3 GetMousePositionInViewport() const;

	// x: [-1 ~ +1]
	// y: [-1 ~ +1]
	// z: 0: The mouse is on surface of near plane
	// * The main camera is must exist
	V3 GetMousePositionInNDC() const;

	// Return mouse position on surface of near plane
	// * The main camera is must exist
	V3 GetMousePositionInViewSpace(ICamera* camera) const;

	// * The main camera is must exist
	Ray GetRayInWorldSpace(ICamera* camera) const;

	void Used(Key key);
	void UsedWheelDelta();

private:

	void SetKeyDown(unsigned char key);

	void SetKeyUp(unsigned char key);

	void SetWheelDelta(short wParamHiword);

private:

	bool		m_enable = true;

	bool		m_pressingState[0xFF];

	struct Buffer
	{
		bool DownState[0xFF];

		bool UpState[0xFF];

		float WheelDelta;
	};

	Buffer		m_capture = {};
	Buffer		m_frame = {};

	System*		m_system = nullptr;
};
ENGINE_END
