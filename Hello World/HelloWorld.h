#pragma once
#include "Scene.h"

class HelloWorld : public Scene
{
public:
	HelloWorld(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update();

	int GetPlayer();

private:
	int m_player;
	int count = 1;
	int powc = 12;

	//Box2D user data
	int blue = 0;
	int orange = 1;
	int platform = 2;
	int border = 3;
	int power = 4;

};
