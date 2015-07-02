#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>

void binarization(const cv::Mat & base, cv::Mat & converted)
{
	converted.create(base.size(), CV_8U);

	for (int pix_No = 0; pix_No < base.size().width * base.size().height; ++pix_No)
	{
		if (base.data[pix_No] > 5)
		{
			converted.data[pix_No] = 255;
		}
		else
		{
			converted.data[pix_No] = 0;
		}
	}
}