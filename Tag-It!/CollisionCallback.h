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
	//returns which player is currently it 
	int GetIt();
	//returns if the player's whose it has recently changed
	bool GetItChange();
	//sets if the person whose it has recently changed (so the trigger can be turned off when the animation has started)
	void SetItChange(bool change);
	//returns if the not it objective exists
	bool GetNotItObjExists();
	//sets if the not it objective exist
	void SetNotItObjExists(bool exists);

	void jumpThrough(b2Contact* contact, b2Fixture* playFix);
	void jumpReset(int* ud);
private:
	bool canJumpB;
	bool canJumpO; 
	//int tracking whose it, 0 is noone (start of a match), 1 is blue, 2 is orange
	int it = 0;
	//bool tracking if the person whose it has recently changed (so we can trigger an animation indicating whose it)
	bool itChange = false; 
	//bool tracking if the notitobjective has been destroyed (only set from the scene class, use it in combination with the itChange to delete the objective)
	bool NotItObjExists = true;
};