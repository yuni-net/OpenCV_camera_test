#pragma once

#include "Coor2.h"

struct MouseData
{
	IntCoor2 beg;
	IntCoor2 end;
	bool began_drag;
	bool finished_drag;
};
