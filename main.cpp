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

#if 0
	cv::Mat base_im;
	base_im = cv::imread("perfectly_opened.bmp");

#else
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
	cap >> im;
#endif

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
#if 1
		// �J�����f���̎擾
		cap >> im;
#else
		base_im.copyTo(im);
#endif

		if (eyezer.was_set_eye_area() == false)
		{
			cv::rectangle(
				im,
				cv::Point2d(mouse_data.beg.x, mouse_data.beg.y),
				cv::Point2d(mouse_data.end.x, mouse_data.end.y),
				cv::Scalar(0xff, 0x00, 0x00));
		}
		else if (eyezer.was_lefttop_set() == false)
		{
			cv::rectangle(
				im,
				cv::Point2d(mouse_data.beg.x, mouse_data.beg.y),
				cv::Point2d(mouse_data.end.x, mouse_data.end.y),
				cv::Scalar(0xff, 0x00, 0x00));

			eyezer.show_pupil(im);

			// show "look upper left on your display."
			//      "And plz press left button of your mouse."
		}
		else if (eyezer.was_rightbot_set() == false)
		{
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

#if 0
		if (mouse_data.finished_drag)
		{
			eyezer.recognize(im, mouse_data);
			SetCursorPos(static_cast<int>(eyezer.win_coor().x), static_cast<int>(eyezer.win_coor().y));
		}

		cv::Mat temp;
		im.copyTo(temp);
		cv::rectangle(
			temp,
			cv::Point2d(mouse_data.beg.x, mouse_data.beg.y),
			cv::Point2d(mouse_data.end.x, mouse_data.end.y),
			cv::Scalar(0xff, 0x00, 0x00));

		// �f���̕\��
		cv::imshow(window, temp);
#else

		// �f���̕\��
		cv::imshow(window, im);
#endif

		// �L�[���͂�����ΏI��
		if (cv::waitKey(3) >= 0)
		{
			break;
		}
	}

	return 0;
}