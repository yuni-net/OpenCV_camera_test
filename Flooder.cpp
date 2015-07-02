#include "Flooder.h"

void Flooder::init(cv::Mat & cap_image, const my::Rectangle & search_area, const int threshold)
{
	this->cap_image = &cap_image;
	this->search_area = search_area;
	this->threshold = threshold;

	cap_image.copyTo(base_image);

	const int width = search_area.end.x - search_area.beg.x;
	const int height = search_area.end.y - search_area.beg.y;

	did_judge_table.create(height, width, false);
}

void Flooder::flood(const IntCoor2 & coor)
{
	if (coor.x < search_area.beg.x ||
		coor.x >= search_area.end.x ||
		coor.y < search_area.beg.y ||
		coor.y >= search_area.end.y)
	{
		return;
	}

	const IntCoor2 local_coor = compute_local_coor(coor);

	if (did_judge_table.at(local_coor) == true)
	{
		return;
	}

	bool is_different = judge(coor);
	did_judge_table.at(local_coor) = true;

#if 1
		// for debug
		cv::rectangle(
			*cap_image,
			cv::Point2d(coor.x, coor.y),
			cv::Point2d(coor.x, coor.y),
			cv::Scalar(255, 100, 100));
#endif

	if (is_different)
	{
		return;
	}

	for (int y = 1; y >= -1; --y)
	{
		for (int x = 1; x >= -1; --x)
		{
			IntCoor2 next_coor;
			next_coor.x = coor.x + x;
			next_coor.y = coor.y + y;
			flood(next_coor);
		}
	}
}

bool Flooder::judge(const IntCoor2 & coor)
{
	int def = 0;

	for (int y = 1; y >= -1; --y)
	{
		for (int x = 1; x >= -1; --x)
		{
			IntCoor2 target_coor;
			target_coor.x = coor.x + x;
			target_coor.y = coor.y + y;
			def += compute_def(coor, target_coor);
		}
	}

	if (def > threshold)
	{
		return true;
	}

	return false;
}

IntCoor2 Flooder::compute_local_coor(const IntCoor2 & coor)
{
	IntCoor2 result;
	result.x = coor.x - search_area.beg.x;
	result.y = coor.y - search_area.beg.y;
	return result;
}

int Flooder::compute_def(const IntCoor2 & base_coor, const IntCoor2 & target_coor)
{
	int total_def = 0;

	const int base_index = index1D(base_coor);
	const int target_index = index1D(target_coor);

	for (int color = 0; color < 3; ++color)
	{
		const int base_color = static_cast<int>(base_image.data[base_index * 3 + color]);
		const int target_color = static_cast<int>(base_image.data[target_index * 3 + color]);
		total_def += abs(base_color - target_color);
	}

	return total_def;
}

int Flooder::index1D(const IntCoor2 & coor) const
{
	return base_image.size().width*coor.y + coor.x;
}
