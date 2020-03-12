#include "MainMenu.h"

MainMenu::MainMenu(std::string name)
	: Scene(name){

}

void MainMenu::InitScene(float windowWidth, float windowHeight){
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

	//button
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components 
		std::string fileName = "play.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 10, 5);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));

		//Setup indentifier 
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Play Button");
		ECS::SetIsMainPlayer(entity, true);
	}

	//menu background
	{
		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//sets up components 
		std::string fileName = "back.png";

		//sets up sprite and transform components
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100, 50);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, -90.f));

		//Setup indentifier 
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Background");
	}



}

void MainMenu::MouseClick(SDL_MouseButtonEvent evnt){ //mouse click
	printf("Mouse moved (%f, %f)\n", float(evnt.x), float(evnt.y));
}

//mouse motion
void MainMenu::MouseMotion(SDL_MouseMotionEvent evnt){
	
	if ((float(evnt.x) >= 393.f && float(evnt.x) <= 527.f) && (float(evnt.y) >= 307.f && float(evnt.y) <= 373.f)) {
		//SetScene(m_menu);
		printf("MOVED");
		auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::MainPlayer());
		std::string fileName = "play2.png";
		spr.LoadSprite(fileName, 10, 5);}

	else  {
		auto& spr = ECS::GetComponent<Sprite>(EntityIdentifier::MainPlayer());
		std::string fileName = "play.png";
		spr.LoadSprite(fileName, 10, 5);}



}
