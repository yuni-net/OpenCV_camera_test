#pragma once

#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Rectangle.h"
#include "Coor2.h"
#include "Array2D.h"
#include "Top4.h"

/***
�ׂ荇����f�����n���̐F���ǂ��������X�Ɣ��肵�Ă䂭���߂̃N���X�ł��B
*/
class Flooder
{
public:
	/***
	@brief ���������s���܂��Bflood�֐����ĂԑO�ɕK���Ăяo���Ă��������B
	@param
	    cap_image: �T���Ώۂ̉摜���w�肵�܂��B
		search_area: �T������͈͂��w�肵�܂��B���E�Ɉʒu�����f�ɂ��Ăł����Abeg�ʒu�͒T������Aend�ʒu�͒T������܂���B
		             ex. (1) beg(0,0) end(1,1)�̏ꍇ �� 1��f�����T������܂��B
					     (2) beg(0,0) end(0,0)�̏ꍇ �� �T��������f�͂���܂���B
		threshold: ���n�����ǂ����𔻒f����臒l���w�肵�܂��B
	*/
	void init(cv::Mat & cap_image, const my::Rectangle & search_area, const int threshold);

	/*** 
	@brief �ׂ荇����f�����n���̐F���ǂ��������X�Ɣ��肵�Ă䂫�܂��B
	@param ��Ƃ����f�̍��W���w�肵�Ă��������B
	*/
	my::Rectangle flood(const IntCoor2 & coor, Top4 & pupil_top4);



private:
	cv::Mat base_image;
	cv::Mat * cap_image;
	my::Rectangle search_area;
	Array2D<bool> did_judge_table;
	int threshold;
	my::Rectangle pupil_area;

	bool judge(const IntCoor2 & coor);
	IntCoor2 compute_local_coor(const IntCoor2 & coor);
	int compute_def(const IntCoor2 & base_coor, const IntCoor2 & target_coor);
	int index1D(const IntCoor2 & coor) const;
};