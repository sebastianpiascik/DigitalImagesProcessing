#include "labs.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <conio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <ctime>

using namespace cv;
using std::cin;
using std::cout;
using std::endl;
using std::cerr;

int motionDetection() {
	VideoCapture vcap("robot_no_loop.avi");
	Mat background = imread("logo.bmp");

	if (!vcap.isOpened())
		return -1;

	Mat firstFrame, frame, gray, dst, thresh;
	vcap >> frame;
	cvtColor(frame, firstFrame, COLOR_BGR2GRAY);


	for (;;) {

		//if (!vcap.read(frame)) {
		//	cerr << "ERROR! pusta klatka\n";
		//	break;
		//}
		//cvtColor(frame, firstFrame, COLOR_BGR2GRAY);

		if (!vcap.read(frame)) {
			cerr << "ERROR! pusta klatka\n";
			break;
		}
		cvtColor(frame, gray, COLOR_BGR2GRAY);

		absdiff(firstFrame, gray, dst);

		threshold(dst, thresh, 128, 255, THRESH_BINARY);

		namedWindow("window1", 1);
		imshow("window1", firstFrame);

		namedWindow("window2", 1);
		imshow("window2", gray);

		namedWindow("window4", 1);
		imshow("window4", thresh);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}

int motionDetectionCamera() {
	VideoCapture vcap(0);
	Mat background = imread("logo.bmp");

	if (!vcap.isOpened())
		return -1;


	// compare with first frame

	Mat firstFrame, frame, gray, dst, thresh;
	vcap >> frame;
	cvtColor(frame, firstFrame, COLOR_BGR2GRAY);


	for (;;) {

		// compare with previous frame

		//if (!vcap.read(frame)) {
		//	cerr << "ERROR! pusta klatka\n";
		//	break;
		//}
		//cvtColor(frame, firstFrame, COLOR_BGR2GRAY);

		if (!vcap.read(frame)) {
			cerr << "ERROR! pusta klatka\n";
			break;
		}
		cvtColor(frame, gray, COLOR_BGR2GRAY);

		absdiff(firstFrame, gray, dst);

		threshold(dst, thresh, 128, 255, THRESH_BINARY);

		namedWindow("window1", 1);
		imshow("window1", firstFrame);

		namedWindow("window2", 1);
		imshow("window2", gray);

		namedWindow("window4", 1);
		imshow("window4", thresh);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}

int addBackgroundLogo() {
	//VideoCapture vcap("robot_no_loop.avi");
	VideoCapture vcap(0);
	Mat background = imread("logo.bmp");
	Mat frame, watermarked;

	if (!vcap.isOpened())
		return -1;

	for (;;) {

		frame = imread("logo.bmp");

		if (!vcap.read(frame)) {
			cerr << "ERROR! pusta klatka\n";
			break;
		}

		//I want to put logo on top - left corner, So I create a ROI
		Rect rect = Rect(0, 0, background.cols, background.rows);
		Mat roi = frame(rect);
		//Now create a mask of logo and create its inverse mask also
		Mat img2gray, ret, mask, mask_inv, img1_bg, img2_fg, dst, sum;
		cvtColor(background, mask, COLOR_BGR2GRAY);
		threshold(mask, mask, 100, 255, THRESH_BINARY);
		bitwise_not(mask, mask_inv);
		//Now black - out the area of logo in ROI
		bitwise_and(roi, roi, img1_bg, mask_inv);
		bitwise_and(background, background, img2_fg, mask);
		//Put logo in ROI and modify the main image
		add(img1_bg, img2_fg, sum);

		dst = frame.clone();
		for (int i = 0; i < background.rows; i++) {
			for (int j = 0; j < background.cols * 3; j++) {
				dst.at<uchar>(i, j) = sum.at<uchar>(i, j);
			}
		}

		namedWindow("window1", 1);
		imshow("window1", dst);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}