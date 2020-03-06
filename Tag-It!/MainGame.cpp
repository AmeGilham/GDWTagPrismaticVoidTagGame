#include "MainGame.h"

//Constructor 
MainGame::MainGame(std::string name)
	: Scene(name)
{
	//create the gravity vector for the scene's dynamic objects
	m_gravity = b2Vec2(float32(0.f), float32(-150.f));
	//set the gravity vector to be active for the scene
	m_physicsWorld->SetGravity(m_gravity);
	//setup the box2d contact listener 
	m_physicsWorld->SetContactListener(&listener);
}

//Initlize the scene
void MainGame::InitScene(float windowWidth, float windowHeight)
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
		ECS::GetComponent<Camera>(entity).Zoom(75.f);
		vec4 size = ECS::GetComponent<Camera>(entity).GetOrthoSize();

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::CameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Main Cam");
		ECS::SetIsMainCamera(entity, true);
	}

	//setup for the backdrop (static, not the flow of the water)
	{
		//Creates enetity
		auto entity = ECS::CreateEntity(); 

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components 
		std::string fileName = "backdrop.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 90, 50);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));

		//Setup indentifier 
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Backdrop (static)");
	}

	//setup for backdrop (animated, waterfall flow section)
	{
		//our JSON animation file
		auto flow = File::LoadJSON("waterfall.json");

		//creates entity
		auto entity = ECS::CreateEntity();

		//Adds components 
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		//Sets up components
		std::string fileName = "flow.png";
		//grab a reference to the animation controller
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		//set the spriteset
		animController.InitUVs(fileName);

		//setup the sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 23, 23, true, &animController);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-33.f, -13.f, 1.f));
		
		//creates the animations
		animController.AddAnimation(flow["flow"]);
		animController.SetActiveAnim(0);

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
		ECS::SetUpIdentifier(entity, bitHolder, "backdrop (dynamic waterflow part");
	}

	//setup Not-It! objective 
	{
		//creates entity 
		auto entity = ECS::CreateEntity();

		//adds components 
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//setup the sprite and transform components 
		std::string fileName = "Not-It!.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 8, 2);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 17.35f, 30.f));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Not-It! obejctive");
		ECS::SetIsNotItObjective(entity, true);
		imporantEntities[0] = entity;
	}
	
	//Setup player entities (both orange and blue, cycling blue then orange)
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
			std::string fileName = "orange.png"; //set the default sprite sheet to be Orange's
			if (i == 0) fileName = "blue.png"; //if the first player is being created, make it blue
			//grab a reference to the animation controler
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			//set the spritesheet 
			animController.InitUVs(fileName);

			//setup the sprite and transform compoments 
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 6, 6, true, &animController);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 2.5f, 50.f+0.1f*i));

			//grab references to the sprite and physic body compoments
			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			//calculate the area of the sprite that shouldn't have a physics body attached (empty space, ponytail/necklace, etc.)
			float shrinkX = tempSpr.GetWidth() / 3.f;
			float shrinkY = tempSpr.GetWidth() / 6.f;

			//setup the dynamic box2d physics body
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			//set the position
			if (i == 0) tempDef.position.Set(float32(-18.f), float32(-7.5f)); //blue position
			else tempDef.position.Set(float32(18.f), float32(-7.5f)); //orange position

			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);

			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//setup the user data to identify them as players
			if (i == 0) tempBody->SetUserData(&blue); //blue
			else tempBody->SetUserData(&orange); //orange

			//add their animations, and make sure they're set to be repeating
			animController.AddAnimation(moving["runLeft"]);
			animController.GetAnimation(0).SetRepeating(true);
			animController.AddAnimation(moving["runLeft"]);
			animController.GetAnimation(1).SetRepeating(true);
			animController.AddAnimation(moving["runRight"]);
			animController.GetAnimation(2).SetRepeating(true);

			//set the active animations so that they're facing the right direction when they spawn
			if (i == 0) animController.SetActiveAnim(2);
			else animController.SetActiveAnim(1);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Player " + std::to_string(i+1));
			//if it's blue
			if (i == 0)
			{
				//set them as the main player
				ECS::SetIsMainPlayer(entity, true);
			}
			//if it's orange
			else if (i == 1) 
			{
				//set them as the second player
				ECS::SetIsSecondPlayer(entity, true);
			}
		}
	}

	//plank entities
	for (int i = 0; i < 50; i++)
	{
		{ 
			//Creates entity
			auto entity = ECS::CreateEntity();

			//adds components 
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//loadsprite sheet and set up sprite component
			std::string fileName = "planks.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 2);
			//setup transform component
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 10.f + 0.01 * i));

			//grab references to the sprite and physics body components
			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			//calculate the area of the sprite that shouldn't have a physics body attached (empty space, etc.)
			float shrinkX = 0.f;
			float shrinkY = 0.f;

			//setup the static box2d physics body
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			//set the position
			if (i < 12) tempDef.position.Set(float32(-60.f + (10.f * i)), float32(-15.f));
			else if (i > 11 && i < 24) tempDef.position.Set(float32(-60.f + (10.f * (i - 11))), float32(-17.f));
			else if (i > 23 && i < 36) tempDef.position.Set(float32(-60.f + (10.f * (i - 23))), float32(-19.f));
			else if (i > 35 && i < 40)  tempDef.position.Set(float32(-60.f + (10.f * (i - 35))), float32(1.f));
			else if (i > 39 && i < 44) tempDef.position.Set(float32(60.f - (10.f * (i - 39))), float32(1.f));
			else if (i > 43 && i < 50)tempDef.position.Set(float32(35.f - (10.f * (i - 43))), float32(15.f));
			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);

			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.45f), false);

			//set up user data to indentify as a border (players can't jump through the bottom)
			tempBody->SetUserData(&border);

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Plank " + std::to_string(i+1));
		}
	}

	//create the crosswalk entities (just asethic)
	for (int i = 0; i < 12; i++)
	{
		{ 
			//Creates entity
			auto entity = ECS::CreateEntity();

			//adds components 
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);

			//loadsprite sheet and set up sprite component
			std::string fileName = "crosswalk boards.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 2);
			//setup transform component
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(-60.f + (10.f * i), -13.f, 70.f + 0.01 * i));

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Crosswalk" + std::to_string(i));
		}
	}

	//platform  entities
	for (int i = 0; i < 14; i++) {
		{  
			//creates entity
			auto entity = ECS::CreateEntity();

			//adds components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//loadsprite sheet and set up sprite component
			std::string fileName = "logs.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 2);
			//setup transform component
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(2.5f, 0.f, 12.f + (0.01f * i)));

			//grab references to the sprite and physics body components
			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			//calculate the area of the sprite that shouldn't have a physics body attached (empty space, etc.)
			float shrinkX = 0.f;
			float shrinkY = 0.f;

			//setup the static box2d physics body
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			//Sets positions for each platform 
			if(i ==0 )tempDef.position.Set(float32(-60.f), float32(-7.f)); 
			else if(i == 1)tempDef.position.Set(float32(-50.f), float32(-7.f)); 
			else if (i == 2)tempDef.position.Set(float32(-40.f), float32(-7.f)); 
			else if (i == 3)tempDef.position.Set(float32(60.f), float32(-7.f)); 
			else if (i == 4)tempDef.position.Set(float32(50.f), float32(-7.f)); 
			else if (i == 5)tempDef.position.Set(float32(40.f), float32(-7.f)); 
			else if (i == 6)tempDef.position.Set(float32(-5.f), float32(-7.f));
			else if (i == 7)tempDef.position.Set(float32(5.f), float32(-7.f));
			else if (i == 8)tempDef.position.Set(float32(-65.f), float32(9.f));
			else if (i == 9)tempDef.position.Set(float32(-55.f), float32(9.f));
			else if (i == 10)tempDef.position.Set(float32(-45.f), float32(9.f));
			else if (i == 11)tempDef.position.Set(float32(65.f), float32(9.f));
			else if (i == 12)tempDef.position.Set(float32(55.f), float32(9.f));
			else if (i == 13)tempDef.position.Set(float32(45.f), float32(9.f));

			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);

			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);

			//set up user data to indentify as a platform (players can jump through the bottom)
			tempBody->SetUserData(&platform);

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Log platform " + std::to_string(i+1));
		}
	}

	//create the rope entities (just asethic, first jump distance)
	for (int i = 0; i < 24; i++)
	{
		{
			//Creates entity
			auto entity = ECS::CreateEntity();

			//adds components 
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);

			//loadsprite sheet and set up sprite component
			std::string fileName = "ropes.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 2);
			//setup transform component
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(-60.f + (10.f * i), -7.f, 11.f + 0.01 * i));
			if(i > 11) ECS::GetComponent<Transform>(entity).SetPosition(vec3(-60.f + (10.f * (i-12)), 9.f, 11.f + 0.01 * i));

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Rope " + std::to_string(i));
		}
	}

	//vertical segements
	//vertical planks
	for (int i = 0; i < 4; i++){
		{
			//Creates entity
			auto entity = ECS::CreateEntity();

			//adds components 
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//loadsprite sheet and set up sprite component
			std::string fileName = "vert planks.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 2, 10);
			//setup transform component
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 14.f + 0.01 * i));

			//grab references to the sprite and physics body components
			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			//calculate the area of the sprite that shouldn't have a physics body attached (empty space, etc.)
			float shrinkX = 0.f;
			float shrinkY = 0.f;

			//setup the static box2d physics body
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			//set the position
			if (i < 2) tempDef.position.Set(float32(-1.f + (2.f * i)), float32(-3.f));
			else tempDef.position.Set(float32(-1.f + (2.f * (i-2))), float32(24.f));
			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);

			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX + 0.5f), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.0f), false);

			//set up user data to indentify as a border (players can't jump through the bottom)
			tempBody->SetUserData(&border);

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Vertical plank " + std::to_string(i + 1));
		}
	}

	//create the vertical rope entities (just asethic)
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//adds components 
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//load sprites and set up sprite component
		std::string fileName = "vert ropes.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 2, 16);
		//setup transform component
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 8.f, 9.f));

		//Setup indentifier 
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Vertical rope");
	}

	//create the bombs part of the HUD 
	for(int i = 0; i < 2; i++){
		{
			//creates entity
			auto entity = ECS::CreateEntity();

			//adds components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);

			//load sprites and set up sprite component
			std::string fileName = "blue bomb.png";
			if (i == 1) fileName = "orange bomb.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 12, 4);
			//Setup transform 
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(-15,-17.f, 98.f+(0.01 * i)));
			if (i == 1) ECS::GetComponent<Transform>(entity).SetPosition(vec3(15, -17.f, 98.f + (0.01 * i)));

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
			if (i == 0) ECS::SetUpIdentifier(entity, bitHolder, "blue bomb");
			else ECS::SetUpIdentifier(entity, bitHolder, "orange bomb");
		}
	}
}



//Update the scene, every frame
void MainGame::Update()
{
	//add the change in time to the time since blue and orange last jumped (used to control jumping with platforms reseting jumps)
	blueTimeSinceLastJump += Timer::deltaTime;
	orangeTimeSinceLastJump += Timer::deltaTime;

	//grab blue's physics body info
	auto& bluetempPhysBod = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
	b2Body* bluebody = bluetempPhysBod.GetBody();

	//if Blue has run off the right of the screen, make her appear on the left
	if (bluetempPhysBod.GetPosition().x > 50.5) {
		bluetempPhysBod.GetBody()->SetTransform(b2Vec2(-50.5, bluebody->GetPosition().y), float32(0));
	}
	//if Blue has run off the left of the screen, make her appear on the right
	else if (bluetempPhysBod.GetPosition().x < -50.5) {
		bluetempPhysBod.GetBody()->SetTransform(b2Vec2(50.5, bluebody->GetPosition().y), float32(0));
	}

	//grab orange's physics body info
	auto& orangetempPhysBod = ECS::GetComponent<PhysicsBody>(EntityIdentifier::SecondPlayer());
	b2Body* orangebody = orangetempPhysBod.GetBody();

	//if Orange has run off the right of the screen, make him appear on the left 
	if (orangetempPhysBod.GetPosition().x > 50.5) {
		orangetempPhysBod.GetBody()->SetTransform(b2Vec2(-50.5, orangebody->GetPosition().y), float32(0));
	}
	//if Orange has run off the left of the screen, make him appear on the right
	else if (orangetempPhysBod.GetPosition().x < -50.5) {
		orangetempPhysBod.GetBody()->SetTransform(b2Vec2(50.5, orangebody->GetPosition().y), float32(0));
	}

	//if the person whose it has changed
	if (listener.GetItChange()) {
		//check if it was through the not it objective 
		if (listener.GetNotItObjExists()) {
			//if it was, set the not it objective to not exist
			listener.SetNotItObjExists(false);
			/*fill rest of code about destroying not it objective later*/
		}
		//check if blue is now it 
		if (listener.GetIt() == 1) {
			/*fill rest of code about showing that blue is now it*/
		}
		//if not, then orange must now be it 
		else {
			/*fill rest of code about showing that orange is now it*/
		}
	}

	//if Blue is currently it
	if (listener.GetIt() == 1) {
		/*fill in code about showing blue is it, and having her bomb fuse burn*/
		blueFuseTimeRemaining -= Timer::deltaTime;
	}
	//Or if Orange is currently it 
	else if (listener.GetIt() == 2) {
		/*fill in code about showing Orange is it, and having his bomb fuse burn*/
		orangeFuseTimeRemaining -= Timer::deltaTime;
	}

	printf("%f\n", 1.0 / Timer::deltaTime);
}

//Stroke of the gamepad input
void MainGame::GamepadStroke(XInputController* con)
{
}

//Gamepad stick input
void MainGame::GamepadStick(XInputController* con)
{
}

//Gamepad trigger button input
void MainGame::GamepadTrigger(XInputController* con)
{
}

//keyboard key held down input
void MainGame::KeyboardHold()
{
	//vector with the force for the player's x movement
	vec3 runforce = vec3(1000.f * 60.f * Timer::deltaTime, 0.f, 0.f);

	//grab a reference to blue's physics body
	auto& tempPhysBodB = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
	//create a pointer to Blue's box2d body
	b2Body* bodyB = tempPhysBodB.GetBody();

	//if Blue's player is pressing A, and their x-velocity isn't above the left cap, apply the run force to the left
	if (Input::GetKey(Key::A) && bodyB->GetLinearVelocity().x > float32(-30.f)) {
		tempPhysBodB.ApplyForce(-runforce);
	}

	//if Blue's player is pressing D, and their x-velocity isn't above the right cap, apply the run force to the right
	else if (Input::GetKey(Key::D) && bodyB->GetLinearVelocity().x < float32(30.f)) {
		tempPhysBodB.ApplyForce(runforce);
	}

	//otherwise blue isn't moving on the x-axis
	else {
		//so if she's still moving right, subtract from velocity from her motion
		if (bodyB->GetLinearVelocity().x > float32(0.f))
		{
			//if her velocity is above 5, subtract 5
			if (bodyB->GetLinearVelocity().x > float32(5.f)) {

				bodyB->SetLinearVelocity(b2Vec2(bodyB->GetLinearVelocity().x - 5, bodyB->GetLinearVelocity().y));
			}
			//otherwise it's between 0 and 5, so just set it to 0
			else {
				bodyB->SetLinearVelocity(b2Vec2(0, bodyB->GetLinearVelocity().y));
			}
		}
		//and if she's still moving left, add velocity to her motion (bringing her to 0 and thus not moving)
		else if (bodyB->GetLinearVelocity().x < float32(0.f))
		{
			//if her velocity is below -5, add 5
			if (bodyB->GetLinearVelocity().x < float32(-5.f)) {

				bodyB->SetLinearVelocity(b2Vec2(bodyB->GetLinearVelocity().x + 5, bodyB->GetLinearVelocity().y));
			}
			//otherwise it's between -5 and 0, so just set it to 0
			else {
				bodyB->SetLinearVelocity(b2Vec2(0, bodyB->GetLinearVelocity().y));
			}
		}
	}

	//grab a reference to orange's physics body
	auto& tempPhysBodO = ECS::GetComponent<PhysicsBody>(EntityIdentifier::SecondPlayer());
	//create a pointer to Orange's box2d body
	b2Body* bodyO = tempPhysBodO.GetBody();

	//if Orange's player is pressing leftArrow, and their x-velocity isn't above the left cap, apply the run force to the left
	if (Input::GetKey(Key::LeftArrow) && bodyO->GetLinearVelocity().x > float32(-30.f)) {
		tempPhysBodO.ApplyForce(-runforce);
	}

	//if Orange's player is pressing rightArrow, and their x-velocity isn't above the right cap, apply the run force to the right
	else if (Input::GetKey(Key::RightArrow) && bodyO->GetLinearVelocity().x < float32(30.f)) {
		tempPhysBodO.ApplyForce(runforce);
	}

	//otherwise Orange isn't moving on the x-axis
	else {
		//so if he's still moving right, subtract from velocity from his motion
		if (bodyO->GetLinearVelocity().x > float32(0.f))
		{
			//if his velocity is above 5, subtract 5
			if (bodyO->GetLinearVelocity().x > float32(5.f)) {

				bodyO->SetLinearVelocity(b2Vec2(bodyO->GetLinearVelocity().x - 5, bodyO->GetLinearVelocity().y));
			}
			//otherwise it's between 0 and 5, so just set it to 0
			else {
				bodyO->SetLinearVelocity(b2Vec2(0, bodyO->GetLinearVelocity().y));
			}
		}
		//and if he's still moving left, add velocity to his motion (bringing him to 0 and thus not moving)
		else if (bodyO->GetLinearVelocity().x < float32(0.f))
		{
			//if his velocity is below -5, add 5
			if (bodyO->GetLinearVelocity().x < float32(-5.f)) {

				bodyO->SetLinearVelocity(b2Vec2(bodyO->GetLinearVelocity().x + 5, bodyO->GetLinearVelocity().y));
			}
			//otherwise it's between -5 and 0, so just set it to 0
			else {
				bodyO->SetLinearVelocity(b2Vec2(0, bodyO->GetLinearVelocity().y));
			}
		}
	}

	//vector for the force of player's jumping
	vec3 jump = vec3(0.f, 5200.f * 60.f * Timer::deltaTime, 0.f);

	//if blue's player has pressed W, and she can jump, make her jump
	if (Input::GetKey(Key::W)) {
		//Check if Blue can jump 
		if (listener.getJumpB() && blueTimeSinceLastJump > 0.4f) {
			//if she can, set it so she can't 
			listener.setJumpB(false);
			blueTimeSinceLastJump = 0.f;
			//and apply the upward force of the jump
			tempPhysBodB.ApplyForce(jump);
		}
	}

	//if orange's player has pressed upArrow, and he can jump, make him jump
	if (Input::GetKey(Key::UpArrow)) {
		//Check if Orange can jump 
		if (listener.getJumpO() && orangeTimeSinceLastJump > 0.4f) {
			//if he can, set it so he can't 
			listener.setJumpO(false);
			orangeTimeSinceLastJump = 0.f;
			//and apply the upward force of the jump
			tempPhysBodO.ApplyForce(jump);
		}
	}
}

//keyboard key first pressed input
void MainGame::KeyboardDown()
{
	if (Input::GetKeyDown(Key::D))
	{
		auto& animController = ECS::GetComponent<AnimationController>(EntityIdentifier::MainPlayer());
		//Sets active animation
		animController.SetActiveAnim(2);
	}
	if (Input::GetKeyDown(Key::A))
	{
		auto& animController = ECS::GetComponent<AnimationController>(EntityIdentifier::MainPlayer());
		//Sets active animation
		animController.SetActiveAnim(1);
	}

	if (Input::GetKeyDown(Key::RightArrow)) {
		auto& animController = ECS::GetComponent<AnimationController>(EntityIdentifier::SecondPlayer());
		//Sets active animation
		animController.SetActiveAnim(2);
	}
	if (Input::GetKeyDown(Key::LeftArrow)) {
		auto& animController = ECS::GetComponent<AnimationController>(EntityIdentifier::SecondPlayer());
		//Sets active animation
		animController.SetActiveAnim(1);
	}
}

//keyboard, button released input
void MainGame::KeyboardUp()
{
}

//mouse moved input
void MainGame::MouseMotion(SDL_MouseMotionEvent evnt)
{
}

//mouse click input
void MainGame::MouseClick(SDL_MouseButtonEvent evnt)
{
}

//mouse wheel input
void MainGame::MouseWheel(SDL_MouseWheelEvent evnt)
{
}
