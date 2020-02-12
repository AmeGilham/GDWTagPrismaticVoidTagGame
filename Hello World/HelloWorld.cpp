#include "HelloWorld.h"

HelloWorld::HelloWorld(std::string name)
	: Scene(name)
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
		ECS::AttachComponent<HorizontalScroll>(entity);
		vec4 temp = ECS::GetComponent<Camera>(entity).GetOrthoSize();
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::HoriScrollCameraBit() | EntityIdentifier::CameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Horizontal Scrolling Cam");
		ECS::SetIsMainCamera(entity, true);

		//Attaches the camera to the vertical scroll
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<HorizontalScroll>(entity).SetOffest(15.f);
	}

	//Setup player sprite entity
	{
		//Our JSON animation file
		auto moving = File::LoadJSON("PikachuMain.json");

		//Creates entity
		auto entity = ECS::CreateEntity();
		EntityIdentifier::MainPlayer(entity);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<AnimationController>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Sets up components
		std::string fileName = "pikachu.png";
		auto &animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(fileName);		

	   ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 36, 32, true, &animController);
	   ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 10.f, 100.f));

	   auto& tempSpr = ECS::GetComponent<Sprite>(entity);
	   auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

	   float shrinkX = tempSpr.GetWidth() / 3.f;
	   float shrinkY = tempSpr.GetWidth() / 3.f;

	   b2Body* tempBody;
	   b2BodyDef tempDef;
	   tempDef.type = b2_dynamicBody;
	   tempDef.position.Set(float32(0.f), float32(5.f));

	   tempBody = m_physicsWorld->CreateBody(&tempDef);

	   tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
		   vec2(0.f, 0.f), false);
		   
	   tempPhsBody.SetFriction(0.15f);
	   tempPhsBody.SetMaxVelo(0.10f);
	   tempPhsBody.SetGravity(true);
	   tempPhsBody.SetGravityAcceleration(vec3(1000.f, -9990.f, 0.f));
	   
	   animController.AddAnimation(moving["PikachuStand"]);
	   animController.GetAnimation(0).SetRepeating(true);
	   animController.AddAnimation(moving["PikachuJump"]);
	   animController.GetAnimation(1).SetRepeating(true);
	   animController.AddAnimation(moving["PikachuJump2"]);
	   animController.GetAnimation(2).SetRepeating(true);

	   animController.SetActiveAnim(1);

	   //Sets up the Identifier
	   unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
	   ECS::SetUpIdentifier(entity, bitHolder, "Pikachu");
	   ECS::SetIsMainPlayer(entity, true);

	   m_player = entity;
	}

//
//#pragma region Boxes
//	//Setup Box Entity 1
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(0.f), float32(200.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu1");
//	}
//
//	//Setup Box Entity 2
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(18.f), float32(175.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu2");
//	}
//
//	//Setup Box Entity 3
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(-18.f), float32(175.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu3");
//	}
//
//	//Setup Box Entity 4
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(-36.f), float32(150.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu4");
//	}
//
//	//Setup Box Entity 5
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(0.f), float32(150.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu5");
//	}
//
//	//Setup Box Entity 6
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(36.f), float32(150.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu6");
//	}
//
//	//Setup Box Entity 7
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(-54.f), float32(125.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu7");
//	}
//
//	//Setup Box Entity 8
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(-18.f), float32(125.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu8");
//	}
//
//	//Setup Box Entity 9
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(18.f), float32(125.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu9");
//	}
//
//	//Setup Box Entity 10
//	{
//		//Creates entity
//		auto entity = ECS::CreateEntity();
//		//Add components
//		ECS::AttachComponent<Sprite>(entity);
//		ECS::AttachComponent<Transform>(entity);
//		ECS::AttachComponent<PhysicsBody>(entity);
//		//Sets up components
//		std::string fileName = "Eiscue2.png";
//		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 27, 20);
//		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 1.f));
//		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
//		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
//
//		float shrinkX = tempSpr.GetWidth() / 11.f;
//		float shrinkY = tempSpr.GetWidth() / 11.f;
//		b2Body* tempBody;
//		b2BodyDef tempDef;
//		tempDef.type = b2_dynamicBody;
//
//		tempDef.position.Set(float32(54.f), float32(125.f));
//
//		tempBody = m_physicsWorld->CreateBody(&tempDef);
//		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
//			vec2(0.f, 0.f), false);
//		tempPhsBody.SetFriction(0.20f);
//		tempPhsBody.SetMaxVelo(0.50f);
//		tempPhsBody.SetGravity(true);
//
//		//Sets up the Identifier
//		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
//		ECS::SetUpIdentifier(entity, bitHolder, "Pingu10");
//	}
//#pragma endregion
//	
//	
	//Set up ground entity
	
	{  //platform entity
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		std::string fileName = "BG.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 90, 30);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(10.f, 0.f, -10.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = (tempSpr.GetHeight() / 2.f);

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(80.f), float32(20.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, (-tempSpr.GetHeight() / 16.f) * 6.f), false);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Platform");
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
		tempDef.position.Set(float32(0.f), float32(0.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, (-tempSpr.GetHeight() / 16.f) * 6.f), false);

		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Ground");
	}

	//Makes the camera focus on the main player
	//We do this at the very bottom so we get the most accurate pointer to our Transform
	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
}



int HelloWorld::GetPlayer()
{
	return m_player;
}
