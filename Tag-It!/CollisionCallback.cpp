#include "CollisionCallback.h"
#include "Input.h"

myListener::myListener()
	:b2ContactListener(), canJumpB(false) ,canJumpO(false)
{
}

//called by Box2D when two things begin colliding
void myListener::BeginContact(b2Contact* contact){
	//begin contact code



}

//called by Box2D just before the code for a collision is run, continues throughout the entire time it's colliding
void myListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold){
	//grab pointers to the fixtures
	b2Fixture* fixa = contact->GetFixtureA();
	b2Fixture* fixb = contact->GetFixtureB();

	//grab the user data in each fixture 
	int* uda = reinterpret_cast<int*>(fixa->GetBody()->GetUserData()); 
	int* udb = reinterpret_cast<int*>(fixb->GetBody()->GetUserData());
	
	//check if a player in a is colliding with a platform in b
	if ((*uda == 0 || *uda == 1) && *udb == 2) {
		//if so see if it needs to jump through
		jumpThrough(contact, fixa);
	}
	//check if a player in b is colliding with a platform in a
	else if ((*udb == 0 || *udb == 1) && *uda == 2) {
		//if so see if it needs to jump through
		jumpThrough(contact, fixb);
	}

	//check if the players are colliding with each other 
	if ((*uda == 0 && *udb == 1) || (*uda == 1 && *udb == 0)) {
		//if they are, let them pass through each other 
		contact->SetEnabled(false);
	}


	//if fixature a is a player and colliding with a platform or border in b
	if ((*uda == 0 || *uda == 1) && (*udb == 2 || *udb == 3)) {
		//send a's user data and velocity to see if we need to reset the jump
		b2Vec2 vel = fixa->GetBody()->GetLinearVelocity();
		jumpReset(uda);
	}
	//else if fixature b is a player and colliding with a platform or border in a
	else if ((*udb == 0 || *udb == 1) && (*uda == 2 || *uda == 3)) {
		//send b's user data and velocity to see if we need to reset the jump
		b2Vec2 vel = fixb->GetBody()->GetLinearVelocity();
		jumpReset(uda);
	}
}

void myListener::PostSolve(b2Contact* contact, b2ContactImpulse* impulse)
{
}

//set the status of wheter or not Blue can jump
void myListener::setJumpB(bool status)
{
	canJumpB = status;
}

//set the status of whether or not Orange can jump
void myListener::setJumpO(bool status)
{
	canJumpO = status;
}

//return the status of whether or not Blue can jump
bool myListener::getJumpB()
{
	return canJumpB;
}

//return the status of wheter or not Orange can jump
bool myListener::getJumpO()
{
	return canJumpO;
}

//called by Box2D when two objects stop colliding
void myListener::EndContact (b2Contact* contact){
	//end contact code
	//grab pointers to the fixtures
	b2Fixture* fixa = contact->GetFixtureA();
	b2Fixture* fixb = contact->GetFixtureB();

	//grab the userdata in each fixture 
	int* uda = reinterpret_cast<int*>(fixa->GetBody()->GetUserData());
	int* udb = reinterpret_cast<int*>(fixb->GetBody()->GetUserData());

	//if fixature a is a player and colliding with a platform or border in b
	if ((*uda == 0 || *uda == 1) && (*udb == 2 || *udb == 3)) {
		if (*uda == 0) canJumpB = false;
		else canJumpO = false;
	}
	//else if fixature b is a player and colliding with a platform or border in a
	else if ((*udb == 0 || *udb == 1) && (*uda == 2 || *uda == 3)) {
		if (*udb == 0) canJumpB = false;
		else canJumpO = false;
	}
}

void myListener::jumpThrough(b2Contact* contact, b2Fixture* playFix)
{
	//grab the velocity of the fixture
	b2Vec2 vel = playFix->GetBody()->GetLinearVelocity();

	//stop the player's hitbox from rotating  
	playFix->GetBody()->SetFixedRotation(true);

	//if the player is jumping
	if (vel.y > 0.0) {
		//stop the collision
		contact->SetEnabled(false);
	}
}

void myListener::jumpReset(int* ud)
{
	//if the player is landing 
	if (*ud == 0) {
		canJumpB = true;
	}
	else if (*ud == 1) {
		canJumpO = true;
	}
}







//fixa->GetBody()->GetPosition();
	//fixb->GetBody()->GetPosition();

	//const b2Body* bodyA = contact->GetFixtureA() ->GetBody();
	//const b2Body* bodyB = contact->GetFixtureB()->GetBody();
	//b2WorldManifold worldManifold;

	//b2Vec2 position = contact->GetFixtureA()->GetBody()->GetPosition();
	//b2Vec2 position2 = contact->GetFixtureB()->GetBody()->GetPosition();
	//contact->GetWorldManifold(&worldManifold);

	/*HelloWorld* scene = (HelloWorld*)Game::m_activeScene;
	scene->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetPosition();*/
	//std::cout << position <<"Position A";

	//if (((worldManifold.normal.x >= 80 && worldManifold.normal.x <= 170) && worldManifold.normal.y <= 50)  ) {
	//	contact->SetEnabled(false);
	//}