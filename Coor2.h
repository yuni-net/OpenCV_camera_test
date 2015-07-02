#pragma once

struct Coor2
{
	float x;
	float y;
};

struct IntCoor2
{
	int x;
	int y;

	IntCoor2(){}
	IntCoor2(const int x, const int y)
	{
		this->x = x;
		this->y = y;
	}
};
