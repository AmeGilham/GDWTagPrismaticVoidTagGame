#pragma once

#include "PhysicsSystem.h"

class myListener : public b2ContactListener {
public:
	void BeginContact(b2Contact* contact);

	void EndContact(b2Contact* contact);
};