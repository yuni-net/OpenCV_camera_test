#pragma once

#include "MouseData.h"
#include "EyeRecognizer.h"

struct MouseWrapper
{
	MouseData * mouse_data;
	EyeRecognizer * eyezer;
	cv::Mat * cap_image;
};