#include "Box2D/Box2D.h"
#ifndef IFORCE2D_ONEWAYWALLS_DEMO_H
#define IFORCE2D_ONEWAYWALLS_DEMO_H

#ifndef DEGTORAD
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#endif

enum _keyState {
	MS_LEFT = 0x1,
	MS_RIGHT = 0x2,
	MS_JUMP = 0x4
};

class iforce2d_OneWayWalls_demo : public Test
{
public:

	b2Body* setupOneWayWall(b2BodyType type, b2Vec2 position, float angle, int shrinkBy = 1)
	{
		//setup platform shape for reuse
		b2PolygonShape polygonShape;
		b2Vec2 verts[5];
		verts[0].Set(0, -0.75);
		verts[1].Set(2.5, -0.5);
		verts[2].Set(2.5, 0.5);
		verts[3].Set(-2.5, 0.5);
		verts[4].Set(-2.5, -0.5);
		for (int i = 0; i < 5; i++)
			verts[i] *= 1 / (float)shrinkBy;
		polygonShape.Set(verts, 5);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &polygonShape;
		fixtureDef.density = 1;
		fixtureDef.friction = 0.8f;

		b2BodyDef bodyDef;
		bodyDef.type = type;
		bodyDef.position = position;
		bodyDef.angle = angle;
		b2Body* body = m_world->CreateBody(&bodyDef);
		b2Fixture* fixture = body->CreateFixture(&fixtureDef);
		fixture->SetUserData((void*)shrinkBy); //mark this as a small one-sided wall of a certain size
		return body;
	}

	// http://www.iforce2d.net/b2dtut/projected-trajectory
	float calculateVerticalVelocityForHeight(float desiredHeight)
	{
		if (desiredHeight <= 0)
			return 0; //wanna go down? just let it drop

		//gravity is given per second but we want time step values here
		float t = 1 / 60.0f;
		b2Vec2 stepGravity = t * t * m_world->GetGravity(); // m/s/s

		//quadratic equation setup (ax² + bx + c = 0)
		float a = 0.5f / stepGravity.y;
		float b = 0.5f;
		float c = desiredHeight;

		//check both possible solutions
		float quadraticSolution1 = (-b - b2Sqrt(b * b - 4 * a * c)) / (2 * a);
		float quadraticSolution2 = (-b + b2Sqrt(b * b - 4 * a * c)) / (2 * a);

		//use the one which is positive
		float v = quadraticSolution1;
		if (v < 0)
			v = quadraticSolution2;

		//convert answer back to seconds
		return v * 60.0f;
	}

	iforce2d_OneWayWalls_demo()
	{
		b2BodyDef groundBodyDef;
		b2Body* groundBody = m_world->CreateBody(&groundBodyDef);

		{//boundary fence
			b2PolygonShape polygonShape;
			polygonShape.SetAsBox(20, 1, b2Vec2(0, -1), 0);//ground
			groundBody->CreateFixture(&polygonShape, 0)->SetFriction(0.8);
			polygonShape.SetAsBox(20, 1, b2Vec2(0, 40), 0);//ceiling
			groundBody->CreateFixture(&polygonShape, 0)->SetFriction(0.8);
			polygonShape.SetAsBox(1, 20, b2Vec2(-20, 20), 0);//left wall
			groundBody->CreateFixture(&polygonShape, 0)->SetFriction(0.8);
			polygonShape.SetAsBox(1, 20, b2Vec2(20, 20), 0);//right wall
			groundBody->CreateFixture(&polygonShape, 0)->SetFriction(0.8);
		}

		//static platforms to make a little maze
		setupOneWayWall(b2_staticBody, b2Vec2(15, 12.55f), 0 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-15, 2.5f), 270 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-15, 7.5f), 90 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-5, 7.5f), 90 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(10, 7.5f), 90 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-10, 2.5f), 90 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(15, 2.5f), -25 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(15, 7.5f), 25 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(1, 2.5f), 90 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-12.5, 5), 0 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-7.5, 5), 180 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-2.5, 5), 0 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(2.5, 5), 0 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-12.5, 10), 180 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-7.5, 10), 180 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-2.5, 10), 180 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(2.5, 10), 180 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(7.5, 10), 180 * DEGTORAD);

		setupOneWayWall(b2_staticBody, b2Vec2(6.5, 27), 0 * DEGTORAD);
		setupOneWayWall(b2_staticBody, b2Vec2(-16.5, 27), 0 * DEGTORAD);

		{//cart on prismatic joint
			b2Body* cartBody = setupOneWayWall(b2_dynamicBody, b2Vec2(1.49, 27), 0 * DEGTORAD);
			b2Body* cartEdge1 = setupOneWayWall(b2_dynamicBody, b2Vec2(1.49 + 2.375, 27.5 + (2.5 / 4.0f)), 90 * DEGTORAD, 4);
			b2Body* cartEdge2 = setupOneWayWall(b2_dynamicBody, b2Vec2(1.49 - 2.375, 27.5 + (2.5 / 4.0f)), 270 * DEGTORAD, 4);

			b2WeldJointDef wJointDef;
			wJointDef.Initialize(cartBody, cartEdge1, cartEdge1->GetPosition());
			m_world->CreateJoint(&wJointDef);
			wJointDef.Initialize(cartBody, cartEdge2, cartEdge2->GetPosition());
			m_world->CreateJoint(&wJointDef);

			b2PrismaticJointDef jointDef;
			jointDef.collideConnected = true;
			jointDef.bodyA = groundBody;
			jointDef.bodyB = cartBody;
			jointDef.localAnchorA.Set(1.49, 27);
			jointDef.localAnchorB.SetZero();
			jointDef.localAxisA.Set(-1, 0);
			jointDef.enableLimit = true;
			jointDef.lowerTranslation = 0;
			jointDef.upperTranslation = 11.5 + 1.5 - 0.02;
			m_world->CreateJoint(&jointDef);
		}

		//kinematic bodies for moving platforms
		m_platformBody = setupOneWayWall(b2_kinematicBody, b2Vec2(15, 15), 0);
		m_platformBody2 = setupOneWayWall(b2_kinematicBody, b2Vec2(15, 20), 0);

		//kinematic body for rotating floor section
		m_rotatingFloor = setupOneWayWall(b2_kinematicBody, b2Vec2(7.5f, 5), 0);
		m_rotatingFloorTimer = 0;
		m_rotatingFloorTurnCount = 0;

		{//swinging wall
			b2Body* swingDoor = setupOneWayWall(b2_dynamicBody, b2Vec2(-5, 2.5), 90 * DEGTORAD);

			b2RevoluteJointDef jointDef;
			jointDef.bodyA = groundBody;
			jointDef.bodyB = swingDoor;
			jointDef.localAnchorA.Set(-5, 5);
			jointDef.localAnchorB.Set(2.5, 0);
			m_world->CreateJoint(&jointDef);
		}

		{//swing bridge
			b2RevoluteJointDef jointDef;
			jointDef.localAnchorA.Set(0.5, 0);
			b2Body* lastChainPiece = NULL;
			for (int i = 0; i < 10; i++) {
				b2Body* chainPiece = setupOneWayWall(b2_dynamicBody, b2Vec2(9.5 + i, 27.5), 0, 5);
				if (lastChainPiece) {
					jointDef.bodyA = lastChainPiece;
					jointDef.bodyB = chainPiece;
					jointDef.localAnchorA.Set(0.5, 0);
					jointDef.localAnchorB.Set(-0.5, 0);
					m_world->CreateJoint(&jointDef);
				}
				else {
					jointDef.bodyA = groundBody;
					jointDef.bodyB = chainPiece;
					jointDef.localAnchorA.Set(9, 27.375);
					jointDef.localAnchorB.Set(-0.5, 0);
					m_world->CreateJoint(&jointDef);
				}
				lastChainPiece = chainPiece;
			}
			jointDef.localAnchorA.Set(19, 27.5);
			jointDef.localAnchorB.Set(0.5, 0);
			jointDef.bodyA = groundBody;
			jointDef.bodyB = lastChainPiece;
			m_world->CreateJoint(&jointDef);
		}

		//player
		{
			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.fixedRotation = true;
			bodyDef.position.Set(-17.5, 1.25f);

			b2PolygonShape polygonShape;
			polygonShape.SetAsBox(0.5f, 0.75f);
			m_playerBody = m_world->CreateBody(&bodyDef);
			m_playerBody->CreateFixture(&polygonShape, 1);

			b2CircleShape circleShape;
			circleShape.m_radius = 0.5f;
			bodyDef.position.Set(-17.5, 0.5f);
			m_playerFootBody = m_world->CreateBody(&bodyDef);
			b2Fixture* footFixture = m_playerFootBody->CreateFixture(&circleShape, 1);
			footFixture->SetUserData((void*)100);//mark fixture as player foot

			b2RevoluteJointDef jointDef;
			jointDef.bodyA = m_playerBody;
			jointDef.bodyB = m_playerFootBody;
			jointDef.localAnchorA.Set(0, -0.75f);
			jointDef.localAnchorB.Set(0, 0);
			m_world->CreateJoint(&jointDef);
			m_numFootContacts = 0;
			m_keyState = 0;
			m_jumpTimeout = 0;
		}
	}

	void BeginContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		//check if one of the fixtures is single-sided
		b2Fixture* platformFixture = NULL;
		b2Fixture* otherFixture = NULL;
		int platformScale = 1;
		int fudA = (int)fixtureA->GetUserData();
		int fudB = (int)fixtureB->GetUserData();
		bool fixtureAIsPlatform = (fudA && fudA < 100);
		bool fixtureBIsPlatform = (fudB && fudB < 100);
		if (fixtureAIsPlatform && fixtureBIsPlatform) {
			contact->SetEnabled(false);//avoids problems with swinging wall
			return;
		}
		else if (fixtureAIsPlatform) {
			platformFixture = fixtureA;
			platformScale = fudA;
			otherFixture = fixtureB;
		}
		else if (fixtureBIsPlatform) {
			platformFixture = fixtureB;
			platformScale = fudB;
			otherFixture = fixtureA;
		}

		bool solid = true;

		if (platformFixture) {
			int numPoints = contact->GetManifold()->pointCount;
			b2WorldManifold worldManifold;
			contact->GetWorldManifold(&worldManifold);

			b2Body* platformBody = platformFixture->GetBody();
			b2Body* otherBody = otherFixture->GetBody();

			//check if contact points are moving into platform
			solid = false;
			for (int i = 0; i < numPoints; i++) {
				b2Vec2 pointVelPlatform =
					platformBody->GetLinearVelocityFromWorldPoint(worldManifold.points[i]);
				b2Vec2 pointVelOther =
					otherBody->GetLinearVelocityFromWorldPoint(worldManifold.points[i]);
				b2Vec2 relativeVel = platformBody->GetLocalVector(pointVelOther - pointVelPlatform);
				if (relativeVel.y < -1)
					solid = true;//point is moving into platform, leave contact solid
				else if (relativeVel.y < 1) {
					//borderline case, moving only slightly out of platform
					b2Vec2 contactPointRelativeToPlatform =
						platformBody->GetLocalPoint(worldManifold.points[i]);
					float platformFaceY = 0.5f * 1 / (float)platformScale;
					if (contactPointRelativeToPlatform.y > platformFaceY - 0.05)
						solid = true;
				}
			}
		}

		if (solid) {
			//check if fixture A was the foot
			void* fixtureUserData = contact->GetFixtureA()->GetUserData();
			if ((int)fixtureUserData == 100) {
				m_numFootContacts++;
			}
			//check if fixture B was the foot
			fixtureUserData = contact->GetFixtureB()->GetUserData();
			if ((int)fixtureUserData == 100) {
				m_numFootContacts++;
			}
		}
		else
			//no points are moving into platform, contact should not be solid
			contact->SetEnabled(false);
	}

	void EndContact(b2Contact* contact)
	{
		if (contact->IsEnabled()) {
			//check if fixture A was the foot
			void* fixtureUserData = contact->GetFixtureA()->GetUserData();
			if ((int)fixtureUserData == 100)
				m_numFootContacts--;
			//check if fixture B was the foot
			fixtureUserData = contact->GetFixtureB()->GetUserData();
			if ((int)fixtureUserData == 100)
				m_numFootContacts--;
		}

		contact->SetEnabled(true);
	}

	void Keyboard(unsigned char key)
	{
		switch (key)
		{
		case 'a':
			m_keyState |= MS_LEFT;
			break;
		case 'w':
			m_keyState |= MS_JUMP;
			break;
		case 'd':
			m_keyState |= MS_RIGHT;
			break;
		default:
			//run default behaviour
			Test::Keyboard(key);
		}
	}

	void KeyboardUp(unsigned char key)
	{
		switch (key)
		{
		case 'a':
			m_keyState &= ~MS_LEFT;
			break;
		case 'w':
			m_keyState &= ~MS_JUMP;
			break;
		case 'd':
			m_keyState &= ~MS_RIGHT;
			break;
		default:
			//run default behaviour
			Test::Keyboard(key);
		}
	}

	void Step(Settings* settings)
	{
		//update the moving platforms
		float theta = 0.025 * m_stepCount;
		{
			b2Vec2 targetPos(15 + 2 * sinf(theta), 15 + 2.55 * cosf(theta));
			m_platformBody->SetLinearVelocity(60 * (targetPos - m_platformBody->GetPosition()));
		}
		{
			b2Vec2 targetPos(15 - 0 * sinf(theta), 20 - 2.55 * cosf(theta));
			m_platformBody2->SetLinearVelocity(60 * (targetPos - m_platformBody2->GetPosition()));
		}

		//update the rotating floor section
		if (!settings->pause) { //testbed continuously calls Step which screws up this part
			m_rotatingFloorTimer--;
			if (m_rotatingFloorTimer < 0) {
				m_rotatingFloor->SetAngularVelocity(0);
				m_rotatingFloorTimer = 180;//3 second timeout
				m_rotatingFloorTurnCount++;
			}
			else if (m_rotatingFloorTimer < 70) {//start rotation just over 1 second before timeout
				float targetAngle = m_rotatingFloorTurnCount * 180 * DEGTORAD;
				float angleDiff = targetAngle - m_rotatingFloor->GetAngle();
				if (angleDiff < 2 * DEGTORAD) {
					m_rotatingFloor->SetTransform(m_rotatingFloor->GetPosition(), targetAngle);
					m_rotatingFloor->SetAngularVelocity(0);
				}
				else
					m_rotatingFloor->SetAngularVelocity(180 * DEGTORAD);
			}
		}

		b2Vec2 vel = m_playerBody->GetLinearVelocity();

		//update player sideways movement
		float desiredVel = 0;
		int move = m_keyState & (MS_LEFT | MS_RIGHT);
		switch (move)
		{
		case MS_LEFT:  desiredVel = b2Max(vel.x - 0.5f, -5.0f); break;//desiredVel = -5; break;
		case MS_RIGHT: desiredVel = b2Min(vel.x + 0.5f, 5.0f); break;//desiredVel =  5; break;
		}
		float velChange = desiredVel - vel.x;
		float impulse = m_playerBody->GetMass() * velChange;
		if (m_numFootContacts < 1)
			impulse *= 0.1f;
		m_playerBody->ApplyLinearImpulse(b2Vec2(impulse, 0), m_playerBody->GetWorldCenter());

		//update player jump
		m_jumpTimeout--;
		if (m_jumpTimeout < 0 && m_numFootContacts > 0 && m_keyState & MS_JUMP) {
			m_jumpTimeout = 15;
			float jumpVel = calculateVerticalVelocityForHeight(6);//for 60fps
			m_playerBody->SetLinearVelocity(b2Vec2(vel.x, jumpVel));
			m_playerFootBody->SetLinearVelocity(b2Vec2(vel.x, jumpVel));
		}


		Test::Step(settings);

		m_debugDraw.DrawString(5, m_textLine, "Press a/w/d to control player body");
		m_textLine += 15;
	}

	static Test* Create()
	{
		return new iforce2d_OneWayWalls_demo;
	}

	b2Body* m_platformBody;
	b2Body* m_platformBody2;

	b2Body* m_rotatingFloor;
	int m_rotatingFloorTimer;
	int m_rotatingFloorTurnCount;

	b2Body* m_playerBody;
	b2Body* m_playerFootBody;
	int m_numFootContacts;
	int m_keyState;
	int m_jumpTimeout;
};

#endif