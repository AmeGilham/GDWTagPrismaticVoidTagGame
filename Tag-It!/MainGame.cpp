#include "MainGame.h"

//Constructor 
MainGame::MainGame(std::string name)
	: Scene(name)
{
	m_gravity = b2Vec2(float32(0.f), float32(-150.f));
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
		ECS::GetComponent<Camera>(entity).Zoom(75.f);
		vec4 size = ECS::GetComponent<Camera>(entity).GetOrthoSize();

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
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 6, 6, true, &animController);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 2.5f, 99.f+0.1f*i));

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
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 154, 57);
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
		tempDef.position.Set(float32(0.f), float32(-7.5f));

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
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 8, 4);
			//setup transform component
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(2.5f, 0.f, -10.f));

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
			if(i ==0 )tempDef.position.Set(float32(-36.25f), float32(-5.f)); //bottom left
			else if(i == 1)tempDef.position.Set(float32(0.f), float32(-5.f)); //bottom middle
			else if (i == 2)tempDef.position.Set(float32(36.25f), float32(-5.f)); //bottom right
			else if (i == 3)tempDef.position.Set(float32(-18.f), float32(5.5f)); //middle left
			else if (i == 4)tempDef.position.Set(float32(18.f), float32(5.5f)); //midle right
			else if (i == 5)tempDef.position.Set(float32(-36.25f), float32(15.f)); //top left
			else if (i == 6)tempDef.position.Set(float32(0.f), float32(15.f)); //top middle
			else if (i == 7)tempDef.position.Set(float32(36.25f), float32(15.f));//top right

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
	vec3 runforce = vec3(2000.f, 0.f, 0.f);

	//grab a reference to blue's physics body
	auto& tempPhysBodB = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
	//create a pointer to Blue's box2d body
	b2Body* bodyB = tempPhysBodB.GetBody();

	//if Blue's player is pressing A, and their x-velocity isn't above the left cap, apply the run force to the left
	if (Input::GetKey(Key::A) && bodyB->GetLinearVelocity().x > float32(-40.f)) {
		tempPhysBodB.ApplyForce(-runforce);
	}

	//if Blue's player is pressing D, and their x-velocity isn't above the right cap, apply the run force to the right
	else if (Input::GetKey(Key::D) && bodyB->GetLinearVelocity().x < float32(40.f)) {
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
	if (Input::GetKey(Key::LeftArrow) && bodyO->GetLinearVelocity().x > float32(-40.f)) {
		tempPhysBodO.ApplyForce(-runforce);
	}

	//if Orange's player is pressing rightArrow, and their x-velocity isn't above the right cap, apply the run force to the right
	else if (Input::GetKey(Key::RightArrow) && bodyO->GetLinearVelocity().x < float32(40.f)) {
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
	vec3 jump = vec3(0.f, 5750.f, 0.f);

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
