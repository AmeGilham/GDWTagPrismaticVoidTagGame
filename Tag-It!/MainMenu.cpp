#include "MainMenu.h"

MainMenu::MainMenu(std::string name)
    : Scene(name) {

}

void MainMenu::InitScene(float windowWidth, float windowHeight) {
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

    //play button
    {
        //Creates entity
        auto entity = ECS::CreateEntity();

        //Adds components
        ECS::AttachComponent<Sprite>(entity);
        ECS::AttachComponent<Transform>(entity);

        //sets up components
        std::string fileName = "menus/play.png";

        //sets up sprite and transform components
        ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 5);
        ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, -12.5f, 0.f));

        //Setup indentifier
        unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
        ECS::SetUpIdentifier(entity, bitHolder, "Play Button");
        ECS::SetIsMainPlayer(entity, true);
    }

	//quit button
	{
		//Creates entity
		auto entity = ECS::CreateEntity();
			
		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components
		std::string fileName = "menus/quit.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 5);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, -20.f, 2.f));

		//Setup indentifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Quit Button");
		ECS::SetIsSecondPlayer(entity, true);
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

	//character art
	for(int i = 0; i < 2; i++) {
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components
		std::string fileName = "menus/blue sticker.png";
		if(i == 1) fileName = "menus/orange sticker.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 30, 30);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-65.f, -10.f, -2.f));
		if(i == 1) ECS::GetComponent<Transform>(entity).SetPosition(vec3( 65.f, -10.f, -3.f));

		//Setup indentifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		if(i == 0) ECS::SetUpIdentifier(entity, bitHolder, "Character art blue");
		else ECS::SetUpIdentifier(entity, bitHolder, "Character art orange");
		characterArtEntities[i] = entity;
	}

	//title 
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components
		std::string fileName = "menus/title.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 32, 10);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 50.f, 10.f));

		//Setup indentifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "title");
		title = entity;
	}
}

void MainMenu::Update()
{
	//grab a reference to the transforms 
	auto& blueArt = ECS::GetComponent<Transform>(characterArtEntities[0]);
	auto& orangeArt = ECS::GetComponent<Transform>(characterArtEntities[1]);
	auto& titleTrans = ECS::GetComponent<Transform>(title);

	//bring blue in from the side
	if (blueArt.GetPositionX() < -30.f) {
		blueArt.SetPositionX(blueArt.GetPositionX() + 1.f);
	}

	//bring orange in from the side 
	if (orangeArt.GetPositionX() > 30.f) {
		orangeArt.SetPositionX(orangeArt.GetPositionX() - 1.f);
	}

	//bring the title in from the top
	if (titleTrans.GetPositionY() > 15.f) {
		titleTrans.SetPositionY(titleTrans.GetPositionY() - 1.f);
	}
}

void MainMenu::MouseClick(SDL_MouseButtonEvent evnt) { //mouse click
    printf("Mouse moved (%f, %f)\n", float(evnt.x), float(evnt.y));
}

//mouse motion
void MainMenu::MouseMotion(SDL_MouseMotionEvent evnt) {

    if ((float(evnt.x) >= 856.f && float(evnt.x) <= 1063.f) && (float(evnt.y) >= 762.f && float(evnt.y) <= 860.f)) {
        auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::MainPlayer());
        std::string fileName = "menus/play2.png";
        spr.LoadSprite(fileName, 10, 5);
    }

    else {
        auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::MainPlayer());
        std::string fileName = "menus/play.png";
        spr.LoadSprite(fileName, 10, 5);
	}

	if ((float(evnt.x) >= 856.f && float(evnt.x) <= 1063.f) && (float(evnt.y) >= 924.f && float(evnt.y) <= 1022.f)) {
		auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::SecondPlayer());
		std::string fileName = "menus/quit2.png";
		spr.LoadSprite(fileName, 10, 5);
	}

	else {
		auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::SecondPlayer());
		std::string fileName = "menus/quit.png";
		spr.LoadSprite(fileName, 10, 5);
	}

}
