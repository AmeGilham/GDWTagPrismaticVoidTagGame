#pragma once
//class for power up identification
// all NON power ups must have a  value of 0
class PowerNum
{
public:

	PowerNum() {}; //empty constructor

	void SetNum(int);
	int GetNum();
	   
protected:

	int pnum=0;

};

