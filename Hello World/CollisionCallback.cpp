#include "CollisionCallback.h"


void myListener::BeginContact(b2Contact* contact){
	std::cout << "Begin collide\n";



}

void myListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
	std::cout << "Begin pre collide\n";

	b2Fixture* fixa = contact->GetFixtureA();
	b2Fixture* fixb = contact->GetFixtureB();

	b2Vec2 vel = fixa->GetBody()->GetLinearVelocity();

	//if (vel.y > 0) {
	//	contact->SetEnabled(false);
	//	std::cout << "Begin HELPPPPPP\n";

	//}
	/*if (vel2.y > 0) {
		contact->SetEnabled(false);
	}*/
	//(((fixa->GetBody()->GetPosition().x >= 22 && fixa->GetBody()->GetPosition().x <= 115) && fixa->GetBody()->GetPosition().y <= 30)) && 
	if (vel.y > 0.0) {
		contact->SetEnabled(false);
		std::cout << contact->IsEnabled();

		std::cout << "Begin pre HELPPPPPP\n";}
	else{
		contact->SetEnabled(true);}


}

void myListener::EndContact (b2Contact* contact){
	std::cout << contact->IsEnabled();
	std::cout << "End collide\n";
	contact->SetEnabled(false);

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