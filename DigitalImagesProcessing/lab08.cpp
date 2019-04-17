#include "labs.h"


#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "background_segm.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;

// 1 background cale z czeronym, foreground gauss ghost

// transofrmacje hue, zaznacza linie

// historia 20 , mieszanie 100
// hoight linie


int motionDetectionGauss() {
	//VideoCapture vcap(1);
	VideoCapture vcap("bike.avi");

	Mat firstFrame, firstFrame2, frame, gray, dst, thresh, background, foreground, fgMaskMOG, fgMaskMOG2;

	Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	pMOG2 = createBackgroundSubtractorMOG2();

	if (!vcap.isOpened())
		return -1;

	namedWindow("Final window", 1);

	int history = 20;
	int mixtures = 100;

	createTrackbar("history", "Final window", &history, 100);
	createTrackbar("mixtures", "Final window", &mixtures, 255);

	vcap >> frame;
	cvtColor(frame, firstFrame, COLOR_BGR2GRAY);
	GaussianBlur(firstFrame, firstFrame, Size(21, 21), 0);


	while (vcap.read(frame)) {

		cvtColor(frame, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(21, 21), 0);

		// Obliczenie wartoci bezwglêdnej ró¿nicy jasnoci pomiêdzy bie¿¹cym obrazem a t³em.
		absdiff(firstFrame, gray, dst);

		//update the background model
		pMOG2->apply(frame, fgMaskMOG2);

		// Convert the image to black and white
		threshold(dst, thresh, 100, 255, THRESH_BINARY);

		gray = gray - dst;
		//gray &= dst;

		imshow("Original", frame);
		imshow("FG Mask MOG 2", fgMaskMOG2);
		imshow("dst", dst);
		imshow("gray", gray);
		imshow("thresh", thresh);
		imshow("Final window", firstFrame);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}

int houghTranform() {
	VideoCapture vcap(0);
	//VideoCapture vcap("bike.avi");

	Mat frame, gray, edges;

	int thresh = 50, minLineLenght = 10, maxLineGap = 50;

	if (!vcap.isOpened())
		return -1;

	namedWindow("Final window", 1);
	createTrackbar("Threshold", "Final window", &thresh, 255);
	createTrackbar("minLineLenghta", "Final window", &minLineLenght, 100);
	createTrackbar("maxLineGap", "Final window", &maxLineGap, 300);

	while (vcap.read(frame)) {

		cvtColor(frame, gray, COLOR_BGR2GRAY);

		// Znajdz kontury
		Canny(gray, edges, 50, 200);

		// Create a vector to store lines of the image
		vector<Vec4i> lines;
		// Apply Hough Transform
		HoughLinesP(edges, lines, 1, CV_PI / 180, thresh, minLineLenght, maxLineGap);
		// Draw lines on the image
		for (size_t i = 0; i < lines.size(); i++) {
			Vec4i l = lines[i];
			line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
		}

		imshow("edges", edges);
		imshow("Final window", frame);

		char c = (char)waitKey(33);
		if (c == 27) break;
	}

	vcap.release();
	cvDestroyAllWindows();

	return 0;
}