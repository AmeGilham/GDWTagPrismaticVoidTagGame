#pragma once
#include "Scene.h"
#include "Game.h"

class MainMenu : public Scene {
public:
	//constructor
	MainMenu(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void MouseClick(SDL_MouseButtonEvent evnt);
	void MouseMotion(SDL_MouseMotionEvent evnt);

private:

};

