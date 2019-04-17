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
using std::string;


int motionDetectionAlpha() {
	VideoCapture vcap(1);
	//VideoCapture vcap("bike.avi");
	Mat firstFrame, firstFrame2, frame, gray, dst, thresh;

	if (!vcap.isOpened())
		return -1;

	vcap >> frame;
	cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
	//threshold(firstFrame, firstFrame, 127, 255, THRESH_BINARY);
	//firstFrame.convertTo(firstFrame, CV_32FC3);
	//Mat acc = Mat::zeros(frame.size(), CV_32FC1);

	namedWindow("Final window", 1);

	int alpha = 0;
	createTrackbar("Alpha", "Final window", &alpha, 100);

	int threshold_value = 100;
	createTrackbar("Threshold", "Final window", &threshold_value, 255);

	for (;;) {

		if (!vcap.read(frame)) {
			cerr << "ERROR! pusta klatka\n";
			break;
		}
		cvtColor(frame, gray, COLOR_BGR2GRAY);

		// Obliczenie wartoci bezwglêdnej ró¿nicy jasnoci pomiêdzy bie¿¹cym obrazem a t³em.
		absdiff(firstFrame, gray, dst);

		// Convert the image to black and white
		threshold(dst, thresh, threshold_value, 255, THRESH_BINARY);

		// Obliczenie redniej ruchomej dla kolejno wywietlanych obrazów
		double beta = (double)alpha / 100;
		cout << beta << endl;
		firstFrame = (1 - beta) * firstFrame + (beta * thresh);
		//accumulateWeighted(thresh, firstFrame, beta);

		//namedWindow("Original", 1);
		//imshow("Original", frame);

		namedWindow("thresh", 1);
		imshow("thresh", thresh);

		imshow("Final window", firstFrame);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}

int createVideoFromImages() {
	VideoCapture vcap("img%02d.png");
	Mat frame;

	if (!vcap.isOpened())
		return -1;

	Size S = Size((int)vcap.get(CV_CAP_PROP_FRAME_WIDTH), (int)vcap.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter video("vide2o.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, S, true);

	int counter = 1;

	for (;;)
	{
		vcap >> frame;
		if (frame.empty())
			break;

		video.write(frame);

		namedWindow("window", 1);
		imshow("window", frame);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}