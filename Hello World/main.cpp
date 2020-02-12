#include "Game.h"

#undef main
int main()
{
	//Create the Game
	Game game;

	//Initialize the game
	game.InitGame();

	//Runs the game until it returns :)
	return game.Run();
}