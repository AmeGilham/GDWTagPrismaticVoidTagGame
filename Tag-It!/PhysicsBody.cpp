#include "PhysicsBody.h"

unsigned int CollisionIDs::m_playerID       = 0x1;
unsigned int CollisionIDs::m_environmentID  = 0x10;
unsigned int CollisionIDs::m_enemyID        = 0x100;
unsigned int CollisionIDs::m_platformID     = 0x1000;


unsigned int CollisionIDs::Player()
{
	return m_playerID;
}

unsigned int CollisionIDs::Environment()
{
	return m_environmentID;
}

unsigned int CollisionIDs::Enemy()
{
	return m_enemyID;
}

unsigned int CollisionIDs::Platform()
{
	return m_platformID;
}

vec3 PhysicsBody::m_gravityAcceleration = vec3(0.f, -9990.f, 0.f);
bool PhysicsBody::m_drawBodies = false;

vec3 PhysicsBody::GetForce() const
{
	//Returns applied force
	return m_appliedForce;
}

vec3 PhysicsBody::GetAcceleration() const
{
	return m_acceleration;
}

float PhysicsBody::GetMaxVelo() const
{
	return m_maxVelo;
}

vec3 PhysicsBody::GetVelocity() const
{
	return m_velocity;
}

float PhysicsBody::GetFriction() const
{
	return m_friction;
}

vec3 PhysicsBody::GetGravityAcceleration()
{
	return m_gravityAcceleration;
}

bool PhysicsBody::GetGravity() const
{
	return m_applyGravity;
}

float PhysicsBody::GetMass() const
{
	return m_mass;
}

BodyType PhysicsBody::GetBodyType() const
{
	return m_bodyType;
}

vec2 PhysicsBody::GetCenterOffset() const
{
	return m_centerOffset;
}

vec2 PhysicsBody::GetBottomLeft() const
{
	return m_bottomLeft;
}

vec2 PhysicsBody::GetBottomRight() const
{
	return m_bottomRight;
}

vec2 PhysicsBody::GetTopLeft() const
{
	return m_topLeft;
}

vec2 PhysicsBody::GetTopRight() const
{
	return m_topRight;
}

float PhysicsBody::GetWidth() const
{
	return m_width;
}

float PhysicsBody::GetHeight() const
{
	return m_height;
}

float PhysicsBody::GetRadius() const
{
	return m_radius;
}

bool PhysicsBody::GetDraw()
{
	return m_drawBodies;
}

unsigned int PhysicsBody::GetBodyID() const
{
	return m_bodyID;
}

unsigned int PhysicsBody::GetCollideID() const
{
	return m_collideID;
}

bool PhysicsBody::GetDynamic() const
{
	return m_dynamic;
}

b2Body * PhysicsBody::GetBody() const
{
	return m_body;
}

b2Vec2 PhysicsBody::GetPosition() const
{
	return m_position;
}

void PhysicsBody::SetBody(b2Body * body)
{
	m_body = body;
}

void PhysicsBody::SetPosition(b2Vec2 bodyPos)
{
	m_position = bodyPos;
}

void PhysicsBody::SetForce(vec3 force)
{
	//Sets the applied force
	m_appliedForce = force;
}

void PhysicsBody::SetAcceleration(vec3 accel)
{
	m_acceleration = accel;
}

void PhysicsBody::SetMaxVelo(float velo)
{
	m_maxVelo = velo;
}

void PhysicsBody::SetVelocity(vec3 velo)
{
	m_velocity = velo;
}

void PhysicsBody::SetFriction(float fric)
{
	m_friction = fric;
}

void PhysicsBody::SetGravityAcceleration(vec3 grav)
{
	m_gravityAcceleration = grav;
}

void PhysicsBody::SetGravity(bool grav)
{
	m_applyGravity = grav;
}

void PhysicsBody::SetMass(float mass)
{
	m_mass = mass;
}

void PhysicsBody::SetBodyType(BodyType type)
{
	m_bodyType = type;
}

void PhysicsBody::SetCenterOffset(vec2 cent)
{
	m_centerOffset = cent;
}

void PhysicsBody::SetBottomLeft(vec2 BL)
{
	m_bottomLeft = BL;
}

void PhysicsBody::SetBottomRight(vec2 BR)
{
	m_bottomRight = BR;
}

void PhysicsBody::SetTopLeft(vec2 TL)
{
	m_topLeft = TL;
}

void PhysicsBody::SetTopRight(vec2 TR)
{
	m_topRight = TR;
}

void PhysicsBody::SetWidth(float width)
{
	m_width = width;
}

void PhysicsBody::SetHeight(float height)
{
	m_height = height;
}

void PhysicsBody::SetRadius(float radius)
{
	m_radius = radius;
}

void PhysicsBody::SetDraw(bool drawBodies)
{
	m_drawBodies = drawBodies;
}

void PhysicsBody::SetBodyID(unsigned int bodyID)
{
	m_bodyID = bodyID;
}

void PhysicsBody::SetCollideID(unsigned int collideID)
{
	m_collideID = collideID;
}

void PhysicsBody::SetDynamic(bool isDynamic)
{
	m_dynamic = isDynamic;
}

PhysicsBody::PhysicsBody(b2Body * body, float radius, vec2 centerOffset, bool isDynamic)
{
	b2CircleShape tempShape;
	tempShape.m_p = b2Vec2(float32(centerOffset.x), float32(centerOffset.y));
	tempShape.m_radius = radius;

	//Creates the actual fixture (aka, shape, mass, etc)
	b2FixtureDef tempFixture;
	tempFixture.shape = &tempShape;
	tempFixture.density = 1.f;
	tempFixture.friction = 0.1f;

	m_body = body;
	m_body->CreateFixture(&tempFixture);

	m_body = body;
	m_bodyType = BodyType::CIRCLE;
	
	//Stores the position
	m_position = m_body->GetPosition();

	m_width = radius * 2.f;
	m_height = radius * 2.f;

	m_centerOffset = centerOffset;
	m_bottomLeft = vec2(centerOffset.x - (radius), centerOffset.y - (radius));
	m_bottomRight = vec2(centerOffset.x + (radius), centerOffset.y - (radius));
	m_topLeft = vec2(centerOffset.x - (radius), centerOffset.y + (radius));
	m_topRight = vec2(centerOffset.x + (radius), centerOffset.y + (radius));

	m_dynamic = isDynamic;

	InitBody();
}

PhysicsBody::PhysicsBody(b2Body * body, float width, float height, vec2 centerOffset, bool isDynamic)
{
	b2PolygonShape tempShape;
	tempShape.SetAsBox(float32(width / 2.f), float32(height / 2.f), b2Vec2(float32(centerOffset.x), float32(centerOffset.y)), float32(0.f));

	//Creates the actual fixture (aka, shape, mass, etc)
	b2FixtureDef tempFixture;
	tempFixture.shape = &tempShape;
	tempFixture.density = 0.08f;
	tempFixture.friction = 0.2f;//0.3f

	m_body = body;
	m_body->CreateFixture(&tempFixture);

	m_body = body;
	m_bodyType = BodyType::BOX;

	m_width = width;
	m_height = height;

	m_centerOffset = centerOffset;
	m_bottomLeft = vec2(centerOffset.x - (width / 2.f), centerOffset.y - (height / 2.f));
	m_bottomRight = vec2(centerOffset.x + (width / 2.f), centerOffset.y - (height / 2.f));
	m_topLeft = vec2(centerOffset.x - (width / 2.f), centerOffset.y + (height / 2.f));
	m_topRight = vec2(centerOffset.x + (width / 2.f), centerOffset.y + (height / 2.f));

	m_dynamic = isDynamic;

	InitBody();
}

void PhysicsBody::ApplyForce(vec3 force)
{
	//Add a force to the applied force
	m_body-> ApplyForce ( b2Vec2(float32(force.x), float32(force.y)), b2Vec2(float32(m_body->GetPosition().x), float32(m_body->GetPosition().y)), true);
}

void PhysicsBody::Update(Transform * trans)
{
	//Stores the position
	m_position = m_body->GetPosition();

	//Sets the transform position to the position of the physics body
	trans->SetPosition(vec3(m_body->GetPosition().x, m_body->GetPosition().y, trans->GetPosition().z));
	trans->SetRotationAngleZ(Degrees(m_body->GetAngle()));
}

void PhysicsBody::InitBody()
{
	m_vao = VertexManager::CreateVAO();
	glBindVertexArray(m_vao);

	//Enable slot 0
	glEnableVertexAttribArray(0); //Vertices

	//VBO positions
	m_vboPos = VertexManager::GetPlaneVertVBO();

	//Pushes away the warnings
#pragma warning(push)
#pragma warning(disable : 4312)
	//Bind position buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vboPos);
	//Sets the attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
#pragma warning(pop)

	//Unbinds the buffers
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);
}

void PhysicsBody::DrawBody()
{
	//Bind vao
	glBindVertexArray(m_vao);
	//Draw the points
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//Unbind vao
	glBindVertexArray(GL_NONE);
}