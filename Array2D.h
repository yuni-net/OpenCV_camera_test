#pragma once

#include "Coor2.h"

template<typename X>
class Array2D
{
public:
	void create(const int height, const int width)
	{
		delete[] data;
		data = new X[width*height];
		width_ = width;
	}

	void create(const int height, const int width, const X & initial_value)
	{
		delete[] data;
		data = new X[width*height];
		width_ = width;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				at(y, x) = initial_value;
			}
		}
	}

	X & at(const int y, const int x)
	{
		const int index = width_*y + x;
		return data[index];
	}
	const X & at(const int y, const int x) const
	{
		const int index = width_*y + x;
		return data[index];
	}

	X & at(const IntCoor2 & coor)
	{
		const int index = width_*coor.y + coor.x;
		return data[index];
	}
	const X & at(const IntCoor2 & coor) const
	{
		const int index = width_*coor.y + coor.x;
		return data[index];
	}


	Array2D()
	{
		data = nullptr;
	}
	~Array2D()
	{
		delete[] data;
	}
private:
	X * data;
	int width_;
};