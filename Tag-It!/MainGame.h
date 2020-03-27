#pragma once

#include "Game.h"
#include "Scene.h"
#include "Input.h"
#include <math.h>

//Scene child class for the main match / core of the gameplay
class MainGame : public Scene
{
public:
	//constructor
	MainGame(std::string name);

	//initilizes the scene
	void InitScene(float windowWidth, float windowHeight, int level) override;

	//updates the scene every frame
	void Update() override;

	void destroy();
	void createT(int ud);
	void destroyT();

	void level1(float windowWidth, float windowHeight);
	void level2(float windowWidth, float windowHeight);

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

	//create animations 
	void createAnimation(Animation* anim, int x, int y, int width, int height, int frames, bool flipped, float lenghtOfFrame, bool repeating);
	void itAnimB();
	void itAnimO();
	float round(float var);
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

	//time since each player hit the "Tag button" 
	float timeSinceTagTriggered = 0.f;

	//time since palyer last slid
	float timeSinceSlideB = 0.f;
	float timeSinceSlideO = 0.f;

	//bool for if a tag entity exists
	bool tagExists = false; 
	//variable to store the entity number for the tag object 
	unsigned int tagEntity = 0;
	//variable to store the entity number of the "It!" Hud object
	unsigned int itIdentifyingHudEntity = 0;
	float itTime = 5.f;
	float animTime = itTime;
	float timeLeft;

	float animTimeO = itTime;
	float timeLeftO;


	//Hud bomb entity numbers, 0 is blue, 1 is orange, 2 is the burning fuse sprite
	unsigned int bombs[3];

	//time each player has left as the one "it"
	float maxTime = 90.f; 
	float blueFuseTimeRemaining = maxTime;
	float orangeFuseTimeRemaining = maxTime;
};