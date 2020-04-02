#ifndef __GAME_H__
#define __GAME_H__

#include "BackEnd.h"
#include "MainGame.h"
#include "MainMenu.h"
#include "LevelSelect.h"

//Our main class for running our game
class Game
{
public:
	//Empty constructor
	Game() { };
	//Deconstructor for game
	//*Unloads window
	~Game();

	//Initiaiizes game
	//*Seeds random
	//*Initializes SDL
	//*Creates Window
	//*Initializes GLEW
	//*Create Main Camera Entity
	//*Creates all other entities and adds them to register
	void InitGame();

	//Runs the game
	//*While window is open
	//*Clear window
	//*Update 
	//*Draw
	//*Poll events
	//*Flip window
	//*Accept input
	bool Run();
	
	//Updates the game
	//*Update timer
	//*Update the rendering system
	//*Update the animation system
	void Update();

	//Runs the GUI
	//*Uses ImGUI for this
	void GUI();

	//Check events
	//*Checks the results of the events that have been polled
	void CheckEvents();

	/*Input Functions*/
	void AcceptInput();
	void KeyboardHold();
	void KeyboardDown();
	void KeyboardUp();
	//Mouse input
	void MouseMotion(SDL_MouseMotionEvent evnt);
	void MouseClick(SDL_MouseButtonEvent evnt);
	void MouseWheel(SDL_MouseWheelEvent evnt);
private:
	//The window
	Window *m_window = nullptr;

	//Scene name
	std::string m_name;
	//Clear color for when we clear the window
	vec4 m_clearColor;
	
	//The main register for our ECS
	entt::registry* m_register;

	//Scenes
	Scene* m_activeScene;
	std::vector<Scene*> m_scenes;
	
	//Imgui stuff
	bool m_guiActive = false;

	
	//Hooks for events
	bool m_close = false;
	bool m_motion = false;
	bool m_click = false;
	bool m_wheel = false;

	float camTime = 0.f;
	//scene and level control variables 
	int level=1;//1 is waterfall level, 2 is house level, 3 is city level, 4 is pyramid level 
	int currentScene = 0;//0 is main menu, 1 is level select menu, 2 is main game (levels controller by the above variable) 
};



#endif // !__GAME_H__

