#include "LevelSelect.h"

LevelSelect::LevelSelect(std::string name) : Scene(name) {
}

void LevelSelect::InitScene(float windowWidth, float windowHeight) {
    //Dynamically allocates the register (so when you unload the scene when you switch between scenes you can later re Init this scene)
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
        //ECS::AttachComponent<Spawn>(entity);

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

	//menu background
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components
		std::string fileName = "menus/BG.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100, 50);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, -90.f));

		//Setup indentifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Background");
	}

	//top text "pick your stage"
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components
		std::string fileName = "menus/pick your stage.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 46, 6);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 20.f, 90.f));

		//Setup indentifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "text prompt");
		ECS::SetIsMainPlayer(entity, true);
	}

	//arrow to go back to the main menu
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components
		std::string fileName = "menus/back button unpressed.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 6, 3);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-40.f, 23.f, 91.f));

		//Setup indentifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "back button");
		ECS::SetIsSecondPlayer(entity, true);
	}

    //setup for level 1 (waterfall)
    {
        //Creates entity
        auto entity = ECS::CreateEntity();

        //Adds components
        ECS::AttachComponent<Sprite>(entity);
        ECS::AttachComponent<Transform>(entity);

        //sets up components
        std::string fileName = "menus/level 1 unselected.png";

        //sets up sprite and transform components
        ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 15);
        ECS::GetComponent<Transform>(entity).SetPosition(vec3(-20.326f - 24.f, 6.5f, 0.f));

        //Setup indentifier
        unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
        ECS::SetUpIdentifier(entity, bitHolder, "Level 1");
		levels[0] = entity;
    }

    //setup level 2 (tbd)
    {
        //Creates entity
        auto entity = ECS::CreateEntity();

        //Adds components
        ECS::AttachComponent<Sprite>(entity);
        ECS::AttachComponent<Transform>(entity);

        //sets up components
        std::string fileName = "menus/level 2 unselected.png";

        //sets up sprite and transform components
        ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 15);
        ECS::GetComponent<Transform>(entity).SetPosition(vec3(20.326f + 24.f, 6.5f, 1.f));

        //Setup indentifier
        unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
        ECS::SetUpIdentifier(entity, bitHolder, "Level 2");
		levels[1] = entity;
    }

	//setup level 3 (tbd)
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components
		std::string fileName = "menus/level 3 unselected.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 15);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-20.326f - 24.f, -13.815f, 2.f));

		//Setup indentifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Level 3");
		levels[2] = entity;
	}

	//setup level 4 (tbd)
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components
		std::string fileName = "menus/level 4 unselected.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 25, 15);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(20.326f + 24.f, -13.815f, 3.f));

		//Setup indentifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Level 4");
		levels[3] = entity;
	}

}

void LevelSelect::MouseClick(SDL_MouseButtonEvent evnt) {
    printf("Mouse moved (%f, %f)\n", float(evnt.x), float(evnt.y));
}

void LevelSelect::MouseMotion(SDL_MouseMotionEvent evnt) {
	//waterfall level
    if ((float(evnt.x) >= 250.f && float(evnt.x) <= 790.f) && (float(evnt.y) >= 243.2f && float(evnt.y) <= 567.2f)) {
      //  printf("MOVED");
        auto& spr = ECS::GetComponent<Sprite>(levels[0]);
        std::string fileName = "menus/level 1 selected.png";
        spr.LoadSprite(fileName, 25, 15);
    }
    else {
        auto& spr = ECS::GetComponent<Sprite>(levels[0]);
        std::string fileName = "menus/level 1 unselected.png";
        spr.LoadSprite(fileName, 25, 15);
    }

	//house level
    if ((float(evnt.x) >= 1130.f && float(evnt.x) <= 1670.f) && (float(evnt.y) >= 243.2f && float(evnt.y) <= 567.2f)) {
        //  printf("MOVED");
        auto& spr = ECS::GetComponent<Sprite>(levels[1]);
		std::string fileName = "menus/level 2 selected.png";
		spr.LoadSprite(fileName, 25, 15);
    }
    else {
        auto& spr = ECS::GetComponent<Sprite>(levels[1]);
		std::string fileName = "menus/level 2 unselected.png";
		spr.LoadSprite(fileName, 25, 15);
    }

	//city level
	if ((float(evnt.x) >= 250.f && float(evnt.x) <= 790.f) && (float(evnt.y) >= 676.4f && float(evnt.y) <= 1000.4f)) {
		//  printf("MOVED");
		auto& spr = ECS::GetComponent<Sprite>(levels[2]);
		std::string fileName = "menus/level 3 selected.png";
		spr.LoadSprite(fileName, 25, 15);
	}
	else {
		auto& spr = ECS::GetComponent<Sprite>(levels[2]);
		std::string fileName = "menus/level 3 unselected.png";
		spr.LoadSprite(fileName, 25, 15);
	}

	//pyramid level
	if ((float(evnt.x) >= 1130.f && float(evnt.x) <= 1670.f) && (float(evnt.y) >= 676.4f && float(evnt.y) <= 1000.4f)) {
		//  printf("MOVED");
		auto& spr = ECS::GetComponent<Sprite>(levels[3]);
		std::string fileName = "menus/level 4 selected.png";
		spr.LoadSprite(fileName, 25, 15);
	}

	else {
		auto& spr = ECS::GetComponent<Sprite>(levels[3]);
		std::string fileName = "menus/level 4 unselected.png";
		spr.LoadSprite(fileName, 25, 15);
	}

	//back button
	if ((float(evnt.x) >= 30.24f && float(evnt.x) <= 159.84f) && (float(evnt.y) >= 10.8f && float(evnt.y) <= 75.6f)) {
		//  printf("MOVED");
		auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::SecondPlayer());
		std::string fileName = "menus/back button pressed.png";
		spr.LoadSprite(fileName, 6, 3);
	}

	else {
		auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::SecondPlayer());
		std::string fileName = "menus/back button unpressed.png";
		spr.LoadSprite(fileName, 6, 3);
	}
}

void LevelSelect::Update()
{
	//grab a reference to the transforms 
	auto& level1thumbTrans = ECS::GetComponent<Transform>(levels[0]);
	auto& level2thumbTrans = ECS::GetComponent<Transform>(levels[1]);
	auto& level3thumbTrans = ECS::GetComponent<Transform>(levels[2]);
	auto& level4thumbTrans = ECS::GetComponent<Transform>(levels[3]);

	//bring the waterfall prompt in from the side
	if (level1thumbTrans.GetPositionX() < -20.326f) {
		level1thumbTrans.SetPositionX(level1thumbTrans.GetPositionX() + 1.f);
	}

	//bring the house level prompt in from the side 
	if (level2thumbTrans.GetPositionX() > 20.326f) {
		level2thumbTrans.SetPositionX(level2thumbTrans.GetPositionX() - 1.f);
	}

	//bring the city level prompt in from the side 
	if (level3thumbTrans.GetPositionX() < -20.326f) {
		level3thumbTrans.SetPositionX(level3thumbTrans.GetPositionX() + 1.f);
	}

	//bring the pyramid level prompt in from the side 
	if (level4thumbTrans.GetPositionX() > 20.326f) {
		level4thumbTrans.SetPositionX(level4thumbTrans.GetPositionX() - 1.f);
	}

}
