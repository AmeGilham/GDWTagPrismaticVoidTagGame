#pragma once
#include "Scene.h"

class HelloWorld : public Scene
{
public:
	HelloWorld(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	int GetPlayer();

private:
	int m_player;

	//Box2D user data
	int blue = 0;
	int orange = 1;
	int platform = 2;
	int border = 3;
};
