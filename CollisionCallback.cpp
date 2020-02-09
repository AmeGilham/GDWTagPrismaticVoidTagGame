#include "CollisionCallback.h"

void myListener::BeginContact(b2Contact* contact)
{
	std::cout << "Begin collide\n"; 
}

void myListener::EndContact(b2Contact* contact)
{
	std::cout << "End collide\n";
}
