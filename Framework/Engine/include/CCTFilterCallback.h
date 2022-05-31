#pragma once

ENGINE_BEGIN

class CharacterController;
class PhysicsLayerManager;

class ENGINE_API CCTFilterCallback abstract
{
public:

	static bool static_cct_filtercallback(PhysicsLayerManager* layerManager, const PxController& a, const PxController& b);
};

ENGINE_END
