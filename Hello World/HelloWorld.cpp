#include "HelloWorld.h"
#include "Game.h"
#include "Timer.h"

HelloWorld::HelloWorld(std::string name) : Scene(name)
{
	m_gravity = b2Vec2(float32(0.f), float32(-15.f));
	m_physicsWorld->SetGravity(m_gravity);
	m_physicsWorld->SetContactListener(&listener);

}
void HelloWorld::InitScene(float windowWidth, float windowHeight)
{
	//Dynamically allocates the register (so when you unload the scene when you switch between scenes
	//you can later reInit this scene
	m_sceneReg = new entt::registry;
	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	//Setup main camera entity
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
		EntityIdentifier::MainCamera(entity);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		vec4 temp = ECS::GetComponent<Camera>(entity).GetOrthoSize();
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::CameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Main Cam");
		ECS::SetIsMainCamera(entity, true);
	}

	//power ups
	/*{
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string fileName = "power.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 15, 20);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(10.f, 0.f, -10.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 5.f;
		float shrinkY = (tempSpr.GetHeight() / 2.5f);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-15.f), float32(-35.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, (-tempSpr.GetHeight() / 45.f) * 0.1f), false);

		tempBody->SetUserData(&power);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Power ");

		if (Timer::time >= 5.0f) {
			ECS::DestroyEntity(entity);
			Timer::Reset();
			Timer::Update();
		}

	

	}*/

	//Setup player sprite entity
	for (int i = 0; i < 2; i++) {
		{
			//Our JSON animation file
			auto moving = File::LoadJSON("Player.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//Sets up components
			std::string fileName = "orange.png";
			if (i == 0) fileName = "blue.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 24, 24, true, &animController);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 10.f, 100.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = tempSpr.GetWidth() / 3.f;
			float shrinkY = tempSpr.GetWidth() / 6.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			if (i == 0) tempDef.position.Set(float32(-72.f), float32(-30.f));
			else tempDef.position.Set(float32(72), float32(-30.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//tempPhsBody.SetFriction(0.15f);
			//tempPhsBody.SetMaxVelo(0.10f);
			tempPhsBody.SetGravity(true);
			tempPhsBody.SetGravityAcceleration(vec3(1000.f, -9990.f, 0.f));

			if (i == 0) tempBody->SetUserData(&blue);
			else tempBody->SetUserData(&orange);

			animController.AddAnimation(moving["runLeft"]);
			animController.GetAnimation(0).SetRepeating(true);
			animController.AddAnimation(moving["runLeft"]);
			animController.GetAnimation(1).SetRepeating(true);
			animController.AddAnimation(moving["runRight"]);
			animController.GetAnimation(2).SetRepeating(true);

			if (i == 0) animController.SetActiveAnim(2);
			else animController.SetActiveAnim(1);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Player " + std::to_string(i+1));
			if (i == 0)
			{
				ECS::SetIsMainPlayer(entity, true);
			}
			else if (i == 1) 
			{
				ECS::SetIsSecondPlayer(entity, true);
			}

			m_player = entity;
		}
	}

	{  //ground entity
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string fileName = "BG.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 615, 228);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, -10.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = (tempSpr.GetHeight() / 2.f);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0.f), float32(-30.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, (-tempSpr.GetHeight() / 16.f) * 6.f), false);

		tempBody->SetUserData(&border);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Ground");
	}

	
	for (int i = 0; i < 8; i++) {
		{  //platform  entity
			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			std::string fileName = "BG.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 30, 15);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(10.f, 0.f, -10.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = (tempSpr.GetHeight() / 2.f);

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			if(i ==0 )tempDef.position.Set(float32(-145.f), float32(-20.f));
			else if(i == 1)tempDef.position.Set(float32(0.f), float32(-20.f));
			else if (i == 2)tempDef.position.Set(float32(145.f), float32(-20.f));
			else if (i == 3)tempDef.position.Set(float32(72.f), float32(20.f));
			else if (i == 4)tempDef.position.Set(float32(-72.f), float32(20.f));
			else if (i == 5)tempDef.position.Set(float32(-145.f), float32(50.f));
			else if (i == 6)tempDef.position.Set(float32(0.f), float32(50.f));
			else if (i == 7)tempDef.position.Set(float32(145.f), float32(50.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, (-tempSpr.GetHeight() / 16.f) * 6.f), false);

			tempBody->SetUserData(&platform);

			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Platform " + std::to_string(i+1));
		}
	}


}

void HelloWorld::Update()
{
	{
		float rx1= rand() % (199 - (-199)) + (-199); //x coord between 199 and -199  [rand() % (high - low) + low;]
		//float ry1 = rand() % (-11 - (-48)) + (-48); 
		float ry1 = -35.f, ry2 = 30.f, ry3 = 78.f;
		float ry = rand() % (4 - (1)) + (1);

		if (Timer::time >= 2.0f) {
			Timer::Reset();
			Timer::Update();

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			std::string fileName = "power.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 15, 20);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(10.f, 0.f, -10.f));

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 5.f;
			float shrinkY = (tempSpr.GetHeight() / 2.5f);

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			if (ry == 1) { tempDef.position.Set(float32(rx1), float32(ry1));	}
			else if (ry == 2) { tempDef.position.Set(float32(rx1), float32(ry2));  }
			else if (ry == 3) { tempDef.position.Set(float32(rx1), float32(ry3)); 	}
			tempDef.fixedRotation = true;

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, (-tempSpr.GetHeight() / 45.f) * 0.1f), false);

			tempBody->SetUserData(&power);
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Power ");
			count++;
			if (Timer::time >= 5.0f && count >= 1) {
				ECS::DestroyEntity(entity);
				Timer::Reset();
				Timer::Update();
				std::cout << Timer::time << "Time(destroy): " << std::endl;}
		}
	}



}



int HelloWorld::GetPlayer()
{
	return m_player;
}
