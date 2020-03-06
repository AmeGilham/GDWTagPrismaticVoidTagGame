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

	//updates the scene every frame
	void Update() override;

	void destroy();
	void destroyT();

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
	//Box2D user data
	int blue = 0;
	int orange = 1;
	int platform = 2;
	int border = 3; 
	int btag = 4; //tag box for blue (2 different tag variables beacuse if they used the same, pre solve thinks the box is touching the other player
	int otag = 6; //tag box for orange
	int notItObjective = 5; 

	bool objective = true;

	bool bright = true; //is blue facing right?
	bool oright = false; //is orange facing right?

	//time since jump timers
	float blueTimeSinceLastJump = 0.f;
	float orangeTimeSinceLastJump = 0.f;

	//time each player has left as the one "it"
	float blueFuseTimeRemaining = 150.f;
	float orangeFuseTimeRemaining = 150.f;

	//array of ints with important entities, index 0 is not-it, index 1 is the sprite behind it 
	int imporantEntities[6];
	int notitdirection = 1;
};
