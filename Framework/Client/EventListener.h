#pragma once

class EventListener abstract
{
public:

	virtual ~EventListener() = default;

	virtual void OnEvent(const string& msg, void* pContext) = 0;
};

