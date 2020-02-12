#pragma once

#include "PhysicsSystem.h"
//#include "PhysicsBody.h"

class myListener : public b2ContactListener {
public:
	void BeginContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, b2ContactImpulse impulse);



	void EndContact(b2Contact* contact);

private:

	
};