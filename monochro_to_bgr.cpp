#include <cmath>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>

void monochrome_to_bgr(const cv::Mat & monochro, cv::Mat & bgr)
{
	const int pix_width = monochro.size().width;
	bgr.create(cv::Size(monochro.size().width, monochro.size().height), CV_8UC3);

	for (int pix_No = 0; pix_No < pix_width*monochro.size().height; ++pix_No)
	{
		bgr.data[pix_No * 3 + 0] = monochro.data[pix_No];			
		bgr.data[pix_No * 3 + 1] = monochro.data[pix_No];
		bgr.data[pix_No * 3 + 2] = monochro.data[pix_No];
	}
}