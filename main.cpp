#include <iostream>
#include <vector>
#include <cmath>
#include <Windows.h>

#include <opencv2/nonfree/nonfree.hpp>
#include <opencv/highgui.h>

#include "MouseData.h"
#include "EyeRecognizer.h"
#include "MouseWrapper.h"

bool create_bmp(int width, int height, const char * file_path, const unsigned char * data_bgr);
void bgr_to_monochrome(const cv::Mat & bgr, cv::Mat & monochro);
void monochrome_to_bgr(const cv::Mat & monochro, cv::Mat & bgr);
void dif_filtering_horizon(const cv::Mat & base, cv::Mat & converted);
void dif_filtering_virtical(const cv::Mat & base, cv::Mat & converted);
void binarization(const cv::Mat & base, cv::Mat & converted);

void my_mouse_callback(int event, int x, int y, int flags, void * param);
Coor2 eye_recog(const cv::Mat & target, const MouseData & mouse_data);

int main()
{
	EyeRecognizer eyezer;

	// �ϐ��錾
	cv::Mat im;

	// �J�����̃L���v�`��
	cv::VideoCapture cap(0);
	// �L���v�`���̃G���[����
	if (!cap.isOpened())
	{
		std::cout << "�J�����Ƃ̐ڑ��Ɏ��s���܂����B�v���O�������I�����܂��B" << std::endl;
		rewind(stdin);
		getchar();
		return -1;
	}

	cap >> im;	// �ŏ��̉摜�͔j�����Ȃ��ƁA���ɂ���Ă̓G���[�ɂȂ�炵���B
	{
		cv::Mat temp;
		cap >> temp;
		cv::flip(temp, im, 1);
	}

	std::string window("capture");
	cv::namedWindow(window, CV_WINDOW_AUTOSIZE);

	MouseData mouse_data;
	mouse_data.began_drag = false;
	mouse_data.finished_drag = false;
	mouse_data.beg.x = 0;
	mouse_data.beg.y = 0;
	mouse_data.end.x = im.size().width;
	mouse_data.end.y = im.size().height;

	MouseWrapper wrapper;
	wrapper.mouse_data = &mouse_data;
	wrapper.eyezer = &eyezer;
	wrapper.cap_image = &im;
	cv::setMouseCallback(window, my_mouse_callback, reinterpret_cast<void *>(&wrapper));

	while (1)
	{
		// �J�����f���̎擾
		{
			cv::Mat temp;
			cap >> temp;
			cv::flip(temp, im, 1);
		}

		cv::rectangle(
			im,
			cv::Point2d(mouse_data.beg.x, mouse_data.beg.y),
			cv::Point2d(mouse_data.end.x, mouse_data.end.y),
			cv::Scalar(0xff, 0x00, 0x00));

		if (eyezer.was_set_eye_area() == false)
		{
			// show "draw rectangle around your an eye"
		}
		else if (eyezer.was_set_threshold() == false)
		{
			eyezer.renew_threshold();
			eyezer.show_pupil(im);
		}
		else if (eyezer.was_lefttop_set() == false)
		{
			eyezer.show_pupil(im);
			// show "look upper left on your display."
			//      "And plz press left button of your mouse."
		}
		else if (eyezer.was_rightbot_set() == false)
		{
			eyezer.show_pupil(im);
			// show "look lower right on your display."
			//      "And plz press left button of your mouse."
		}
		else
		{
			eyezer.recognize(im);
			SetCursorPos(
				static_cast<int>(round(eyezer.win_coor().x)),
				static_cast<int>(round(eyezer.win_coor().y)));
		}


		// �f���̕\��
		cv::imshow(window, im);

		// �L�[���͂�����ΏI��
		if (cv::waitKey(3) >= 0)
		{
			break;
		}
	}

	return 0;
}