#include "Spawn.h"

void Spawn::SetNum(int a){
	pnum = a;
}

void Spawn::SetObj(bool a){
	itob = a;
}

bool Spawn::GetBlue()
{
	return blue;
}

bool Spawn::GetOrange()
{
	return oran;
}

void Spawn::SetBlue(bool a)
{
	blue = a;
}

void Spawn::SetOrange(bool a)
{
	oran = a;
}

int Spawn::GetNum(){
	return pnum;
}

bool Spawn::GetObj()
{
	return itob;
}


