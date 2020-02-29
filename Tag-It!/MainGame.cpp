#include "MainGame.h"

//Constructor 
MainGame::MainGame(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(float32(0.f), float32(0.f));
	m_physicsWorld->SetGravity(m_gravity);
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

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::CameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Main Cam");
		ECS::SetIsMainCamera(entity, true);
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
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 24, 24, true, &animController);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 10.f, 99.f+0.1f*i));

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
			if (i == 0) tempDef.position.Set(float32(-72.f), float32(-30.f)); //blue position
			else tempDef.position.Set(float32(72), float32(-30.f)); //orange position

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
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
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
			m_player = entity;
		}
	}

	{  //ground entity
		//Creates entity
		auto entity = ECS::CreateEntity();

		//adds components 
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//loadsprite sheet and set up sprite component
		std::string fileName = "BG.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 615, 228);
		//setup transform component
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, -10.f));

		//grab references to the sprite and physics body components
		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		//calculate the area of the sprite that shouldn't have a physics body attached (empty space, etc.)
		float shrinkX = 0.f;
		float shrinkY = (tempSpr.GetHeight() / 2.f);

		//setup the static box2d physics body
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		//set the position
		tempDef.position.Set(float32(0.f), float32(-30.f));

		//add the physics body to box2D physics world simulator
		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//create a spriteLib physics body using the box2D physics body
		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, (-tempSpr.GetHeight() / 16.f) * 6.f), false);

		//set up user data to indentify as a border (players can't jump through the bottom)
		tempBody->SetUserData(&border);

		//Setup indentifier 
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Ground");
	}

	//platform  entities (cycles through 8 for this prototype)
	for (int i = 0; i < 8; i++) {
		{  
			//creates entity
			auto entity = ECS::CreateEntity();

			//adds components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//loadsprite sheet and set up sprite component
			std::string fileName = "BG.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 30, 15);
			//setup transform component
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(10.f, 0.f, -10.f));

			//grab references to the sprite and physics body components
			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			//calculate the area of the sprite that shouldn't have a physics body attached (empty space, etc.)
			float shrinkX = 0.f;
			float shrinkY = (tempSpr.GetHeight() / 2.f);

			//setup the static box2d physics body
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			//Sets positions for each platform 
			if(i ==0 )tempDef.position.Set(float32(-145.f), float32(-20.f)); //bottom left
			else if(i == 1)tempDef.position.Set(float32(0.f), float32(-20.f)); //bottom middle
			else if (i == 2)tempDef.position.Set(float32(145.f), float32(-20.f)); //bottom right
			else if (i == 3)tempDef.position.Set(float32(-72.f), float32(22.f)); //middle left
			else if (i == 4)tempDef.position.Set(float32(72.f), float32(22.f)); //midle right
			else if (i == 5)tempDef.position.Set(float32(-145.f), float32(60.f)); //top left
			else if (i == 6)tempDef.position.Set(float32(0.f), float32(60.f)); //top middle
			else if (i == 7)tempDef.position.Set(float32(145.f), float32(60.f));//top right

			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);

			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, (-tempSpr.GetHeight() / 16.f) * 6.f), false);

			//set up user data to indentify as a platform (players can jump through the bottom)
			tempBody->SetUserData(&platform);

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Platform " + std::to_string(i+1));
		}
	}


}

//returns the player entity number (not used, players are identified through other methods)
int MainGame::GetPlayer()
{
	return m_player;
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
	if (bluetempPhysBod.GetPosition().x > 202) {
		bluetempPhysBod.GetBody()->SetTransform(b2Vec2(-202, bluebody->GetPosition().y), float32(0));
	}
	//if Blue has run off the left of the screen, make her appear on the right
	else if (bluetempPhysBod.GetPosition().x < -202) {
		bluetempPhysBod.GetBody()->SetTransform(b2Vec2(202, bluebody->GetPosition().y), float32(0));
	}

	//grab orange's physics body info
	auto& orangetempPhysBod = ECS::GetComponent<PhysicsBody>(EntityIdentifier::SecondPlayer());
	b2Body* orangebody = orangetempPhysBod.GetBody();

	//if Orange has run off the right of the screen, make him appear on the left 
	if (orangetempPhysBod.GetPosition().x > 202) {
		orangetempPhysBod.GetBody()->SetTransform(b2Vec2(-202, orangebody->GetPosition().y), float32(0));
	}
	//if Orange has run off the left of the screen, make him appear on the right
	else if (orangetempPhysBod.GetPosition().x < -202) {
		orangetempPhysBod.GetBody()->SetTransform(b2Vec2(202, orangebody->GetPosition().y), float32(0));
	}

	//apply some gravity for blue and orange 
	bluebody->SetLinearVelocity(b2Vec2(bluebody->GetLinearVelocity().x, bluebody->GetLinearVelocity().y - (9075 * (Timer::deltaTime * Timer::deltaTime))));
	orangebody->SetLinearVelocity(b2Vec2(orangebody->GetLinearVelocity().x, orangebody->GetLinearVelocity().y - (9075 * (Timer::deltaTime * Timer::deltaTime))));
	//printf("y velo: %f\n\n", bluebody->GetLinearVelocity().y);
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
	vec3 force = vec3(15000.f, 0.f, 0.f);
	vec3 force2 = vec3(0.f, 3000.f, 0.f);

	//grab a reference to blue's physics body
	auto& tempPhysBod = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
	//create a pointer to blue's box2d body
	b2Body* bluebody = tempPhysBod.GetBody();
	//create a copy of blue's velocity
	b2Vec2 bluevel = bluebody->GetLinearVelocity();

	//if blue's player is pressing A, and blue's x velocity greater than -120, subtract 30 from blue's x velocity	
	if (Input::GetKey(Key::A) && bluevel.x > float32(-120.f)) {
		bluevel.x -= 30.f;
		bluebody->SetLinearVelocity(bluevel);
	}

	//if blue's player is pressing D, and blue's x velocity less than 120, add 30 to blue's x velocity
	else if (Input::GetKey(Key::D) && bluebody->GetLinearVelocity().x < float32(120.f)) {
		bluevel.x += 30.f;
		bluebody->SetLinearVelocity(bluevel);
	}

	//otherwise, blue isn't trying to move left or right
	else {
		//if blue is moving right 
		if (bluevel.x > float32(0.f))
		{
			//if her x velocity is greater than 30, subtract 30 from it
			if (bluevel.x > float32(30.f)) {
				bluevel = b2Vec2(bluevel.x - 30, bluevel.y);
				bluebody->SetLinearVelocity(bluevel);
			}
			//otherwise it must be between 30 and 0, so just set it to 0
			else {
				bluevel = b2Vec2(0.f, bluevel.y);
				bluebody->SetLinearVelocity(bluevel);
			}
		}
		//or if blue is moving left
		else if (bluevel.x < float32(0.f))
		{
			//if her x velocity is less than -30, add 30 to it 
			if (bluevel.x < float32(-30.f)) {

				bluevel = b2Vec2(bluevel.x + 30, bluevel.y);
				bluebody->SetLinearVelocity(bluevel);
			}
			//otherwise it must be between -30 and 0, so just set it to 0
			else {
				bluevel = b2Vec2(0, bluevel.y);
				bluebody->SetLinearVelocity(bluevel);
			}
		}
	}

	//grab a reference to orange's physics body
	auto& tempPhysBodO = ECS::GetComponent<PhysicsBody>(EntityIdentifier::SecondPlayer());
	//create a pointer to orange's box2d body
	b2Body* bodyO = tempPhysBodO.GetBody();
	//create a copy of orange's velocity
	b2Vec2 orangevel = bodyO->GetLinearVelocity();

	//if orange's player is pressing leftArrow, and orange's x velocity greater than -120, subtract 30 from orange's x velocity
	if (Input::GetKey(Key::LeftArrow) && orangevel.x > float32(-120.f)) {
		orangevel = b2Vec2(orangevel.x - 30, orangevel.y);
		bodyO->SetLinearVelocity(orangevel);
	}

	//if orange's player is pressing rightArrow, and orange's x velocity less than 120, add 30 to orange's x velocity
	else if (Input::GetKey(Key::RightArrow) && orangevel.x < float32(120.f)) {
		orangevel = b2Vec2(orangevel.x + 30, orangevel.y);
		bodyO->SetLinearVelocity(orangevel);
	}

	//otherwise, orange isn't trying to move left or right
	else {
		//if he is moving right 
		if (orangevel.x > float32(0.f))
		{
			//and his x velocity is greater than 30, subtract 30 from it 
			if (orangevel.x > float32(30.f)) {
				orangevel = b2Vec2(orangevel.x - 30, orangevel.y);
				bodyO->SetLinearVelocity(orangevel);
			}
			//otherwise it must be between 0 and 30, so just set it to 0
			else {
				orangevel = b2Vec2(0.f, orangevel.y);
				bodyO->SetLinearVelocity(orangevel);
			}
		}
		//otherwise, if he's moving left
		else if (orangevel.x < float32(0.f))
		{
			//and his x velocity is less than -30, add 20 to it 
			if (orangevel.x < float32(-30.f)) {

				orangevel = b2Vec2(orangevel.x + 30, orangevel.y);
				bodyO->SetLinearVelocity(orangevel);
			}
			//otherwise it must be between 0 and -30, so just set it to 0
			else {
				orangevel = b2Vec2(0.f, orangevel.y);
				bodyO->SetLinearVelocity(orangevel);
			}
		}
	}

	vec3 jump = vec3(0.f, 180000.f, 0.f);

	if (Input::GetKey(Key::W)) {
		//Check if Blue can jump 
		if (listener.getJumpB() && blueTimeSinceLastJump > 0.5f) {
			//if she can, set it so she can't 
			listener.setJumpB(false);
			blueTimeSinceLastJump = 0.f;
			//and apply the upward force of the jump
			bluebody->SetLinearVelocity(b2Vec2(bluevel.x, 110.f));
		}
	}

	//Check if Orange can jump
	if (Input::GetKey(Key::UpArrow)) {
		if (listener.getJumpO() && orangeTimeSinceLastJump > 0.5f) {
			//if he can, set it so he can't 
			listener.setJumpO(false);
			orangeTimeSinceLastJump = 0.f;
			//and apply the upward force of the jump
			bodyO->SetLinearVelocity(b2Vec2(orangevel.x, 110.f));
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
