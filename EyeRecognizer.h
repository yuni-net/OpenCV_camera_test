#pragma once

#include "Coor2.h"
#include "MouseData.h"

class EyeRecognizer
{
public:
	bool was_set_eye_area() const;
	void set_eye_area(const MouseData & mouse_data);

	bool was_lefttop_set() const;
	void set_lefttop(const cv::Mat & cap_image);

	bool was_rightbot_set() const;
	void set_rightbot(const cv::Mat & cap_image);

	void recognize(const cv::Mat & target);
	const Coor2 & win_coor();

	EyeRecognizer();
private:
	struct Rectangle
	{
		IntCoor2 beg;
		IntCoor2 end;
	};

	bool was_set_eye_area_;
	Rectangle eye_area;	// based on the image

	Coor2 win_coor_;
	bool was_lefttop_set_;
	bool was_rightbot_set_;

	float pupil_leftest;	// most left coor of pupil which based on the image
	float pupil_topest;		// most top coor of pupil which based on the image

	float pupil_movable_width;
	float pupil_movable_height;

	cv::Mat close_image;

	float pupil_radius;

	bool get_pupil_coor(const cv::Mat & cap_image, float & x, float & y);
	int compute_pupil_coor(const cv::Mat & cap_image, float & x, float & y);
	int compute_def_close(const cv::Mat & cap_image);
	int compute_def_close(const cv::Mat & cap_image, const int plus_y);
	int compute_def_pupil(const cv::Mat & cap_image, const int x, const int y);
	float compute_length2(const IntCoor2 & one, const IntCoor2 & another);
};