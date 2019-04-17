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
using namespace std;

int negativeImage() {
	VideoCapture vcap(0);

	if (!vcap.isOpened())
		return -1;

	namedWindow("Display window", 1);
	namedWindow("Display window 2", 1);

	for (;;) {

		Mat frame, grayscale;
		vcap >> frame;
		imshow("Display window", frame);

		for (int i = 0; i < frame.rows; i++)
		{
			for (int j = 0; j < frame.cols * 3; j++)
			{
				frame.at<uchar>(i, j) = 255 - frame.at<uchar>(i, j);
			}
		}

		cvtColor(frame, grayscale, CV_RGB2GRAY);

		imshow("Display window 2", grayscale);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}


int videoSaveFrames() {
	//VideoCapture vcap("snake_synth.avi");
	VideoCapture vcap(0);

	if (!vcap.isOpened())
		return -1;

	namedWindow("window", 1);

	double frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	double frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	//VideoWriter video("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);

	int time, amountFrames = 10;
	int choice;
	cout << "Menu\n";
	cout << "1 - 1sec\n";
	cout << "2 - 2sec\n";
	cout << "3 - 3sec\n";
	cout << "Selection: ";
	cin >> choice;

	switch (choice) {
	case 1:
		time = 1;
		break;
	case 2:
		time = 2;
		break;
	case 3:
		time = 3;
		break;
	default:
		time = 1;
	}

	cout << "Menu\n";
	cout << "Liczba klatek: ";
	cin >> choice;

	amountFrames = choice;


	std::clock_t start;
	double duration;

	start = std::clock();

	for (;;) {

		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;


		Mat frame;
		vcap >> frame;

		std::ostringstream oss;
		if (amountFrames < 10) {
			oss << "img0" << amountFrames << ".png";
		}
		else {
			oss << "img" << amountFrames << ".png";
		}
		std::string imageFileName = oss.str();

		imwrite(imageFileName, frame);

		imshow("window", frame);

		char c = (char)waitKey(33);
		if (c == 27) break;

		std::cout << "printf: " << duration << ", amountFrames: " << amountFrames << '\n';
		if (duration > time) {
			amountFrames--;
			//video.write(frame);
			if (amountFrames == 0) break;
		}
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}