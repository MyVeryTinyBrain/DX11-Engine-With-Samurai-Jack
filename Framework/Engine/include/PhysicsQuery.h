#pragma once

ENGINE_BEGIN

class ENGINE_API PhysicsQuery
{
public:

	PhysicsQuery(PxScene* pxScene);

	~PhysicsQuery() = default;

private:

	PxScene*		m_scene = nullptr;
};

ENGINE_END
