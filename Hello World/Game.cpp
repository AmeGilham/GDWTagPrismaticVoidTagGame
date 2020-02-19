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
	vec3 force = vec3(8000.f, 0.f, 0.f);
	vec3 force2 = vec3(0.f, 3000.f, 0.f);

	//blue 
	auto& tempPhysBod = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer());
	b2Body* body = tempPhysBod.GetBody();

	b2Vec2 position = m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).GetPosition();

	if (Input::GetKey(Key::A) && body->GetLinearVelocity().x > float32(-80.f)) {
		m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(-force);}


	 else if (Input::GetKey(Key::D) && body->GetLinearVelocity().x < float32(80.f)) {
		m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(force);}

	 else {
		if (body->GetLinearVelocity().x > float32(0.f))
		{
			if (body->GetLinearVelocity().x > float32(20.f)) {

				body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x - 20, body->GetLinearVelocity().y));
			}
			else {
				body->SetLinearVelocity(b2Vec2(0, body->GetLinearVelocity().y));
			}
		}
		else if (body->GetLinearVelocity().x < float32(0.f))
		{
			if (body->GetLinearVelocity().x < float32(-20.f)) {

				body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x + 20, body->GetLinearVelocity().y));
			}
			else {
				body->SetLinearVelocity(b2Vec2(0, body->GetLinearVelocity().y));
			}
		}
	}

	if (Input::GetKey(Key::S)) m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(vec3(tempPhysBod.GetForce().x, -200000.8f, 0.f));
	else m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(vec3(tempPhysBod.GetForce().x, -3000.8f, 0.f));

	//orange
	auto& tempPhysBodO = ECS::GetComponent<PhysicsBody>(EntityIdentifier::SecondPlayer());
	b2Body* bodyO = tempPhysBodO.GetBody();

	b2Vec2 positionO = m_register->get<PhysicsBody>(EntityIdentifier::SecondPlayer()).GetPosition();

	if (Input::GetKey(Key::LeftArrow) && bodyO->GetLinearVelocity().x > float32(-80.f)) {
		m_register->get<PhysicsBody>(EntityIdentifier::SecondPlayer()).ApplyForce(-force);
	}


	else if (Input::GetKey(Key::RightArrow) && bodyO->GetLinearVelocity().x < float32(80.f)) {
		m_register->get<PhysicsBody>(EntityIdentifier::SecondPlayer()).ApplyForce(force);
	}

	else {
		if (bodyO->GetLinearVelocity().x > float32(0.f))
		{
			if (bodyO->GetLinearVelocity().x > float32(20.f)) {

				bodyO->SetLinearVelocity(b2Vec2(bodyO->GetLinearVelocity().x - 20, bodyO->GetLinearVelocity().y));
			}
			else {
				bodyO->SetLinearVelocity(b2Vec2(0, bodyO->GetLinearVelocity().y));
			}
		}
		else if (bodyO->GetLinearVelocity().x < float32(0.f))
		{
			if (bodyO->GetLinearVelocity().x < float32(-20.f)) {

				bodyO->SetLinearVelocity(b2Vec2(bodyO->GetLinearVelocity().x + 20, bodyO->GetLinearVelocity().y));
			}
			else {
				bodyO->SetLinearVelocity(b2Vec2(0, bodyO->GetLinearVelocity().y));
			}
		}
	}

	if (Input::GetKey(Key::DownArrow)) m_register->get<PhysicsBody>(EntityIdentifier::SecondPlayer()).ApplyForce(vec3(tempPhysBodO.GetForce().x, -200000.8f, 0.f));
	else m_register->get<PhysicsBody>(EntityIdentifier::SecondPlayer()).ApplyForce(vec3(tempPhysBodO.GetForce().x, -3000.8f, 0.f));


	if (Input::GetKey(Key::Z)) {    //zoom in
		m_register->get<Camera>(EntityIdentifier::MainCamera()).Zoom(1);
	}
	if (Input::GetKey(Key::X)) {    //zoom out
		m_register->get<Camera>(EntityIdentifier::MainCamera()).Zoom(-1);
	}

}

void Game::KeyboardDown() {
	vec3 force2 = vec3(0.f, 300000.f, 0.f);

	if (Input::GetKeyDown(Key::W)) {

		//Check if Blue can jump 
		if (m_activeScene->GetListener()->getJumpB()) {
			//if she can, set it so she can't 
			m_activeScene->GetListener()->setJumpB(false);
			//and apply the upward force of the jump
			m_register->get<PhysicsBody>(EntityIdentifier::MainPlayer()).ApplyForce(force2);
		}
	}

	//Check if Orange can jump
	if (Input::GetKeyDown(Key::UpArrow)) {
		if (m_activeScene->GetListener()->getJumpO()) {
			//if he can, set it so he can't 
			m_activeScene->GetListener()->setJumpO(false);
			//and apply the upward force of the jump
			m_register->get<PhysicsBody>(EntityIdentifier::SecondPlayer()).ApplyForce(force2);
		}
	}

	if (Input::GetKeyDown(Key::D))
	{
		auto &animController = ECS::GetComponent<AnimationController>(EntityIdentifier::MainPlayer());
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
