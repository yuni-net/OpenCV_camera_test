#pragma once

#include "Coor2.h"
#include "MouseData.h"
#include "Rectangle.h"
#include "Top4.h"

class EyeRecognizer
{
public:
	bool was_set_eye_area() const;
	void set_eye_area(const MouseData & mouse_data);

	void show_pupil(cv::Mat & cap_image);

	bool was_set_threshold() const;
	void renew_threshold();
	void set_threshold();

	bool was_lefttop_set() const;
	void set_lefttop(cv::Mat & cap_image);

	bool was_rightbot_set() const;
	void set_rightbot(cv::Mat & cap_image);

	void recognize(cv::Mat & target);
	const Coor2 & win_coor();

	EyeRecognizer();
private:

	bool was_set_eye_area_;
	my::Rectangle eye_area;	// based on the image
	my::Rectangle pupil_area;
	Top4 pupil_top4;

	Coor2 win_coor_;
	bool was_set_threshold_;
	bool was_lefttop_set_;
	bool was_rightbot_set_;

	float pupil_leftest;	// most left coor of pupil which based on the image
	float pupil_topest;		// most top coor of pupil which based on the image

	float pupil_movable_width;
	float pupil_movable_height;

	cv::Mat close_image;

	float pupil_radius;
	int threshold;

	float total_radius;
	int sampling_num;

	/***
	@return true...ウィンクしていない　false...ウィンクしている
	*/
	bool get_pupil_coor(cv::Mat & cap_image, float & x, float & y);

	int compute_pupil_coor(const cv::Mat & cap_image, float & x, float & y);
	int compute_def_close(const cv::Mat & cap_image);
	int compute_def_close(const cv::Mat & cap_image, const int plus_y);
	int compute_def_pupil(const cv::Mat & cap_image, const int x, const int y);
	float compute_length2(const IntCoor2 & one, const IntCoor2 & another);
	bool is_not_black(const cv::Mat & cap_image, const IntCoor2 & coor) const;
	IntCoor2 find_black(const cv::Mat & cap_image, const IntCoor2 & coor) const;
	bool search_black_outline(const cv::Mat & cap_image, const IntCoor2 & coor, const int outline_size, IntCoor2 & searched_coor) const;

	/***
	@return true...ウィンクしていない　false...ウィンクしている
	*/
	bool compute_pupil_coor(const my::Rectangle & pupil_area, const Top4 & pupil_top4, float & x, float & y);
};