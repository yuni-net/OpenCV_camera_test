#include <cmath>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>

void bgr_to_monochrome(const cv::Mat & bgr, cv::Mat & monochro)
{
	monochro.create(cv::Size(bgr.size().width, bgr.size().height), CV_8U);

	for (int pix_No = 0; pix_No < bgr.size().width * bgr.size().height; ++pix_No)
	{
		int total_bright = bgr.data[pix_No * 3 + 0] + bgr.data[pix_No * 3 + 1] + bgr.data[pix_No * 3 + 2];
		monochro.data[pix_No] = static_cast<unsigned char>(round(total_bright / 3.0f));
	}
}