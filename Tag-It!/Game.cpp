#include "Game.h"
#include <random>

Game::~Game()
{
	//If window isn't equal to nullptr
	if (m_window != nullptr)
	{
		//Delete window
		delete m_window;
		//set window to nullptr
		m_window = nullptr;
	}

	//Goes through the scenes and deletes them
	for (unsigned i = 0; i < m_scenes.size(); i++)
	{
		if (m_scenes[i] != nullptr)
		{
			delete m_scenes[i];
			m_scenes[i] = nullptr;
		}
	}
}

void Game::InitGame(){
	//Scene names and clear colors
	m_name = "Tag-It!";
	m_clearColor = vec4(0.15f, 0.33f, 0.58f, 1.f);

	//Initializes the backend
	BackEnd::InitBackEnd(m_name);

	//Grabs the initialized window
	m_window = BackEnd::GetWindow();

	//Creates a new HelloWorld scene
	m_scenes.push_back(new MainMenu("Menu"));
	m_scenes.push_back(new LevelSelect("Level Menu"));
	m_scenes.push_back(new MainGame("1v1 Match"));
	//Sets active scene reference to our scene
	m_activeScene = m_scenes[currentScene];
	//Intializes the scene
	if (currentScene == 2)m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()), level);
	else m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()));

	//Sets m_register to point to the register in the active scene
	m_register = m_activeScene->GetScene();

	//initlize the physics system
	PhysicsSystem::Init();
}

bool Game::Run()
{
	//While window is still open
	while (m_window->isOpen())
	{
		//Clear window with clearColor
		m_window->Clear(m_clearColor);
		//Updates the game
		Update();
		//Draws the game
		BackEnd::Draw(m_register);

		//Draws ImGUI
		if (m_guiActive)
			GUI();
		
		//Flips the windows
		m_window->Flip();
		
		//Polls events and then checks them
		BackEnd::PollEvents(m_register, &m_close, &m_motion, &m_click, &m_wheel);
		CheckEvents();

		//does the window have keyboard focus?
		if (Input::m_windowFocus){
			//Accept all input
			AcceptInput();
		}
	}
	//exit the program
	return true;
}

void Game::Update(){
	//Update timer
	Timer::Update();
	//Update the backend
	BackEnd::Update(m_register);
	//Update Physics System
	PhysicsSystem::Update(m_register, m_activeScene->GetPhysicsWorld());
	//Update the current scene 
	//camTime += Timer::deltaTime / 3; was trying to use to pause inputs
	m_activeScene->Update();

}

void Game::GUI()
{
	UI::Start(BackEnd::GetWindowWidth(), BackEnd::GetWindowHeight());

	ImGui::Text("Place your different tabs below.");

	if (ImGui::BeginTabBar(""))
	{
		BackEnd::GUI(m_register, m_activeScene);

		ImGui::EndTabBar();
	}

	UI::End();
}

void Game::CheckEvents(){
	if (m_close)
		m_window->Close();

	if (m_motion)
		MouseMotion(BackEnd::GetMotionEvent());

	if (m_click)
		MouseClick(BackEnd::GetClickEvent());

	if (m_wheel)
		MouseWheel(BackEnd::GetWheelEvent());
}

void Game::AcceptInput()
{
	//Just calls all the other input functions 
	KeyboardHold();
	KeyboardDown();
	KeyboardUp();

	//Resets the key flags
	//Must be done once per frame for input to work
	Input::ResetKeys();
}

void Game::KeyboardHold(){
	float camx = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).GetPositionX();
	float camy = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).GetPosition().y;
	float camz = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).GetPosition().z;
	vec4 zoom = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize();

	/*float orangex = ECS::GetComponent<Transform>(EntityIdentifier::SecondPlayer()).GetPositionX();
	float orangey = ECS::GetComponent<Transform>(EntityIdentifier::SecondPlayer()).GetPositionY();

	float bluex = ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	float bluey = ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()).GetPositionY();*/

	//keyboard button held 
	if (Input::GetKey(Key::Z)) {    //zoom in 
		m_register->get<Camera>(EntityIdentifier::MainCamera()).Zoom(1);}
	if (Input::GetKey(Key::X)) {    //zoom out 
		m_register->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-1);}

	//if (Input::GetKey(Key::C)) {    //zoom in 
	//	std::cout << bluex << std::endl;
	//	std::cout << bluey << std::endl;
	//	std::cout << orangex << std::endl;
	//	std::cout << orangey << std::endl;
	//}

if (Input::GetKey(Key::V)) {    //zoom out 
		ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).SetPosition(vec3(camx, camy+1, camz) );
	}

	
	m_activeScene->KeyboardHold();
}

void Game::KeyboardDown() {
	//keyboard button down
	m_activeScene->KeyboardDown();
}

void Game::KeyboardUp()
{
	//keyboard button up 
	m_activeScene->KeyboardUp();
	
	//Imgui
	if (Input::GetKeyUp(Key::P))
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}

	if (Input::GetKeyUp(Key::F1)){
		if (!UI::m_isInit)
		{
			UI::InitImGUI();
		}
		m_guiActive = !m_guiActive;
	}
}


void Game::MouseMotion(SDL_MouseMotionEvent evnt)
{
	//mouse moved
	m_activeScene->MouseMotion(evnt);

	if (m_guiActive)
	{
		ImGui::GetIO().MousePos = ImVec2(float(evnt.x), float(evnt.y));

		if (!ImGui::GetIO().WantCaptureMouse)
		{

		}
	}

	//Resets the enabled flag
	m_motion = false;
}

void Game::MouseClick(SDL_MouseButtonEvent evnt){
	//mouse click
	m_activeScene->MouseClick(evnt);

	//if it's on the level select menu, and they click the area indicating the waterfall level, load it
	if ((currentScene == 1) && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (float(evnt.x) >= 250.f && float(evnt.x) <= 790.f) && (float(evnt.y) >= 243.2f && float(evnt.y) <= 567.2f)) {
		//unload the level select scene
		m_activeScene->Unload();
		//set the scene to be the main game
		currentScene = 2;
		m_activeScene = m_scenes[currentScene];
		//set the level to be the waterfall level
		level = 1; 
		//initilize the game scene with the apporiate level
		m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()), level);
		//set the register to be the correct scene
		m_register = m_activeScene->GetScene();
	}
	//otherwise if it's on the level select menu, and they click the area indicating the house level, load it
	else if ((currentScene == 1) && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (float(evnt.x) >= 1130.f && float(evnt.x) <= 1670.f) && (float(evnt.y) >= 243.2f && float(evnt.y) <= 567.2f)) {
		//unload the level select scene
		m_activeScene->Unload();
		//set the scene to be the main game
		currentScene = 2;
		m_activeScene = m_scenes[currentScene];
		//set the level to be the house level
		level = 2;
		//initilize the game scene with the apporiate level
		m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()), level);
		//set the register to be the correct scene
		m_register = m_activeScene->GetScene();
	}
	//otherwise if it's on the level select menu, and they click the area indicating the city level, load it
	else if ((currentScene == 1) && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (float(evnt.x) >= 250.f && float(evnt.x) <= 790.f) && (float(evnt.y) >= 676.4f && float(evnt.y) <= 1000.4f)) {
		//unload the level select scene
		m_activeScene->Unload();
		//set the scene to be the main game
		currentScene = 2;
		m_activeScene = m_scenes[currentScene];
		//set the level to be the city
		level = 3;
		//initilize the game scene with the apporiate level
		m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()), level);
		//set the register to be the correct scene
		m_register = m_activeScene->GetScene();
	}
	//otherwise if it's on the level select menu, and they click the area indicating the pyramid level, load it
	else if ((currentScene == 1) && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (float(evnt.x) >= 1130.f && float(evnt.x) <= 1670.f) && (float(evnt.y) >= 676.4f && float(evnt.y) <= 1000.4f)) {
		//unload the level select scene
		m_activeScene->Unload();
		//set the scene to be the main game
		currentScene = 2;
		m_activeScene = m_scenes[currentScene];
		//set the level to be the pyramid level
		level = 4;
		//initilize the game scene with the apporiate level
		m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()), level);
		//set the register to be the correct scene
		m_register = m_activeScene->GetScene();
	}
	//otherwise if it's on the level select menu, and they click the area indicating the back button, return to the main menu
	else if ((currentScene == 1) && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (float(evnt.x) >= 30.24f && float(evnt.x) <= 159.84f) && (float(evnt.y) >= 10.8f && float(evnt.y) <= 75.6f)) {
		//unload the level select scene
		m_activeScene->Unload();
		//set the scene to be the main menu
		currentScene = 0;
		m_activeScene = m_scenes[currentScene];
		//initilize the main menu
		m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()));
		//set the register to be the correct scene
		m_register = m_activeScene->GetScene();
	}

	//if it's on the main menu, and they click the play button bring them to the level select menu
	 else if ((currentScene == 0) && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (float(evnt.x) >= 856.f && float(evnt.x) <= 1063.f) && (float(evnt.y) >= 492.f && float(evnt.y) <= 590.f)) {
		//unload the main menu scene
		m_activeScene->Unload();
		//set the scene to be the level select menu 
		currentScene = 1;
		m_activeScene = m_scenes[currentScene];
		//initiliaze the level select menu 
		m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()));
		//set the register to be the correct scene
		m_register = m_activeScene->GetScene();
	}


	if (m_guiActive)
	{
		ImGui::GetIO().MousePos = ImVec2(float(evnt.x), float(evnt.y));
		ImGui::GetIO().MouseDown[0] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT));
		ImGui::GetIO().MouseDown[1] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT));
		ImGui::GetIO().MouseDown[2] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE));
	}

	//Resets the enabled flag
	m_click = false;
}

void Game::MouseWheel(SDL_MouseWheelEvent evnt)
{
	//mouse wheel
	m_activeScene->MouseWheel(evnt);

	if (m_guiActive)
	{
		ImGui::GetIO().MouseWheel = float(evnt.y);
	}
	//Resets the enabled flag
	m_wheel = false;
}
