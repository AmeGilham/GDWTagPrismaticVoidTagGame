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
	m_name = "Game";
	m_clearColor = vec4(0.15f, 0.33f, 0.58f, 1.f);

	//Initializes the backend
	BackEnd::InitBackEnd(m_name);

	//Grabs the initialized window
	m_window = BackEnd::GetWindow();

	//Creates a new HelloWorld scene
	m_scenes.push_back(new HelloWorld("Hello World"));
	m_scenes.push_back(new Scene("First Loaded Scene"));
	m_scenes.push_back(new HelloWorld("Second Loaded Scene"));
	//Sets active scene reference to our HelloWorld scene
	m_activeScene = m_scenes[0];

	//*m_activeScene = File::LoadJSON("Hello World.json");
	//Intializes the scene
	m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()));

	//Sets m_register to point to the register in the active scene
	m_register = m_activeScene->GetScene();

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

	return true;
}

void Game::Update()
{
	//Update timer
	Timer::Update();
	//Update the backend
	BackEnd::Update(m_register);
	m_activeScene->Update();

	//Update Physics System
	PhysicsSystem::Update(m_register, m_activeScene->GetPhysicsWorld());
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
	vec3 force = vec3(1200.f, 0.f, 0.f);
	vec3 force2 = vec3(0.f, 3000.f, 0.f);

	b2Vec2 position = m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetPosition();

	if (Input::GetKey(Key::A)) {
		m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(-force);}

	 if (Input::GetKey(Key::D)) {
		m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(force);}

	 //if (Input::GetKey(Key::W)) {
		//m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(force2);}

	 if (Input::GetKey(Key::S)) {
		m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(-force2);}

	else {
		force= vec3(0.f, 0.f, 0.f);
		m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(vec3(0.f, -909.8f, 0.f));}

}

void Game::KeyboardDown() {
	vec3 force2 = vec3(0.f, 150000.f, 0.f);

	if (Input::GetKeyDown(Key::W)) {
		numPress++;
		std::cout << m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetForce().y<<"What???????????????\n";


		if (numPress <= 2 && m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetVelocity().y == 0 ) {
			m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(force2);
			//std::cout << numPress;
		}
		else if (m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetVelocity().y == 0){
			numPress = 0;
			std::cout << numPress;
			std::cout << m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetVelocity().y;}
		else {
			std::cout << "Help"<<numPress;}
	}

	if (Input::GetKeyDown(Key::D))
	{
		HelloWorld* scene = (HelloWorld*)m_activeScene;
		auto entity = scene->GetPlayer();
		auto &animController = ECS::GetComponent<AnimationController>(entity);
		//Sets active animation
		animController.SetActiveAnim(2);
	}
	if (Input::GetKeyDown(Key::A))
	{
		HelloWorld* scene = (HelloWorld*)m_activeScene;
		auto entity = scene->GetPlayer();
		auto &animController = ECS::GetComponent<AnimationController>(entity);
		//Sets active animation
		animController.SetActiveAnim(1);
	}
}

void Game::KeyboardUp()
{
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
	//printf("Mouse Moved (%f, %f)\n", float(evnt.x), float(evnt.y));

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

void Game::MouseClick(SDL_MouseButtonEvent evnt)
{
	/*if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		printf("Left Mouse Button Clicked at (%f, %f)\n", float(evnt.x), float(evnt.y));
	}
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		printf("Right Mouse Button Clicked at (%f, %f)\n", float(evnt.x), float(evnt.y));
	}
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
	{
		printf("Middle Mouse Button Clicked at (%f, %f)\n", float(evnt.x), float(evnt.y));
	}*/

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
	printf("Mouse Scroll %f\n", float(evnt.y));

	if (m_guiActive)
	{
		ImGui::GetIO().MouseWheel = float(evnt.y);
	}
	//Resets the enabled flag
	m_wheel = false;
}
