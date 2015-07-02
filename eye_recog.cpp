#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>
#include "Coor2.h"
#include "MouseData.h"

Coor2 eye_recog(const cv::Mat & target, const MouseData & mouse_data)
{
	Coor2 coor;
	coor.x = 0;
	coor.y = 0;
	return coor;
}