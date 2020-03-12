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

void Game::InitGame()
{
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
	m_activeScene = m_scenes[0];

	//Intializes the scene
	m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()));

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
		if (Input::m_windowFocus)
		{
			//Accept all input
			AcceptInput();
		}
	}
	//exit the program
	return true;
}

void Game::Update()
{
	//Update timer
	Timer::Update();
	//Update the backend
	BackEnd::Update(m_register);
	//Update Physics System
	PhysicsSystem::Update(m_register, m_activeScene->GetPhysicsWorld());
	//Update the current scene 
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

void Game::changescene(){
	m_activeScene = m_scenes[1];
	m_register = m_activeScene->GetScene();

}

void Game::CheckEvents()
{
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
	//keyboard button held 
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

void Game::MouseMotion(SDL_MouseMotionEvent evnt){
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

	if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && (float(evnt.x) >= 119.f && float(evnt.x) <= 799.f) && (float(evnt.y) >= 137.f && float(evnt.y) <= 541.f) && (m_activeScene == m_scenes[1])) {
		printf("\n LEVEL SELECT \n");
		m_activeScene->Unload();
		m_activeScene = m_scenes[2];
		m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()));
		m_register = m_activeScene->GetScene();
	}

	//menu switching code
	else if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) &&  (float(evnt.x) >= 393.f && float(evnt.x) <= 527.f) && (float(evnt.y) >= 307.f && float(evnt.y) <= 373.f) && ( m_activeScene == m_scenes[0]) ) {
		printf("\n CLICKED");
		m_activeScene->Unload();
		m_activeScene = m_scenes[1];
		m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()));
		m_register = m_activeScene->GetScene();
		}

	

	if (m_guiActive){
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
