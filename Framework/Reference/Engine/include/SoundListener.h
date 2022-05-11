#pragma once

struct FMOD_SYSTEM;

ENGINE_BEGIN

class ENGINE_API ISoundListener
{
public:

	ISoundListener() = default;
	virtual ~ISoundListener() = default;

	virtual void SetIndex(int index) = 0;
};

class ENGINE_API SoundListener : public ISoundListener
{
private:

	SoundListener(FMOD_SYSTEM* fmodsystem, int index);
	virtual ~SoundListener() = default;

public:

	static Com<SoundListener> Create(FMOD_SYSTEM* fmodsystem, int index);

	ULONG AddRef();
	ULONG Release();
	inline ULONG GetRefCnt() const { return m_refCnt; }

	_declspec(property(get = GetRefCnt)) ULONG refCnt;

public:

	void Update(const V3& position, const V3& velocity, const V3& forward, const V3& up);

	int GetIndex() const { return m_index; }
	FMOD_SYSTEM* GetFMODSystem() const { return m_fmodsystem; }

	_declspec(property(get = GetIndex)) int index;
	_declspec(property(get = GetFMODSystem)) FMOD_SYSTEM* fmodsystem;

private:

	void SetIndex(int index) { m_index = index; }

private:

	ULONG			m_refCnt = 0;
	int				m_index;
	FMOD_SYSTEM*	m_fmodsystem;
};

ENGINE_END
