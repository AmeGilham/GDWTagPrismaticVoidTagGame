#pragma once
#include "Scene.h"
#include "Game.h"
class LevelSelect : public Scene {
public:
    //constructor
    LevelSelect(std::string name);

    void InitScene(float windowWidth, float windowHeight) override;

    void MouseClick(SDL_MouseButtonEvent evnt);
    void MouseMotion(SDL_MouseMotionEvent evnt);

	void Update() override; 

private:
	unsigned int levels[4] = { 0,0,0,0 };
};
