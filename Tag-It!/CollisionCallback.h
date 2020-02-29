#pragma once

//#include "BackEnd.h"
#include "PhysicsSystem.h"
//#include "PhysicsBody.h"

class myListener : public b2ContactListener {
public:
	myListener(); 

	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, b2ContactImpulse* impulse);

	void setJumpB(bool status);
	void setJumpO(bool status);

	bool getJumpB();
	bool getJumpO();


	void jumpThrough(b2Contact* contact, b2Fixture* playFix);
	void jumpReset(int* ud);
private:
	bool canJumpB;
	bool canJumpO; 
};