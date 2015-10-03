#include <Windows.h>

#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "EyeRecognizer.h"
#include "Array2D.h"
#include "Flooder.h"

const float win_width = 1280;
const float win_height = 800;

bool create_bmp(int width, int height, const char * file_path, const unsigned char * data_bgr);


bool EyeRecognizer::was_set_eye_area() const
{
	return was_set_eye_area_;
}
void EyeRecognizer::set_eye_area(const MouseData & mouse_data)
{
	eye_area.beg = mouse_data.beg;
	eye_area.end = mouse_data.end;
	was_set_eye_area_ = true;

	POINT point;
	GetCursorPos(&point);
	win_coor_.x = static_cast<float>(point.x);
	win_coor_.y = static_cast<float>(point.y);

	const int area_width = eye_area.end.x - eye_area.beg.x;
	cv::Mat temp(area_width, area_width, close_image.type());
	try{
		cv::resize(close_image, temp, temp.size(), cv::INTER_LINEAR);
	}
	catch (...){
		// Nothing OpenCVがバグってるから例外を握りつぶす
	}
	temp.copyTo(close_image);

	pupil_radius = area_width*0.5f;
	total_radius = pupil_radius;
	sampling_num = 1;
}

void EyeRecognizer::show_pupil(cv::Mat & cap_image)
{
	const int area_width = eye_area.end.x - eye_area.beg.x;
	const int area_height = eye_area.end.y - eye_area.beg.y;

	IntCoor2 center;
	const float cx = eye_area.beg.x + area_width*0.5f;
	center.x = static_cast<int>(round(cx));
	const float cy = eye_area.beg.y + area_height*0.5f;
	center.y = static_cast<int>(round(cy));

	if (is_not_black(cap_image, center))
	{
		center = find_black(cap_image, center);
	}

	Flooder flooder;
	flooder.init(cap_image, eye_area, threshold);
	pupil_area = flooder.flood(center, pupil_top4);

	// debug
	const int index = cap_image.size().width*center.y + center.x;
	cap_image.data[index * 3 + 0] = 0;
	cap_image.data[index * 3 + 1] = 255;
	cap_image.data[index * 3 + 2] = 0;

	if (was_set_threshold() == false)
	{
		const int width = pupil_area.end.x - pupil_area.beg.x;
		const int height = pupil_area.end.y - pupil_area.beg.y;
		float this_radius;
		if (width >= height)
		{
			this_radius = width*0.5f;
		}
		else
		{
			this_radius = height*0.5f;
		}
		total_radius += this_radius;
		sampling_num += 1;
		pupil_radius = total_radius / sampling_num;	// 平均をとる
		if (sampling_num >= 10000)
		{
			total_radius *= 0.5f;
			sampling_num = 5000;
		}
	}

}

bool EyeRecognizer::was_set_threshold() const
{
	return was_set_threshold_;
}
void EyeRecognizer::renew_threshold()
{
	POINT point;
	GetCursorPos(&point);
	//	ScreenToClient(cv::GetWindowHandle(""), &point);
	ScreenToClient(NULL, &point);

	threshold = static_cast<int>((point.x-640)*0.6f);
}
void EyeRecognizer::set_threshold()
{
	was_set_threshold_ = true;
}



bool EyeRecognizer::was_lefttop_set() const
{
	return was_lefttop_set_;
}
void EyeRecognizer::set_lefttop(cv::Mat & cap_image)
{
	const bool result = get_pupil_coor(cap_image, pupil_leftest, pupil_topest);
	if (result==false)
	{
		// during the blink
		return;
	}
	was_lefttop_set_ = true;
}

bool EyeRecognizer::was_rightbot_set() const
{
	return was_rightbot_set_;
}
void EyeRecognizer::set_rightbot(cv::Mat & cap_image)
{
	// 必ず左上の後に右下を設定することが前提

	float pupil_rightest;
	float pupil_botest;
	const bool result = get_pupil_coor(cap_image, pupil_rightest, pupil_botest);

	if (result == false)
	{
		// during the blink
		return;
	}

	was_rightbot_set_ = true;

	pupil_movable_width = pupil_rightest - pupil_leftest;
	pupil_movable_height = pupil_botest - pupil_topest;
}


void EyeRecognizer::recognize(cv::Mat & target)
{
	float pupil_x;
	float pupil_y;
	const bool result = get_pupil_coor(target, pupil_x, pupil_y);
	if (result == false)
	{
		// during the blink
		return;
	}

	// debug
	const int index = static_cast<int>(target.size().width*pupil_y + pupil_x);
	target.data[index * 3 + 0] = 0;
	target.data[index * 3 + 1] = 0;
	target.data[index * 3 + 2] = 255;

	float pupil_x_rel = pupil_x - pupil_leftest;
	float pupil_y_rel = pupil_y - pupil_topest;

	win_coor_.x = win_width * pupil_x_rel / pupil_movable_width;
	win_coor_.y = win_height * pupil_y_rel / pupil_movable_height;
}

const Coor2 & EyeRecognizer::win_coor()
{
	return win_coor_;
}


EyeRecognizer::EyeRecognizer()
{
	was_set_eye_area_ = false;
	was_set_threshold_ = false;
	was_lefttop_set_ = false;
	was_rightbot_set_ = false;
	threshold = 74;

	close_image = cv::imread("close.bmp");
}

bool EyeRecognizer::get_pupil_coor(cv::Mat & cap_image, float & x, float & y)
{
	show_pupil(cap_image);
	return compute_pupil_coor(pupil_area, pupil_top4, x, y);
}

int EyeRecognizer::compute_pupil_coor(const cv::Mat & cap_image, float & x, float & y)
{
	IntCoor2 beg = eye_area.beg;
	IntCoor2 end = eye_area.end;

	if (was_rightbot_set())
	{
		beg.x = static_cast<int>(pupil_leftest);
		end.x = beg.x + static_cast<int>(pupil_movable_width);
		beg.y = static_cast<int>(pupil_topest);
		end.y = beg.y + static_cast<int>(pupil_movable_height);
	}

	x = static_cast<float>(beg.x);
	y = static_cast<float>(beg.y);
	int smallest_def = compute_def_pupil(cap_image, beg.x, beg.y);

	for (int cap_y = beg.y; cap_y <= end.y; ++cap_y)
	{
		for (int cap_x = beg.x; cap_x <= end.x; ++cap_x)
		{
			int def = compute_def_pupil(cap_image, cap_x, cap_y);
			if (def < smallest_def)
			{
				smallest_def = def;
				x = static_cast<float>(cap_x);
				y = static_cast<float>(cap_y);
			}
		}
	}
	
	return smallest_def;
}

int EyeRecognizer::compute_def_close(const cv::Mat & cap_image)
{
	int smallest = compute_def_close(cap_image, -50);
	for (int plus_y = -50+1; plus_y <= 50; ++plus_y)
	{
		int def = compute_def_close(cap_image, plus_y);
		if (def < smallest)
		{
			smallest = def;
		}
	}

	return smallest;
}

int EyeRecognizer::compute_def_close(const cv::Mat & cap_image, const int plus_y)
{
	int def = 0;
	for (int y = 0; y < close_image.size().height; ++y)
	{
		for (int x = 0; x < close_image.size().width; ++x)
		{
			const int close_pix_index = close_image.size().width*y + x;

			const int cap_x = eye_area.beg.x + x;
			const int cap_y = eye_area.beg.y + y + plus_y;
			const int cap_pix_index = cap_image.size().width*cap_y + cap_x;

			int gap;

			gap = cap_image.data[cap_pix_index * 3 + 0] - close_image.data[close_pix_index * 3 + 0];
			def += abs(gap);

			gap = cap_image.data[cap_pix_index * 3 + 1] - close_image.data[close_pix_index * 3 + 1];
			def += abs(gap);

			gap = cap_image.data[cap_pix_index * 3 + 2] - close_image.data[close_pix_index * 3 + 2];
			def += abs(gap);
		}
	}

	return def;
}

/*
 * x,yは仮の瞳の中心座標。これとの差を求めて返す
 */
int EyeRecognizer::compute_def_pupil(const cv::Mat & cap_image, const int x, const int y)
{
	int def = 0;

	for (int cap_y = eye_area.beg.y; cap_y <= eye_area.end.y; ++cap_y)
	{
		for (int cap_x = eye_area.beg.x; cap_x <= eye_area.end.x; ++cap_x)
		{
			const float distance2 = compute_length2(IntCoor2(x, y), IntCoor2(cap_x, cap_y));
			const float white_radius = pupil_radius*2.0f;
			const float white_radius2 = white_radius*white_radius;
			if (distance2 > white_radius2)
			{
				continue;
			}

			const int pix_index = cap_image.size().width*cap_y + cap_x;
			const float pupil_radius2 = pupil_radius*pupil_radius;

			if (distance2 <= pupil_radius2)
			{
				def += cap_image.data[pix_index * 3 + 0];
				def += cap_image.data[pix_index * 3 + 1];
				def += cap_image.data[pix_index * 3 + 2];
				continue;
			}

			def += 255 - cap_image.data[pix_index * 3 + 0];
			def += 255 - cap_image.data[pix_index * 3 + 1];
			def += 255 - cap_image.data[pix_index * 3 + 2];
		}
	}

	return def;
}

float EyeRecognizer::compute_length2(const IntCoor2 & one, const IntCoor2 & another)
{
	IntCoor2 gap;
	gap.x = one.x - another.x;
	gap.y = one.y - another.y;
	return static_cast<float>(gap.x*gap.x + gap.y*gap.y);
}


bool EyeRecognizer::is_not_black(const cv::Mat & cap_image, const IntCoor2 & coor) const
{
	const int high_bright = 50;
	const int high_contrast = 6;
	const int index = cap_image.size().width*coor.y + coor.x;

	const int b = static_cast<int>(cap_image.data[index * 3 + 0]);
	const int g = static_cast<int>(cap_image.data[index * 3 + 1]);
	const int r = static_cast<int>(cap_image.data[index * 3 + 2]);

	if (b > high_bright ||
		g > high_bright ||
		r > high_bright)
	{
		return true;
	}

	if (abs(b - g) > high_contrast ||
		abs(g - r) > high_contrast ||
		abs(r - b) > high_contrast)
	{
		return true;
	}

	return false;
}

IntCoor2 EyeRecognizer::find_black(const cv::Mat & cap_image, const IntCoor2 & coor) const
{
	const int area_width = eye_area.end.x - eye_area.beg.x;
	IntCoor2 searched_coor;

	for (int outline_size = 3; outline_size <= area_width; outline_size+=2)
	{
		const bool is_black = search_black_outline(cap_image, coor, outline_size, searched_coor);
		if (is_black)
		{
			return searched_coor;
		}
	}

	return coor;
}

bool EyeRecognizer::search_black_outline(const cv::Mat & cap_image, const IntCoor2 & coor, const int outline_size, IntCoor2 & searched_coor) const
{
	const int half_size = outline_size / 2;

	const int beg_x = coor.x - half_size;
	const int end_x = coor.x + half_size;

	const int beg_y = coor.y - half_size;
	const int end_y = coor.y + half_size;

	for (int y = beg_y; y <= end_y; ++y)
	{
		if (y == beg_y || y == end_y)
		{
			for (int x = beg_x; x <= end_x; ++x)
			{
				searched_coor.x = x;
				searched_coor.y = y;
				if (is_not_black(cap_image, searched_coor) == false)
				{
					return true;
				}
			}
		}
		else
		{
			searched_coor.x = beg_x;
			searched_coor.y = y;
			if (is_not_black(cap_image, searched_coor) == false)
			{
				return true;
			}

			searched_coor.x = end_x;
			searched_coor.y = y;
			if (is_not_black(cap_image, searched_coor) == false)
			{
				return true;
			}
		}
	}

	return false;
}

/***
@return true...ウィンクしていない　false...ウィンクしている
*/
bool EyeRecognizer::compute_pupil_coor(const my::Rectangle & pupil_area, const Top4 & pupil_top4, float & x, float & y)
{
	const int pupil_width = pupil_area.end.x - pupil_area.beg.x;
	const int pupil_height = pupil_area.end.y - pupil_area.beg.y;
	float visible_radius;

	if (pupil_width >= pupil_height)
	{
		visible_radius = pupil_width*0.5f;
	}
	else
	{
		visible_radius = pupil_height*0.5f;
	}

	if (pupil_radius - visible_radius < 2.0f)
	{
		x = pupil_area.beg.x + pupil_width*0.5f;
		y = pupil_area.beg.y + pupil_height*0.5f;
		return true;
	}

	const int eye_width = eye_area.end.x - eye_area.beg.x;
	const int center_x = eye_area.beg.x + static_cast<int>(round(eye_width*0.5f));

	if (center_x < pupil_area.beg.x)
	{
		x = pupil_area.beg.x + pupil_radius;
		y = static_cast<float>(pupil_top4.leftest_y);
		return true;
	}

	if (pupil_area.end.x < center_x)
	{
		x = pupil_area.end.x - pupil_radius;
		y = static_cast<float>(pupil_top4.rightest_y);
		return true;
	}

	x = static_cast<float>(pupil_area.beg.x + pupil_width);
	y = static_cast<float>(pupil_top4.rightest_y);
	return true;
}