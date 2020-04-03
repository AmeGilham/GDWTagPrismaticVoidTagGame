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

	void ResetScene(); //restores the game to the state it was in at the beginning

	//return the gamestate variable 
	int GetGameState() override;

	//updates the scene every frame
	void Update() override;

	//main gameplay code for when nothing's paused, nobody's changed whose it recently, and everything is running normally 
	void UpdateMain();
	//code for when a match has ended, and one of the players bombs is going off
	void UpdateGameEnd();

	//create entities for the stage of the game after a player has lost 
	void CreateEndOfMatchEntities();

	void destroy();
	void createT(int ud);
	void destroyT();

	void level1(float windowWidth, float windowHeight);
	void level2(float windowWidth, float windowHeight);
	void level3(float windowWidth, float windowHeight);
	void level4(float windowWidth, float windowHeight);

	//Gamepad input, player variable tells it if it's orange or blue 
	void GamepadInput() override; 
	void GamepadStroke(XInputController* con, int player);
	void GamepadUp(XInputController* con, int player);
	void GamepadDown(XInputController* con, int player);
	void GamepadStick(XInputController* con, int player);
	void GamepadTrigger(XInputController* con, int player);

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

	//hud burning y position
	float hudBurningYPos(double ratio);
private:
	//variable to store which level the game is playing 
	int stage = 1;

	//variable to determine the current state of the game, 0 is game starting, 1 is game running, 2 is it change, 3 is paused, 4 is game ending, 5 is options after game end 
	int gameState = 1;

	//Box2D user data
	int blue = 0;
	int orange = 1;
	int platform = 2;
	int border = 3; 
	int btag = 4; //tag box for blue (2 different tag variables beacuse if they used the same, pre solve thinks the box is touching the other player
	int otag = 6; //tag box for orange
	int notItObjective = 5; 

	//stores wheter or not the players are facing right
	bool bright = true; //is blue facing right?
	bool oright = false; //is orange facing right?

	//TIMERS 
	float timeSinceGameStart = 0.f;
	float timeSinceGameEnd = 0.f;
	//time since jump timers
	float blueTimeSinceLastJump = 0.f;
	float orangeTimeSinceLastJump = 0.f;
	//time since each player hit the "Tag button" 
	float timeSinceTagTriggered = 0.f;
	//time since palyer last slid
	float timeSinceSlideB = 0.f;
	float timeSinceSlideO = 0.f;
	//time each player has left as the one "it"
	float maxTime = 90.f;
	float blueFuseTimeRemaining = maxTime;
	float orangeFuseTimeRemaining = maxTime;


	//store the index of the frame for the animation that's being swapped out with the tagging animation
	int tagFrame = -1; 

	//are the players current sliding 
	bool blueSlide = false; 
	bool orangeSlide = false; 

	//bool for if a tag entity exists
	bool tagExists = false; 
	//variable to store the entity number for the tag object 
	unsigned int tagEntity = 0;
	//variable to store the entity number of the "It!" Hud object
	unsigned int itIdentifyingHudEntity = 0;
	//variable to store the entity number of the not-it objective 
	unsigned int notitEntity = 0;

	float itTime = 5.f;
	float animTime = itTime;
	float timeLeft;

	float animTimeO = itTime;
	float timeLeftO;



	//Hud bomb entity numbers, 0 is blue, 1 is orange, 2 is the burning fuse sprite
	unsigned int bombs[3];
	//Entities numbers for all the entities important to after the match has ended
	unsigned int EndOfMatch[6];
	//Entity number for the animated part of the waterfall in the first stage 
	unsigned int falls;

	//last player to trigger a tag, 1 is blue, 2 is orange
	int  playerWhoTriggedTag = 0;


	//speed caps, too be adjusted when a player is "it"
	float blueSpeedCap = 40.f;
	float orangeSpeedCap = 40.f;

	//booleans to track if the movement input for each player has been registered this frame (used to balance input with both controller and keyboard) 
	bool blueMoved = false; 
	bool orangeMoved = false; 

	//vectors to copy the velocity of the players and not it for when the game is paused
	b2Vec2 blueVel;
	b2Vec2 orangeVel;
	b2Vec2 notItVel;
};