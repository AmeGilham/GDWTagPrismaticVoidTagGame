#include "MainGame.h"
#include "EffectManager.h"

//Constructor 
MainGame::MainGame(std::string name)
	: Scene(name)
{
	//create the gravity vector for the scene's dynamic objects
	m_gravity = b2Vec2(float32(0.f), float32(0.f));
	//set the gravity vector to be active for the scene
	m_physicsWorld->SetGravity(m_gravity);
	//setup the box2d contact listener 
	m_physicsWorld->SetContactListener(&listener);
}

//Initlize the scene
void MainGame::InitScene(float windowWidth, float windowHeight, int level){
	//Dynamically allocates the register (so when you unload the scene when you switch between scenes
	//you can later reInit this scene
	m_sceneReg = new entt::registry;

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	if (level == 1) { //code for creating/ init-ing level 1 (waterfall)
		level1(windowWidth, windowHeight);
	}

	else if (level == 2) {
		level2(windowWidth, windowHeight);
	}

}

//Update the scene, every frame
void MainGame::Update(){
	//NOT IT OBJECTIVE
	if (objective == true) {
		objective = false;

		auto entity = ECS::CreateEntity();
		//adds components 
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Spawn>(entity);

		//setup components 
		std::string fileName = "Not-It!.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 8, 2);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 17.35f, 30.f));
		ECS::GetComponent<Spawn>(entity).SetObj(true);

		//grab references to the sprite and physics body components
		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		//calculate the area of the sprite that shouldn't have a physics body attached (empty space, etc.)
		float shrinkX = tempSpr.GetWidth() / 4; //still off, adjust
		float shrinkY = 0;

		//setup the static box2d physics body
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		//set the position
		tempDef.position.Set(float32(0.f), float32(17.35f));

		//add the physics body to box2D physics world simulator
		tempBody = m_physicsWorld->CreateBody(&tempDef);

		//create a spriteLib physics body using the box2D physics body
		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false); //still off, adjust

		tempBody->SetFixedRotation(true);

		//set up user data to indentify as a border (players can't jump through the bottom)
		tempBody->SetUserData(&notItObjective);

		//Setup indentifier 
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Not-It! obejctive");
		ECS::SetIsNotItObjective(entity, true);
	}

	//grab blue's physics body info
	auto& bluetempPhysBod = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
	b2Body* bluebody = bluetempPhysBod.GetBody();

	//grab orange's physics body info
	auto& orangetempPhysBod = ECS::GetComponent<PhysicsBody>(EntityIdentifier::SecondPlayer());
	b2Body* orangebody = orangetempPhysBod.GetBody();

	//grab both's animation controller
	auto& blueAnimController = ECS::GetComponent<AnimationController>(EntityIdentifier::MainPlayer());
	auto& orangeAnimController = ECS::GetComponent<AnimationController>(EntityIdentifier::SecondPlayer());
	
	//add the change in time to the time since blue and orange last jumped (used to control jumping with platforms reseting jumps)
	blueTimeSinceLastJump += Timer::deltaTime;
	orangeTimeSinceLastJump += Timer::deltaTime;
	//add the change in time to the time since a tag was triggered
	timeSinceTagTriggered += Timer::deltaTime;
	//add the change in time to the time since the player last slid
	timeSinceSlideB += Timer::deltaTime;
	timeSinceSlideO += Timer::deltaTime;

	//apply gravity to both characters 
	bluetempPhysBod.ApplyForce(vec3(0.f, -300.f * 60.f * Timer::deltaTime, 0.f));
	orangetempPhysBod.ApplyForce(vec3(0.f, -300.f * 60.f * Timer::deltaTime, 0.f));



	//if Blue has run off the right of the screen, make her appear on the left
	if (bluetempPhysBod.GetPosition().x > 50.5) {
		bluetempPhysBod.GetBody()->SetTransform(b2Vec2(-50.5, bluebody->GetPosition().y), float32(0));}
	//if Blue has run off the left of the screen, make her appear on the right
	else if (bluetempPhysBod.GetPosition().x < -50.5) {
		bluetempPhysBod.GetBody()->SetTransform(b2Vec2(50.5, bluebody->GetPosition().y), float32(0));}

	//if Orange has run off the right of the screen, make him appear on the left 
	if (orangetempPhysBod.GetPosition().x > 50.5) {
		orangetempPhysBod.GetBody()->SetTransform(b2Vec2(-50.5, orangebody->GetPosition().y), float32(0));}
	//if Orange has run off the left of the screen, make him appear on the right
	else if (orangetempPhysBod.GetPosition().x < -50.5) {
		orangetempPhysBod.GetBody()->SetTransform(b2Vec2(50.5, orangebody->GetPosition().y), float32(0));}

	//TAGGING
	if (timeSinceTagTriggered > 0.116f && tagExists) {
		destroyT();
		//if blue is it 
		if (playerWhoTriggedTag == 1) {
			//set the correct regular animation based on the current tag animation
			blueAnimController.SetActiveAnim(blueAnimController.GetActiveAnim() - 6);
			//make sure it's on the right frame
			blueAnimController.GetAnimation(blueAnimController.GetActiveAnim()).SetCurrentFrameIndex(tagFrame++);
		}
		//otherwise, if orange is it 
		else if (playerWhoTriggedTag == 2) {
			//set the correct regular animation based on the current tag animation
			orangeAnimController.SetActiveAnim(orangeAnimController.GetActiveAnim() - 6);
			//make sure it's on the right frame
			orangeAnimController.GetAnimation(orangeAnimController.GetActiveAnim()).SetCurrentFrameIndex(tagFrame++);
		}
		tagFrame = -1;
	}
	else if (tagExists) {
		b2Body* tagBody = ECS::GetComponent<PhysicsBody>(tagEntity).GetBody();
		if (listener.GetIt() == 1) {
			if (bright) {
				tagBody->SetTransform(b2Vec2(bluebody->GetPosition().x + 3.5, bluebody->GetPosition().y), bluebody->GetAngle());
			}
			else {
				tagBody->SetTransform(b2Vec2(bluebody->GetPosition().x - 3.5, bluebody->GetPosition().y), bluebody->GetAngle());
			}
		}
		else if (listener.GetIt() == 2) {
			if (oright) {
				tagBody->SetTransform(b2Vec2(orangebody->GetPosition().x + 3.5, orangebody->GetPosition().y), orangebody->GetAngle());
			}
			else {
				tagBody->SetTransform(b2Vec2(orangebody->GetPosition().x - 3.5, orangebody->GetPosition().y), orangebody->GetAngle());
			}
		}
	}

	//if the person whose it has changed
	if (listener.GetItChange()) {
		listener.SetItChange(false);

		//check if it was through the not it objective 
		if (listener.GetNotItObjExists()) {
			//if it was, set the not it objective to not exist
			listener.SetNotItObjExists(false);
			/*fill rest of code about destroying not it objective later*/
			destroy();
		}

		//check if blue is now it 
		if (listener.GetIt() == 1) {
			if (itIdentifyingHudEntity != 0){
				ECS::DestroyEntity(itIdentifyingHudEntity);
				itIdentifyingHudEntity = 0;}

			//create entity showing that blue is it
			{
				//creates entity
				auto entity = ECS::CreateEntity();

				//adds components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);
				
				std::string fileName = "Blue It.png";
				//setup up sprite
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 18, 17);
				//Setup transform 
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 99.f));
		
				//Setup indentifier 
				unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
				ECS::SetUpIdentifier(entity, bitHolder, "Blue It Hud");
				itIdentifyingHudEntity = entity;}

			/*fill rest of code about showing that blue is now it*/
		}
		//if not, then orange must now be it 
		else {
			if (itIdentifyingHudEntity != 0){
				ECS::DestroyEntity(itIdentifyingHudEntity);
				itIdentifyingHudEntity = 0;}
			//create entity showing that orange is it
			{
				//creates entity
				auto entity = ECS::CreateEntity();

				//adds components
				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<Transform>(entity);

				//load sprites and set up sprite component
				std::string fileName = "Orange It.png";
				ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 18, 17);
				//Setup transform 
				ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 99.f));

				//Setup indentifier 
				unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
				ECS::SetUpIdentifier(entity, bitHolder, "Orange It Hud");
				itIdentifyingHudEntity = entity;
			}
			/*fill rest of code about showing that orange is now it*/
		}
	}

	//HUD 
	//if Blue is currently it
	if (listener.GetIt() == 1) {
		/*fill in code about showing blue is it, and having her bomb fuse burn*/
		blueFuseTimeRemaining -= Timer::deltaTime;
		float timeLeftRatio = blueFuseTimeRemaining / maxTime;
		float spaceLeftOnFuse = timeLeftRatio * 463.f;
		spaceLeftOnFuse = std::round(spaceLeftOnFuse);
		//printf("%f\n", blueFuseTimeRemaining);
		if (std::fmod(spaceLeftOnFuse, 58.f) == 0.f) {
			auto& animController = ECS::GetComponent<AnimationController>(bombs[0]);
			auto& anim = animController.GetAnimation(0);
			int frameId = anim.GetCurrentFrameIndex();
			anim.frameMod(UVCoordinates(vec2(anim.GetCurrentFrame().m_bottomLeft), vec2(232 + spaceLeftOnFuse, anim.GetCurrentFrame().m_topRight.y)), frameId);
			ECS::GetComponent<Sprite>(bombs[0]).SetWidth(4 + std::round(8 * timeLeftRatio));
			ECS::GetComponent<Transform>(bombs[0]).SetPosition(vec3(-15.f - 0.5f * (12 - ECS::GetComponent<Sprite>(bombs[0]).GetWidth()), -17.f, 98.f));
		}
		ECS::GetComponent<Transform>(bombs[2]).SetPosition (  vec3 (-17.f + (8.f * timeLeftRatio),hudBurningYPos(timeLeftRatio),98.8f) );
		itAnimB(); //function for it animation
	}
	//Or if Orange is currently it 
	else if (listener.GetIt() == 2) {
		/*fill in code about showing Orange is it, and having his bomb fuse burn*/
		orangeFuseTimeRemaining -= Timer::deltaTime;
		float timeLeftRatio = orangeFuseTimeRemaining / maxTime;
		float spaceLeftOnFuse = timeLeftRatio * 463.f;
		spaceLeftOnFuse = std::round(spaceLeftOnFuse);
		printf("%f\n", orangeFuseTimeRemaining);
		if (std::fmod(spaceLeftOnFuse, 58.f) == 0.f) {
			auto& animController = ECS::GetComponent<AnimationController>(bombs[1]);
			auto& anim = animController.GetAnimation(0);
			int frameId = anim.GetCurrentFrameIndex();
			anim.frameMod(UVCoordinates(vec2(463 - spaceLeftOnFuse, anim.GetCurrentFrame().m_bottomLeft.y), vec2(anim.GetCurrentFrame().m_topRight)), frameId);
			ECS::GetComponent<Sprite>(bombs[1]).SetWidth(4 + std::round(8 * timeLeftRatio));
			ECS::GetComponent<Transform>(bombs[1]).SetPosition(vec3(15.f + 0.5f * (12 - ECS::GetComponent<Sprite>(bombs[1]).GetWidth()), -17.f, 98.f));
		}
	ECS::GetComponent<Transform>(bombs[2]).SetPosition(vec3(17.f - (8.f * timeLeftRatio), hudBurningYPos(timeLeftRatio), 98.8f));
	itAnimO();
	}

	//PLAYER ANIMATIONS 
	//make sure it's not on a tag animation first
	if (!tagExists) {
		//check which animation blue should be playing
			//blue idle 
		if ((blueAnimController.GetActiveAnim() != 12 && blueAnimController.GetActiveAnim() != 13) || timeSinceSlideB > 0.332f) {
			if (bluetempPhysBod.GetBody()->GetLinearVelocity().x == 0) {
				//if blue isn't moving, check that the direction she's facing, and if it's not already playing, play the apporiate idle animation
				if (bright && blueAnimController.GetActiveAnim() != 1) blueAnimController.SetActiveAnim(1);
				else if (!bright && blueAnimController.GetActiveAnim() != 0) blueAnimController.SetActiveAnim(0);
			}
			//blue run
			else if (bluetempPhysBod.GetBody()->GetLinearVelocity().x != 0) {
				//if blue is only moving on the x-axis, check that the direction she's facing, and if it's not already playing, play the apporiate run animation
				if (bright && blueAnimController.GetActiveAnim() != 3) blueAnimController.SetActiveAnim(3);
				else if (!bright && blueAnimController.GetActiveAnim() != 2) blueAnimController.SetActiveAnim(2);
			}
		}
		//check which animation orange should be playing
		if ((orangeAnimController.GetActiveAnim() != 12 && orangeAnimController.GetActiveAnim() != 13) || timeSinceSlideO > 0.332f) {
			//orange idle 
			if (orangetempPhysBod.GetBody()->GetLinearVelocity().x == 0) {
				//if orange isn't moving, check that the direction he's facing, and if it's not already playing, play the apporiate idle animation
				if (oright && orangeAnimController.GetActiveAnim() != 1) orangeAnimController.SetActiveAnim(1);
				else if (!oright && orangeAnimController.GetActiveAnim() != 0) orangeAnimController.SetActiveAnim(0);
			}
			//orange run 
			else if (orangetempPhysBod.GetBody()->GetLinearVelocity().x != 0) {
				//if orange is only moving on the x-axis, check that the direction he's facing, and if it's not already playing, play the apporiate run animation
				if (oright && orangeAnimController.GetActiveAnim() != 3) orangeAnimController.SetActiveAnim(3);
				else if (!oright && orangeAnimController.GetActiveAnim() != 2) orangeAnimController.SetActiveAnim(2);
			}
		}
	}

	//end sliding
	//blue
	if (timeSinceSlideB > 0.332f && blueSlide) {
		//say blue is no longer sliding
		blueSlide = false; 
		//reform her hithox
		bluetempPhysBod.SetCenterOffset(vec2(0.f, 0.f));
		bluetempPhysBod.SetHeight(17.f - (17.f / 1.45f));

		//recreating the box2d collision box
		b2PolygonShape tempShape;
		tempShape.SetAsBox(float32(bluetempPhysBod.GetWidth() / 2.f), float32(bluetempPhysBod.GetHeight() / 2.f), b2Vec2(float32(bluetempPhysBod.GetCenterOffset().x), float32(bluetempPhysBod.GetCenterOffset().y)), float32(0.f));
		b2FixtureDef fix;
		fix.shape = &tempShape;
		fix.density = 0.08f;
		fix.friction = 0.35f;//0.3f
		bluebody->DestroyFixture(bluebody->GetFixtureList()); //destroys body's fixture
		bluebody->CreateFixture(&fix); //recreates it with smaller hitbox

	}
	//orange
	if (timeSinceSlideO > 0.332f && orangeSlide) {
		orangeSlide = false;
		//reform his hithox
		orangetempPhysBod.SetCenterOffset(vec2(0.f, 0.f));
		orangetempPhysBod.SetHeight(17.f - (17.f / 1.45f));

		//recreating the box2d collision box
		b2PolygonShape tempShape;
		tempShape.SetAsBox(float32(orangetempPhysBod.GetWidth() / 2.f), float32(orangetempPhysBod.GetHeight() / 2.f), b2Vec2(float32(orangetempPhysBod.GetCenterOffset().x), float32(orangetempPhysBod.GetCenterOffset().y)), float32(0.f));
		b2FixtureDef fix;
		fix.shape = &tempShape;
		fix.density = 0.08f;
		fix.friction = 0.35f;//0.3f
		orangebody->DestroyFixture(orangebody->GetFixtureList()); //destroys body's fixture
		orangebody->CreateFixture(&fix); //recreates it with smaller hitbox
	}

	//printf("%f\n", 1.0 / Timer::deltaTime);
}

//to destroy the not it objective
void MainGame::destroy(){
	auto view = m_sceneReg->view<EntityIdentifier>();

	for (auto entity : view) {

		if (m_sceneReg->has<Spawn>(entity)) {
			if (ECS::GetComponent<Spawn>(entity).GetObj()) { //objective
				ECS::DestroyEntity(entity);
			}
		}
	}
}

void MainGame::createT(int ud){
	if (ud == 4) {
		{
			auto& blue = ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer());
			auto& body = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
			float px = blue.GetPositionX();
			float py = blue.GetPositionY();

			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Spawn>(entity);

			ECS::GetComponent<Transform>(entity).SetPosition(vec3(px, py, 99.f));

			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
			auto& tagid = ECS::GetComponent<Spawn>(entity);

			tagid.SetBlue(true);
			//setup the dynamic box2d physics body

			b2Joint* tagb;
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			//set the position
			if (bright) {//if blue is facing right
				tempDef.position.Set(float32(px + 3.5), float32(py)); //blue right position
			}
			else { //has to face left
				tempDef.position.Set(float32(px - 3.5), float32(py)); //blue left position
			}

			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);

			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, 3.f, 2.f, vec2(0.f, -0.5f), false);

			//setup the user data to identify them as players
			tempBody->SetUserData(&btag); //blue
			unsigned int bitHolder = EntityIdentifier::TransformBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Tag Box Blue");

			tagExists = true;
			tagEntity = entity;
		}
		playerWhoTriggedTag = 1;
		auto& blueAnimController = ECS::GetComponent<AnimationController>(EntityIdentifier::MainPlayer());
		//grab the current frame of the current animation
		tagFrame = blueAnimController.GetAnimation(blueAnimController.GetActiveAnim()).GetCurrentFrameIndex();
		//set the correct tag animation based on the current regular animation
		blueAnimController.SetActiveAnim(blueAnimController.GetActiveAnim() + 6);
		//make sure it's on the right frame
		blueAnimController.GetAnimation(blueAnimController.GetActiveAnim()).SetCurrentFrameIndex(tagFrame);
	}

	else if (ud == 6) {
		{
			auto& orange = ECS::GetComponent<Transform>(EntityIdentifier::SecondPlayer());
			auto& body = ECS::GetComponent<PhysicsBody>(EntityIdentifier::SecondPlayer());
			float px = orange.GetPositionX();
			float py = orange.GetPositionY();

			auto entity = ECS::CreateEntity();
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Spawn>(entity);

			ECS::GetComponent<Transform>(entity).SetPosition(vec3(px, py, 99.f));

			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
			auto& tagid = ECS::GetComponent<Spawn>(entity);

			tagid.SetOrange(true); //set it as orange so it will be flagged for destruction
			//setup the dynamic box2d physics body

			b2Joint* tagb;
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			//set the position
			if (oright) {//if orange is facing right
				tempDef.position.Set(float32(px + 3.5), float32(py)); //orange position
			}
			else {
				tempDef.position.Set(float32(px - 3.5), float32(py)); //orange position
			}

			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);

			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, 3.f, 2.f, vec2(0.f, -0.5f), false);

			//setup the user data to identify them as players
			tempBody->SetUserData(&otag); //orange
			unsigned int bitHolder = EntityIdentifier::TransformBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Tag Box Orange");

			tagExists = true;
			tagEntity = entity;
		}
		playerWhoTriggedTag = 2;
		auto& orangeAnimController = ECS::GetComponent<AnimationController>(EntityIdentifier::SecondPlayer());
		//grab the current frame of the current animation
		tagFrame = orangeAnimController.GetAnimation(orangeAnimController.GetActiveAnim()).GetCurrentFrameIndex();
		//set the correct tag animation based on the current regular animation
		orangeAnimController.SetActiveAnim(orangeAnimController.GetActiveAnim() + 6);
		//make sure it's on the right frame
		orangeAnimController.GetAnimation(orangeAnimController.GetActiveAnim()).SetCurrentFrameIndex(tagFrame);
	}
}

//to destroy the tag boxes
void MainGame::destroyT(){
	auto view = m_sceneReg->view<EntityIdentifier>();

	for (auto entity : view) {

		if (m_sceneReg->has<Spawn>(entity)) {

			if (ECS::GetComponent<Spawn>(entity).GetBlue()) { //for destroying tagging boxes
				ECS::DestroyEntity(entity);
			}
			
			else if (ECS::GetComponent<Spawn>(entity).GetOrange()) { //for destroying tagging boxes
				ECS::DestroyEntity(entity);
			}

		}
	}
	tagEntity = 0;
	tagExists = false;
}

//code for level 1 (waterfall level)
void MainGame::level1(float windowWidth, float windowHeight){
	float aspectRatio = windowWidth / windowHeight;
	//Setup main camera entity
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
		EntityIdentifier::MainCamera(entity);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<Spawn>(entity);

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
			ECS::AttachComponent<Spawn>(entity);

			//Sets up components
			std::string fileName = "orange 2.png"; //set the default sprite sheet to be Orange's
			if (i == 0) fileName = "blue 2.png"; //if the first player is being created, make it blue
			//grab a reference to the animation controler
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			//set the spritesheet 
			animController.InitUVs(fileName);

			//setup the sprite and transform compoments 
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 17, 17, true, &animController);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 2.5f, 25.f + 0.1f * i));

			//grab references to the sprite and physic body compoments
			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			//calculate the area of the sprite that shouldn't have a physics body attached (empty space, ponytail/necklace, etc.)
			float shrinkX = tempSpr.GetWidth() / 1.25f;
			float shrinkY = tempSpr.GetWidth() / 1.45f;

			//setup the dynamic box2d physics body
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			//set the position
			if (i == 0) tempDef.position.Set(float32(-18.f), float32(-7.5f)); //blue position
			else tempDef.position.Set(float32(18.f), float32(-7.5f)); //orange position

			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);
			tempBody->SetFixedRotation(true);
			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//setup the user data to identify them as players
			if (i == 0) tempBody->SetUserData(&blue); //blue
			else tempBody->SetUserData(&orange); //orange

			//add their animations, and make sure they're set to be repeating
			//IDLE 
			//idle while facing left
			animController.AddAnimation(Animation());//0
			auto& idleLeft = animController.GetAnimation(0);
			createAnimation(&idleLeft, 0, 0, 250, 250, 6, false, 0.083f, true);
			//idle while facing right 
			animController.AddAnimation(Animation());//1
			auto& idleRight = animController.GetAnimation(1);
			createAnimation(&idleRight, 0, 0, 250, 250, 6, true, 0.083f, true);

			//RUN 
			//run while facing left
			animController.AddAnimation(Animation());//2
			auto& runLeft = animController.GetAnimation(2);
			createAnimation(&runLeft, 0, 250, 250, 250, 6, false, 0.083f, true);
			//run while facing right
			animController.AddAnimation(Animation());//3
			auto& runRight = animController.GetAnimation(3);
			createAnimation(&runRight, 0, 250, 250, 250, 6, true, 0.083f, true);

			//JUMP
			//jump while facing left
			animController.AddAnimation(Animation());//4
			auto& jumpLeft = animController.GetAnimation(4);
			createAnimation(&jumpLeft, 0, 500, 250, 250, 6, false, 0.083f, true);
			//jump while facing right
			animController.AddAnimation(Animation());//5
			auto& jumpRight = animController.GetAnimation(5);
			createAnimation(&jumpRight, 0, 500, 250, 250, 6, true, 0.083f, true);

			//TAGGING IDLE
			//tag while otherwise idle, left
			animController.AddAnimation(Animation());//6
			auto& idleTagLeft = animController.GetAnimation(6);
			createAnimation(&idleTagLeft, 0, 750, 250, 250, 6, false, 0.083f, true);
			//tag while otherwise idle, right
			animController.AddAnimation(Animation());//7
			auto& idleTagRight = animController.GetAnimation(7);
			createAnimation(&idleTagRight, 0, 750, 250, 250, 6, true, 0.083f, true);

			//TAGGING RUN
			//tag while running to the left
			animController.AddAnimation(Animation());//8
			auto& runTagLeft = animController.GetAnimation(8);
			createAnimation(&runTagLeft, 0, 1000, 250, 250, 6, false, 0.083f, true);
			//tag while running to the right
			animController.AddAnimation(Animation());//9
			auto& runTagRight = animController.GetAnimation(9);
			createAnimation(&runTagRight, 0, 1000, 250, 250, 6, true, 0.083f, true);

			//TAGGING Jump
			//tag while jumping and facing to the left
			animController.AddAnimation(Animation());//10
			auto& jumpTagLeft = animController.GetAnimation(10);
			createAnimation(&jumpTagLeft, 0, 1250, 250, 250, 6, false, 0.083f, true);
			//tag while jumping and facing to the left
			animController.AddAnimation(Animation());//11
			auto& jumpTagRight = animController.GetAnimation(11);
			createAnimation(&jumpTagRight, 0, 1250, 250, 250, 6, true, 0.083f, true);

			//SLIDING 
			//slide while facing left
			animController.AddAnimation(Animation());//12
			auto& slideLeft = animController.GetAnimation(12);
			createAnimation(&slideLeft, 0, 1500, 250, 250, 4, false, 0.083f, true);
			//slide while facing right
			animController.AddAnimation(Animation());//13
			auto& slideRight = animController.GetAnimation(13);
			createAnimation(&slideRight, 0, 1500, 250, 250, 4, true, 0.083f, true);

			//set the active animations so that they're facing the right direction when they spawn
			if (i == 0) animController.SetActiveAnim(1);
			else animController.SetActiveAnim(0);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Player " + std::to_string(i + 1));
			std::cout << " ORIINAL " << tempPhsBody.GetHeight() << std::endl;
			std::cout << " ORIINAL X " << tempPhsBody.GetCenterOffset().x << std::endl;
			std::cout << " ORIINAL Y " << tempPhsBody.GetCenterOffset().y << std::endl;


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
			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY), vec2(0.f, 0.f), false);

			//set up user data to indentify as a border (players can't jump through the bottom)
			tempBody->SetUserData(&border);

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Plank " + std::to_string(i + 1));
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
			if (i == 0)tempDef.position.Set(float32(-60.f), float32(-7.f));
			else if (i == 1)tempDef.position.Set(float32(-50.f), float32(-7.f));
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
			ECS::SetUpIdentifier(entity, bitHolder, "Log platform " + std::to_string(i + 1));
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
			if (i > 11) ECS::GetComponent<Transform>(entity).SetPosition(vec3(-60.f + (10.f * (i - 12)), 9.f, 11.f + 0.01 * i));

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Rope " + std::to_string(i));
		}
	}

	//vertical segements
	//vertical planks
	for (int i = 0; i < 4; i++) {
		{
			//Creates entity
			auto entity = ECS::CreateEntity();

			//adds components 
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//loadsprite sheet and set up sprite component
			std::string fileName = "vert planks.png";
			if (i < 2)ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 2, 10);
			else ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 3, 10);
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
			else tempDef.position.Set(float32(-2.f + (3.f * (i - 2))), float32(24.f));
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
	for (int i = 0; i < 2; i++) {
		{
			//creates entity
			auto entity = ECS::CreateEntity();

			//adds components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);

			//load sprites and set up sprite component
			std::string fileName = "blue bomb.png";
			if (i == 1) fileName = "orange bomb.png";
			//grab a reference to the animation controler
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			//set the spritesheet 
			animController.InitUVs(fileName);

			//setup up sprite
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 12, 4, true, &animController);
			//Setup transform 
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(-15, -17.f, 98.f + (0.01 * i)));
			if (i == 1) ECS::GetComponent<Transform>(entity).SetPosition(vec3(15, -17.f, 98.f + (0.01 * i)));

			animController.AddAnimation(Animation());
			auto& anim = animController.GetAnimation(0);
			anim.AddFrame(vec2(0, 226), vec2(695, 0));
			animController.SetActiveAnim(0);

			//Setup indentifier 
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			if (i == 0) {
				ECS::SetUpIdentifier(entity, bitHolder, "blue bomb");
				bombs[0] = entity;
			}
			else {
				ECS::SetUpIdentifier(entity, bitHolder, "orange bomb");
				bombs[1] = entity;
			}
		}
	}

	//burning fuse part of the HUD 
	{
		//creates entity
		auto entity = ECS::CreateEntity();

		//Adds components 
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		//load sprites and set up sprite component
		std::string fileName = "fuse burn.png";
		//grab a reference to the animation controler
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		//set the spritesheet 
		animController.InitUVs(fileName);

		//setup up sprite
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 4, 4, true, &animController);
		//Setup transform, start it in a place noone will be able to see it 
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-300, -300.f, 98.2f));

		animController.AddAnimation(Animation());
		auto& anim = animController.GetAnimation(0);
		anim.AddFrame(vec2(0, 100), vec2(100, 0));
		anim.AddFrame(vec2(100, 100), vec2(200, 0));
		anim.SetSecPerFrame(0.05f);
		anim.SetRepeating(true);
		animController.SetActiveAnim(0);

		//Setup indentifier 
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
		ECS::SetUpIdentifier(entity, bitHolder, "buring animation");
		bombs[2] = entity;
	}

}

//placeholder code for level 2
void MainGame::level2(float windowWidth, float windowHeight){
	float aspectRatio = windowWidth / windowHeight;

	//Setup main camera entity
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
		EntityIdentifier::MainCamera(entity);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<Spawn>(entity);

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

	//setup backdrop entity
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
			ECS::AttachComponent<Spawn>(entity);

			//Sets up components
			std::string fileName = "orange sprites.png"; //set the default sprite sheet to be Orange's
			if (i == 0) fileName = "blue sprites.png"; //if the first player is being created, make it blue
			//grab a reference to the animation controler
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			//set the spritesheet 
			animController.InitUVs(fileName);

			//setup the sprite and transform compoments 
			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 10, true, &animController);
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 2.5f, 25.f + 0.1f * i));

			//grab references to the sprite and physic body compoments
			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			//calculate the area of the sprite that shouldn't have a physics body attached (empty space, ponytail/necklace, etc.)
			float shrinkX = tempSpr.GetWidth() / 1.5f;
			float shrinkY = tempSpr.GetWidth() / 2.f;

			//setup the dynamic box2d physics body
			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			//set the position
			if (i == 0) tempDef.position.Set(float32(-18.f), float32(-7.5f)); //blue position
			else tempDef.position.Set(float32(18.f), float32(-7.5f)); //orange position

			//add the physics body to box2D physics world simulator
			tempBody = m_physicsWorld->CreateBody(&tempDef);
			tempBody->SetFixedRotation(true);
			//create a spriteLib physics body using the box2D physics body
			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//setup the user data to identify them as players
			if (i == 0) tempBody->SetUserData(&blue); //blue
			else tempBody->SetUserData(&orange); //orange

			//add their animations, and make sure they're set to be repeating
			animController.AddAnimation(Animation());
			auto& anim0 = animController.GetAnimation(0);
			createAnimation(&anim0, 0, 0, 600, 600, 6, false, 0.083f, true);
			animController.AddAnimation(Animation());
			auto& anim1 = animController.GetAnimation(1);
			createAnimation(&anim1, 0, 0, 600, 600, 6, false, 0.083f, true);
			animController.AddAnimation(Animation());
			auto& anim2 = animController.GetAnimation(2);
			createAnimation(&anim2, 0, 0, 600, 600, 6, true, 0.083f, true);

			//set the active animations so that they're facing the right direction when they spawn
			if (i == 0) animController.SetActiveAnim(2);
			else animController.SetActiveAnim(1);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Player " + std::to_string(i + 1));
			std::cout << " ORIINAL " << tempPhsBody.GetHeight() << std::endl;
			std::cout << " ORIINAL X " << tempPhsBody.GetCenterOffset().x << std::endl;
			std::cout << " ORIINAL Y " << tempPhsBody.GetCenterOffset().y << std::endl;
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

	for (int i = 0; i < 4; i++) {
		{
			//Creates entity
			auto entity = ECS::CreateEntity();

			//adds components 
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);

			//loadsprite sheet and set up sprite component
			std::string fileName = "vert planks.png";
			if (i < 2)ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 2, 10);
			else ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 3, 10);
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
			else tempDef.position.Set(float32(-2.f + (3.f * (i - 2))), float32(24.f));
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
			if (i == 0)tempDef.position.Set(float32(-60.f), float32(-7.f));
			else if (i == 1)tempDef.position.Set(float32(-50.f), float32(-7.f));
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
			ECS::SetUpIdentifier(entity, bitHolder, "Log platform " + std::to_string(i + 1));
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
			ECS::SetUpIdentifier(entity, bitHolder, "Plank " + std::to_string(i + 1));
		}
	}


}

//keyboard key held down input
void MainGame::KeyboardHold() {
	//vector with the force for the player's x movement
	vec3 runforce = vec3(1000.f * 60.f * Timer::deltaTime, 0.f, 0.f);

	//grab a reference to blue's physics body
	auto& tempPhysBodB = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
	//create a pointer to Blue's box2d body
	b2Body* bodyB = tempPhysBodB.GetBody();

	//if Blue's player is pressing A, and their x-velocity isn't above the left cap, apply the run force to the left
	if (Input::GetKey(Key::A) && bodyB->GetLinearVelocity().x > float32(-40.f) && !(Input::GetKey(Key::S))) {
		tempPhysBodB.ApplyForce(-runforce);
	}
	//if Blue's player is pressing D, and their x-velocity isn't above the right cap, apply the run force to the right
	else if (Input::GetKey(Key::D) && bodyB->GetLinearVelocity().x < float32(40.f) && !(Input::GetKey(Key::S))) {
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
	if (Input::GetKey(Key::LeftArrow) && bodyO->GetLinearVelocity().x > float32(-40.f) && !(Input::GetKey(Key::DownArrow))) {
		tempPhysBodO.ApplyForce(-runforce);
	}

	//if Orange's player is pressing rightArrow, and their x-velocity isn't above the right cap, apply the run force to the right
	else if (Input::GetKey(Key::RightArrow) && bodyO->GetLinearVelocity().x < float32(40.f) && !(Input::GetKey(Key::DownArrow))) {
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
	vec3 jump = vec3(0.f, 5750.f * 60.f * Timer::deltaTime, 0.f);

	//if blue's player has pressed W, and she can jump, make her jump
	if (Input::GetKey(Key::W) && !(Input::GetKey(Key::S))) {
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
	if (Input::GetKey(Key::UpArrow) && !(Input::GetKey(Key::DownArrow))) {
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
void MainGame::KeyboardDown(){

	//grab a reference to blue's physics body
	auto& tempPhysBodB = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
	//create a pointer to Blue's box2d body
	b2Body* bodyB = tempPhysBodB.GetBody();

	//grab a reference to orange's physics body
	auto& tempPhysBodO = ECS::GetComponent<PhysicsBody>(EntityIdentifier::SecondPlayer());
	//create a pointer to Orange's box2d body
	b2Body* bodyO = tempPhysBodO.GetBody();
	   
	if (Input::GetKeyDown(Key::Q) && listener.GetIt() == 1 && timeSinceTagTriggered > 0.166f && !blueSlide) { //player 1 blue tagging
		createT(btag);
		timeSinceTagTriggered = 0.f;
	}

	else if (Input::GetKeyDown(Key::M) && listener.GetIt() == 2 && timeSinceTagTriggered > 0.166f && !orangeSlide) { //player 2 orange tagging
		createT(otag);
		timeSinceTagTriggered = 0.f;
	}

	if (Input::GetKeyDown(Key::D)){
		bright = true;
	}

	if (Input::GetKeyDown(Key::A)){
		bright = false;
	}

	if (Input::GetKeyDown(Key::RightArrow)) {
		oright = true;
	}
	
	if (Input::GetKeyDown(Key::LeftArrow)) {
		oright = false;
	}


	//SLIDING MECHANIC CODE
	//grab animation controller
	auto& blueAnimController = ECS::GetComponent<AnimationController>(EntityIdentifier::MainPlayer());
	auto& orangeAnimController = ECS::GetComponent<AnimationController>(EntityIdentifier::SecondPlayer());

	//blue slide
	if ((Input::GetKeyDown(Key::S) && !bright && timeSinceSlideB > 1.f)) {
		tempPhysBodB.SetCenterOffset(vec2(0.f, -1.5f));
		tempPhysBodB.SetHeight(2.f);

		//recreating the box2d collision box
		b2PolygonShape tempShape;
		tempShape.SetAsBox(float32(tempPhysBodB.GetWidth() / 2.f), float32(tempPhysBodB.GetHeight() / 2.f), b2Vec2(float32(tempPhysBodB.GetCenterOffset().x), float32(tempPhysBodB.GetCenterOffset().y)), float32(0.f));
		b2FixtureDef fix;
		fix.shape = &tempShape;
		fix.density = 0.08f;
		fix.friction = 0.35f;//0.3f
		bodyB->DestroyFixture(bodyB->GetFixtureList()); //destroys body's fixture
		bodyB->CreateFixture(&fix); //recreates it with smaller hitbox

		tempPhysBodB.ApplyForce(vec3(-6000.f, 0.f, 0.f));
		timeSinceSlideB = 0.f;
		blueSlide = true;
		blueAnimController.SetActiveAnim(12);

	}
	else if ((Input::GetKeyDown(Key::S) && bright && timeSinceSlideB > 1.f)) {
		tempPhysBodB.SetCenterOffset(vec2(0.f, -1.5f));
		tempPhysBodB.SetHeight(2.f);

		//recreating the box2d collision box
		b2PolygonShape tempShape;
		tempShape.SetAsBox(float32(tempPhysBodB.GetWidth() / 2.f), float32(tempPhysBodB.GetHeight() / 2.f), b2Vec2(float32(tempPhysBodB.GetCenterOffset().x), float32(tempPhysBodB.GetCenterOffset().y)), float32(0.f));
		b2FixtureDef fix;
		fix.shape = &tempShape;
		fix.density = 0.08f;
		fix.friction = 0.35f;//0.3f
		bodyB->DestroyFixture(bodyB->GetFixtureList()); //destroys body's fixture
		bodyB->CreateFixture(&fix); //recreates it with smaller hitbox

		tempPhysBodB.ApplyForce(vec3(6000.f, 0.f, 0.f));
		blueSlide = true;
		blueAnimController.SetActiveAnim(13);
		timeSinceSlideB = 0.f;
	}

	//orange slide
	if ((Input::GetKeyDown(Key::DownArrow) && !oright && timeSinceSlideO > 1.f)) {
		tempPhysBodO.SetCenterOffset(vec2(0.f, -1.5f));
		tempPhysBodO.SetHeight(2.f);

		//recreating the box2d collision box
		b2PolygonShape tempShape;
		tempShape.SetAsBox(float32(tempPhysBodO.GetWidth() / 2.f), float32(tempPhysBodO.GetHeight() / 2.f), b2Vec2(float32(tempPhysBodO.GetCenterOffset().x), float32(tempPhysBodO.GetCenterOffset().y)), float32(0.f));
		b2FixtureDef fix;
		fix.shape = &tempShape;
		fix.density = 0.08f;
		fix.friction = 0.35f;//0.3f
		bodyO->DestroyFixture(bodyO->GetFixtureList()); //destroys body's fixture
		bodyO->CreateFixture(&fix); //recreates it with smaller hitbox

		tempPhysBodO.ApplyForce(vec3(-6000.f, 0.f, 0.f));
		timeSinceSlideO = 0.f;
		orangeSlide = true;
		orangeAnimController.SetActiveAnim(12);

	}
	else if ((Input::GetKeyDown(Key::DownArrow) && oright && timeSinceSlideO > 1.f)) {
		tempPhysBodO.SetCenterOffset(vec2(0.f, -1.5f));
		tempPhysBodO.SetHeight(2.f);

		//recreating the box2d collision box
		b2PolygonShape tempShape;
		tempShape.SetAsBox(float32(tempPhysBodO.GetWidth() / 2.f), float32(tempPhysBodO.GetHeight() / 2.f), b2Vec2(float32(tempPhysBodO.GetCenterOffset().x), float32(tempPhysBodO.GetCenterOffset().y)), float32(0.f));
		b2FixtureDef fix;
		fix.shape = &tempShape;
		fix.density = 0.08f;
		fix.friction = 0.35f;//0.3f
		bodyO->DestroyFixture(bodyO->GetFixtureList()); //destroys body's fixture
		bodyO->CreateFixture(&fix); //recreates it with smaller hitbox

		tempPhysBodO.ApplyForce(vec3(6000.f, 0.f, 0.f));
		timeSinceSlideO = 0.f;
		orangeSlide = true;
		orangeAnimController.SetActiveAnim(13);
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

//creates frames for animation objects
void MainGame::createAnimation(Animation* anim, int x, int y, int width, int height, int frames, bool flipped, float lenghtOfFrame, bool repeating){
	//assumes horizontal animation
	//create vectors for the bottom left and top right coordinates
	vec2 bl(0, 0);
	vec2 tr(0, 0);

	//loops through each frame
	for (int i = 0; i < frames; i++)
	{
		//set the coordinates 
		bl = vec2(x+(width*i), y+height);
		tr = vec2(x+width+(width*i), y);

		//if it should be horizontally flipped, do so
		if (flipped) {
			//copy the bottom left's x into a temp variable 
			int temp = bl.x;
			//copy the top right's x into the bottom left
			bl.x = tr.x;
			//copy the bottom left's x into the top right's x
			tr.x = temp;
		}
		//add the frame to the animation 
		anim->AddFrame(bl, tr);
	}
	//Set how long each frame should last
	anim->SetSecPerFrame(lenghtOfFrame);
	//Set if the animation should repeat
	anim->SetRepeating(repeating);
}

void MainGame::itAnimB() {
	std::string fileName = "Blue It.png";
	auto& sprite = ECS::GetComponent<Sprite>(itIdentifyingHudEntity);
	auto& trans = ECS::GetComponent<Transform>(itIdentifyingHudEntity);
	//(trans.GetPositionX() >= -22.5f && trans.GetPositionY() >= -18.f)

	if (listener.GetIt() == 1) {
		animTimeO = itTime;
		animTime -= Timer::deltaTime;
		timeLeft = animTime / maxTime;}

	//else if (listener.GetIt() == 2) {
	//	animTime = itTime;
	//}
	///*else {
	//	animTime = 5.f;
	//	float timeLeft = animTime / itTime;}*/

	float i = 0.f;
	float num = 0.f;
	float q = 0.f;

	//for (int i = 0; i < 15; i++) {
	//	trans.SetPosition (vec3 (trans.GetPositionX() -1.5f, trans.GetPositionY() -1.2f, 99.f) );
	//	sprite.SetHeight(sprite.GetHeight() - 1);
	//	sprite.SetWidth(sprite.GetWidth() - 1);
	////	sprite.LoadSprite (fileName, sprite.GetHeight() - 1, sprite.GetWidth() - 1);
	//}
	std::cout << animTime << std::endl;
	/*if (trans.GetPositionX() >= -22.5f && trans.GetPositionY() >= -18.f) {
		trans.SetPosition(vec3(trans.GetPositionX() - (3.55f * timeLeft), trans.GetPositionY() - (2.8f * timeLeft), 99.f));}*/

	if (trans.GetPositionX() >= -22.5f) {
		trans.SetPositionX(trans.GetPositionX() - (7.5f * timeLeft));}

	if (trans.GetPositionY() >= -18.f) {
		trans.SetPositionY(trans.GetPositionY() - (5.99f * timeLeft));	}



	if (sprite.GetHeight() > 3 && sprite.GetWidth() > 2)
	sprite.LoadSprite(fileName, sprite.GetHeight() - (0.5 * timeLeft), sprite.GetWidth() - (0.5 * timeLeft) );

	//if ((q > 3.f) && trans.GetPositionX() != -22.5f ) {
	//	do {
	//		trans.SetPosition(vec3(trans.GetPositionX() - 1.5f, trans.GetPositionY() - 1.2f, 99.f));
	//		//sprite.LoadSprite(fileName, sprite.GetHeight() - 1, sprite.GetWidth() - 1);
	//		
	//		num = Timer::deltaTime * 10.5f;
	//		i += round(num);
	//		std::cout << round(num) << "\n";
	//	} while (i <= 2.9f);
	//	q = 0.f;
	//}
	//ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 3, 2);
	//ECS::GetComponent<Transform>(entity).SetPosition(vec3(-22.5f, -18.f, 99.f));
}

void MainGame::itAnimO(){
	std::string fileName = "Orange It.png";
	auto& sprite = ECS::GetComponent<Sprite>(itIdentifyingHudEntity);
	auto& trans = ECS::GetComponent<Transform>(itIdentifyingHudEntity);

	if (listener.GetIt() == 2 ) {
		animTime = itTime;
		animTimeO -= Timer::deltaTime;
		timeLeftO = animTimeO / maxTime;
		std::cout << "YO"<< animTimeO << std::endl;
	}
	else if (listener.GetIt() == 1){
		animTimeO = itTime;}
	

	if (sprite.GetHeight() > 3 && sprite.GetWidth() > 2) 
		sprite.LoadSprite(fileName, sprite.GetHeight() - (0.5 * timeLeftO), sprite.GetWidth() - (0.5 * timeLeftO));

	/*if (trans.GetPositionX() <= 22.5f && trans.GetPositionY() >= -18.f) {
		trans.SetPosition(vec3(trans.GetPositionX() + (6.16f * timeLeftO), trans.GetPositionY() - (5.89f * timeLeftO), 99.f));}*/

	if (trans.GetPositionX() <= 22.5f) {
		trans.SetPositionX(trans.GetPositionX() + (7.5f * timeLeftO));}

	if (trans.GetPositionY() >= -18.f) {
		trans.SetPositionY(trans.GetPositionY() - (5.99f * timeLeftO));}


}

float MainGame::round(float var){
	float value = (int)(var * 10 + .5);
	return (float)value / 10;
}

float MainGame::hudBurningYPos(double ratio)
{
	//determine the y-position of the burning hud 
	//goes down from ratio = 1 to ratio = 0.666...
	if (ratio > 0.666f) {
		double factor = 1.f - ratio; 
		factor /= 1.3f;
		return (-16.6f - factor);
	}
	//goes up from ratio = 0.666... to ratio = 0.333...
	else if (ratio <= 0.666f && ratio > 0.333) {
		double factor = 0.666f - ratio;
		factor *= 2.25;
		return (-16.856923f + factor);
	}
	//goes down from ratio = 0.3333 to ratio = 0
	else {
		double factor = 0.333f - ratio;
		factor /= 1.5f;
		return (-16.071166f - factor);
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