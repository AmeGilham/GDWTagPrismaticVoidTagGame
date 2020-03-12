#pragma once
class Spawn
{
public:

	Spawn() {}; //empty constructor 

	void SetNum(int);
	void SetObj(bool);

	bool GetBlue();
	bool GetOrange();

	void SetBlue(bool);
	void SetOrange(bool);

	int GetNum();
	bool GetObj();

protected:

	bool itob = false; //is objective
	bool blue = false;
	bool oran = false;

	int pnum = 0;
	int menu = 2;

	int tagboxblue = 8; //num for player tagging hitbox
	int tagboxorange = 9; //num for player tagging hitbox

};