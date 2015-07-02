#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>

#if 1

class Accesser2D
{
public:
	Accesser2D(const int width, const int height)
	{
		this->width = width;
	}

	int at(const int x, const int y)
	{
		return width*y + x;
	}

private:
	int width;
};

void filtering_pixel(const cv::Mat & base, int filter[3][3], cv::Mat & converted, const int x, const int y)
{
	Accesser2D ser(base.size().width, base.size().height);
	int value = 0;

	for (int filter_y = 0; filter_y < 3; ++filter_y)
	{
		for (int filter_x = 0; filter_x < 3; ++filter_x)
		{
			const int index = ser.at(x + filter_x -1, y + filter_y -1);
			const unsigned char base_pix = base.data[index];
			const int the_filter = filter[filter_y][filter_x];
			value += base_pix * the_filter;
		}
	}

	value = abs(value);

	if (value > 255)
	{
		value = 255;
	}

	converted.data[ser.at(x, y)] = static_cast<unsigned char>(value);
}

void filtering(const cv::Mat & base, int filter[3][3], cv::Mat & converted)
{
	converted.create(base.size(), CV_8U);

	for (int y = 1; y < base.size().height-1; ++y)
	{
		for (int x = 1; x < base.size().width - 1; ++x)
		{
			filtering_pixel(base, filter, converted, x, y);
		}
	}
}

void dif_filtering_horizon(const cv::Mat & base, cv::Mat & converted)
{
	int filter[3][3] = {
		{ 0, 0, 0 },
		{ 0, -1, 1 },
		{ 0, 0, 0 } };

	filtering(base, filter, converted);
}

void dif_filtering_virtical(const cv::Mat & base, cv::Mat & converted)
{
	int filter[3][3] = {
		{ 0, 1, 0 },
		{ 0, -1, 0 },
		{ 0, 0, 0 } };

	filtering(base, filter, converted);
}
#endif
