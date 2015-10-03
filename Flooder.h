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
隣り合う画素が同系統の色かどうかを次々と判定してゆくためのクラスです。
*/
class Flooder
{
public:
	/***
	@brief 初期化を行います。flood関数を呼ぶ前に必ず呼び出してください。
	@param
	    cap_image: 探索対象の画像を指定します。
		search_area: 探索する範囲を指定します。境界に位置する画素についてですが、beg位置は探索され、end位置は探索されません。
		             ex. (1) beg(0,0) end(1,1)の場合 → 1画素だけ探索されます。
					     (2) beg(0,0) end(0,0)の場合 → 探索される画素はありません。
		threshold: 同系統かどうかを判断する閾値を指定します。
	*/
	void init(cv::Mat & cap_image, const my::Rectangle & search_area, const int threshold);

	/*** 
	@brief 隣り合う画素が同系統の色かどうかを次々と判定してゆきます。
	@param 基準とする画素の座標を指定してください。
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