#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>

#include "MouseData.h"
#include "EyeRecognizer.h"
#include "MouseWrapper.h"

void my_mouse_callback(int event, int x, int y, int flags, void * param)
{
	MouseWrapper & wrapper = *reinterpret_cast<MouseWrapper *>(param);
	MouseData & mouse_data = *(wrapper.mouse_data);
	EyeRecognizer & eyezer = *(wrapper.eyezer);
	cv::Mat & cap_image = *(wrapper.cap_image);

	switch (event)
	{
	case cv::EVENT_MOUSEMOVE:
		if (mouse_data.began_drag == true &&
			mouse_data.finished_drag == false)
		{
			mouse_data.end.x = x;
			mouse_data.end.y = y;
		}
		break;

	case cv::EVENT_LBUTTONDOWN:
		if (mouse_data.finished_drag == false)
		{
			mouse_data.beg.x = x;
			mouse_data.beg.y = y;
			mouse_data.end = mouse_data.beg;
			mouse_data.began_drag = true;
		}
		else
		{
			if (eyezer.was_set_threshold() == false)
			{
				eyezer.set_threshold();
				break;
			}
			if (eyezer.was_lefttop_set() == false)
			{
				eyezer.set_lefttop(cap_image);
				break;
			}
			if (eyezer.was_rightbot_set() == false)
			{
				eyezer.set_rightbot(cap_image);
				break;
			}
		}
		break;

	case cv::EVENT_LBUTTONUP:
		if (mouse_data.finished_drag == false)
		{
			mouse_data.end.x = x;
			mouse_data.end.y = y;
			mouse_data.finished_drag = true;
			eyezer.set_eye_area(mouse_data);
		}
		break;
	}
}