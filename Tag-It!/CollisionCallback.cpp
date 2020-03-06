#include "CollisionCallback.h"
#include "Input.h"

myListener::myListener()
	:b2ContactListener(), canJumpB(false) ,canJumpO(false)
{
}

//called by Box2D when two things begin colliding
void myListener::BeginContact(b2Contact* contact){
	//begin contact code
	//check if it's the first phase of the game and noone is it yet
	if (it == 0) {
		//if it is check if the collision is between a player and the objective to become the first "Not it" of the match
		//grab pointers to the fixtures
		b2Fixture* fixa = contact->GetFixtureA();
		b2Fixture* fixb = contact->GetFixtureB();

		//grab the user data in each fixture 
		int* uda = reinterpret_cast<int*>(fixa->GetBody()->GetUserData());
		int* udb = reinterpret_cast<int*>(fixb->GetBody()->GetUserData());

		//check if blue is in fixa, and is colliding with the NotItObjective in fixb, or if she's in fixb and colliding with the objective in fixa
		if ((*uda == 0 && *udb == 5) || (*uda == 5 && *udb == 0)) {
			//if she is, then make orange it
			it = 2;
			//and destroy the not it objective 
			itChange = true;
		}
		//otherwise check if orange is in fixb, and is colliding with the NotItObjective in fixa, or if he's in fixb and collding with the objective in fixa
		else if ((*uda == 1 && *udb == 5) || (*uda == 5 && *udb == 1)) {
			//if he is, then make blue it
			it = 1;
			//and destroy the not it objective 
			itChange = true;
		}
	}

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

	if (((*udb == 1  && *uda == 4)) || (*uda == 1 && *udb == 4)) { //if orange collides with blue tag box
		if (Input::GetKey(Key::Q)) {
			if (it == 1) {
				//if she is, then make orange it
				it = 2;
				//and destroy the not it objective 
				itChange = true;
				std::cout << "Hello";
			}
			//if orange is it 
			else if (it == 2) {
			
			}
		}

		contact->SetEnabled(false);
	}
	
	if (((*udb == 0 && *uda == 6)) || (*uda == 0 && *udb == 6)) { //if blue collides with orange tag box

		if (Input::GetKey(Key::M)) {
			if (it == 1) {//notihng happens to blue
			
			}
			//if orange is it 
			else if (it == 2) {
				//if she is, then make blue it
				it = 1;
				//and destroy the not it objective 
				itChange = true;
				std::cout << "Hello";
			}
		}

		contact->SetEnabled(false);
	}


	//check if the players are colliding with each other 
	if ((*uda == 0 && *udb == 1) || (*uda == 1 && *udb == 0)) {
		if (Input::GetKey(Key::M)) { //player 2

			if (Timer::time >= 1.5f) { //cooldown timer for tagging (1.5 seconds before palyer can tag the other player)
				Timer::Reset();
				Timer::Update();

				// if blue is it
				if (it == 1) {

				}

				//if orange is it 
				else if (it == 2) {

				

				}

			}

		}

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

//return the player whose currently it 
int myListener::GetIt(){
	return it;
}

bool myListener::GetItChange(){
	return itChange;
}

void myListener::SetItChange(bool change){
	itChange = change;
}

bool myListener::GetNotItObjExists(){
	return NotItObjExists;
}

void myListener::SetNotItObjExists(bool exists){
	NotItObjExists = exists;
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

void myListener::jumpThrough(b2Contact* contact, b2Fixture* playFix){
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

void myListener::jumpReset(int* ud){
	//if the player is landing 
	if (*ud == 0) {
		canJumpB = true;
	}
	else if (*ud == 1) {
		canJumpO = true;
	}
}

