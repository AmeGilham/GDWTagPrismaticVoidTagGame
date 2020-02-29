#pragma once

#include "Scene.h"
#include "Input.h"

//Scene child class for the main match / core of the gameplay
class MainGame : public Scene
{
public:
	//constructor
	MainGame(std::string name);

	//initilizes the scene
	void InitScene(float windowWidth, float windowHeight) override;

	//returns the player (not used)
	int GetPlayer();

	//updates the scene every frame
	void Update() override;

	//Gamepad input
	void GamepadStroke(XInputController* con) override;
	void GamepadStick(XInputController* con) override;
	void GamepadTrigger(XInputController* con) override;

	//Keyboard Input
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

	//Mouse input
	void MouseMotion(SDL_MouseMotionEvent evnt) override;
	void MouseClick(SDL_MouseButtonEvent evnt) override;
	void MouseWheel(SDL_MouseWheelEvent evnt) override;

private:
	//player entity number (not used)
	int m_player;

	//Box2D user data
	int blue = 0;
	int orange = 1;
	int platform = 2;
	int border = 3;
	int tag = 4;

	//time since jump timers
	float blueTimeSinceLastJump = 0.0f;
	float orangeTimeSinceLastJump = 0.f;
};
